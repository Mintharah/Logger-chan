#pragma once

// ─────────────────────────────────────────────────────────────────────────────
// ServerBridge
//
// Wraps the real SOMEIP_Server logic. Runs vsomeip on a background std::thread
// so Qt's event loop stays free. Every CPU reading served to the RPi client
// is also passed through LogManager (with GUISink), so the GUI receives it.
// ─────────────────────────────────────────────────────────────────────────────

#include "GUISink.hpp"
#include "LogManager.hpp"
#include "LogFormatter.hpp"
#include "Policies.hpp"
#include "ConsoleSinkImpl.hpp"
#include "FileSinkImpl.hpp"
#include "CPUTelemetrySource.hpp"

#include <vsomeip/vsomeip.hpp>
#include <QObject>
#include <QString>
#include <atomic>
#include <memory>
#include <thread>
#include <cstdlib>   // setenv

#define SERVICE_ID   0x1234
#define INSTANCE_ID  0x5678
#define METHOD_ID    0x0125

class ServerBridge : public QObject {
    Q_OBJECT

public:
    explicit ServerBridge(QObject *parent = nullptr) : QObject(parent) {}
    ~ServerBridge() { stop(); }

    // ── Lifecycle ─────────────────────────────────────────────────────────────
    void start(const QString &vsomeipConfigPath,
               bool consoleEnabled,
               bool fileEnabled,
               const QString &filePath,
               size_t bufferSize,
               GUISink *guiSink)
    {
        stop();
        m_running = true;

        // Build LogManager with the requested sinks + GUI sink
        m_logManager = std::make_unique<LogManager>(bufferSize);

        if (consoleEnabled)
            m_logManager->addSink(std::make_unique<ConsoleSinkImpl>());

        if (fileEnabled) {
            auto fs = std::make_unique<FileSinkImpl>();
            fs->filePath(filePath.toStdString());
            m_logManager->addSink(std::move(fs));
        }

        // Non-owning wrapper — LogManager holds a unique_ptr but does not
        // delete the GUISink (owned by MainWindow via Qt parent chain)
        struct GuiAdapter : public ILogSink {
            ILogSink *s;
            explicit GuiAdapter(ILogSink *sink) : s(sink) {}
            void writeMessage(const LogMessage &m) override { s->writeMessage(m); }
        };
        m_logManager->addSink(std::make_unique<GuiAdapter>(guiSink));

        m_thread = std::thread([this, vsomeipConfigPath]() {
            try {
                runServer(vsomeipConfigPath);
            } catch (const std::exception &e) {
                emit errorOccurred(QString("Server error: %1").arg(e.what()));
            }
            m_running = false;
            emit stopped();
        });
    }

    void stop() {
        m_running = false;
        if (m_app) {
            m_app->stop_offer_service(SERVICE_ID, INSTANCE_ID);
            m_app->stop();
            m_app.reset();
        }
        if (m_thread.joinable()) m_thread.join();
        m_logManager.reset();
    }

    bool   isRunning()     const { return m_running.load(); }
    size_t bufferUsed()    const { return m_logManager ? m_logManager->bufferSize()     : 0; }
    size_t bufferCapacity()const { return m_logManager ? m_logManager->bufferCapacity() : 0; }
    size_t totalServed()   const { return m_totalServed.load(); }
    size_t totalFailed()   const { return m_totalFailed.load(); }

signals:
    void stopped();
    void errorOccurred(const QString &msg);

private:
    void runServer(const QString &cfgPath) {
        // Point vsomeip at the server.json before creating the application
        setenv("VSOMEIP_CONFIGURATION", cfgPath.toStdString().c_str(), 1);

        m_cpuSource = std::make_unique<CPUTelemtrySourceImpl>();
        if (!m_cpuSource->openSource()) {
            emit errorOccurred("Failed to open /proc/stat — cannot read CPU data");
            m_running = false;
            return;
        }

        m_app = vsomeip::runtime::get()->create_application("Telemetry_Server");
        m_app->init();

        m_app->register_message_handler(
            SERVICE_ID, INSTANCE_ID, METHOD_ID,
            [this](std::shared_ptr<vsomeip::message> request) {
                onRequest(request);
            }
        );

        m_app->offer_service(SERVICE_ID, INSTANCE_ID);
        m_app->start();   // blocks until stop() calls app->stop()
    }

    void onRequest(const std::shared_ptr<vsomeip::message> &request) {
        std::string cpuData;
        if (!m_cpuSource || !m_cpuSource->readSource(cpuData)) {
            ++m_totalFailed;
            return;
        }

        // Send SomeIP response back to the RPi client
        auto payload = vsomeip::runtime::get()->create_payload();
        payload->set_data(
            reinterpret_cast<const vsomeip::byte_t*>(cpuData.data()),
            static_cast<vsomeip::length_t>(cpuData.size())
        );
        auto response = vsomeip::runtime::get()->create_response(request);
        response->set_payload(payload);
        m_app->send(response);

        // Also log it — flows to ConsoleSink, FileSink, and GUISink
        auto msg = LogFormatter<CpuPolicy>::formatDataToLogMsg(cpuData);
        if (msg && m_logManager)
            m_logManager->log(std::move(*msg));

        ++m_totalServed;
    }

    std::shared_ptr<vsomeip::application>  m_app;
    std::unique_ptr<LogManager>            m_logManager;
    std::unique_ptr<CPUTelemtrySourceImpl> m_cpuSource;
    std::thread                            m_thread;
    std::atomic<bool>                      m_running{false};
    std::atomic<size_t>                    m_totalServed{0};
    std::atomic<size_t>                    m_totalFailed{0};
};
