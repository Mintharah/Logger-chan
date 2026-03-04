#include "GUIServer.hpp"

#include "CPUTelemetrySource.hpp"
#include "ConsoleSinkImpl.hpp"
#include "FileSinkImpl.hpp"
#include "ILogSink.hpp"
#include "LogFormatter.hpp"
#include "Policies.hpp"

#include <vsomeip/vsomeip.hpp>
#include <stdexcept>

// ─── Constructor / Destructor ─────────────────────────────────────────────────

GUIServer::GUIServer(QObject *parent) : QObject(parent) {}

GUIServer::~GUIServer() { stop(); }

// ─── Public API ───────────────────────────────────────────────────────────────

void GUIServer::start(const QString &vsomeipConfig,
                      bool   consoleSink,
                      bool   fileSink,
                      const QString &filePath,
                      size_t bufferSize,
                      GUISink *guiSink)
{
    stop();
    m_running = true;
    m_thread  = std::thread([=]() {
        try {
            run(vsomeipConfig, consoleSink, fileSink, filePath, bufferSize, guiSink);
        } catch (const std::exception &e) {
            emit errorOccurred(QString("Server error: %1").arg(e.what()));
        }
        m_running = false;
        emit stopped();
    });
}

void GUIServer::stop() {
    m_running = false;
    if (m_app) { m_app->stop(); m_app.reset(); }
    if (m_thread.joinable()) m_thread.join();
}

bool   GUIServer::isRunning()   const { return m_running.load(); }
size_t GUIServer::bufferUsed()  const { return m_logMgr ? m_logMgr->bufferSize()     : 0; }
size_t GUIServer::bufferCap()   const { return m_logMgr ? m_logMgr->bufferCapacity() : 0; }
size_t GUIServer::totalLogged() const { return m_totalLogged.load(); }

// ─── Private: server thread ───────────────────────────────────────────────────

void GUIServer::run(const QString &vsomeipConfig,
                    bool consoleSink, bool fileSink,
                    const QString &filePath, size_t bufferSize,
                    GUISink *guiSink)
{
    // ── LogManager ────────────────────────────────────────────────────────────
    m_logMgr = std::make_unique<LogManager>(bufferSize);

    if (consoleSink)
        m_logMgr->addSink(std::make_unique<ConsoleSinkImpl>());

    if (fileSink) {
        auto fs = std::make_unique<FileSinkImpl>();
        fs->filePath(filePath.toStdString());
        m_logMgr->addSink(std::move(fs));
    }

    if (guiSink) {
        // Non-owning adapter — LogManager holds a unique_ptr but must not
        // delete the GUISink (which is owned by the Qt object tree).
        struct Adapter : public ILogSink {
            ILogSink *s;
            explicit Adapter(ILogSink *s) : s(s) {}
            void writeMessage(const LogMessage &m) override { s->writeMessage(m); }
        };
        m_logMgr->addSink(std::make_unique<Adapter>(guiSink));
    }

    // ── CPU source ────────────────────────────────────────────────────────────
    CPUTelemtrySourceImpl cpuSource;
    if (!cpuSource.openSource())
        throw std::runtime_error("Failed to open /proc/stat");

    // ── vsomeip app ───────────────────────────────────────────────────────────
    setenv("VSOMEIP_CONFIGURATION", vsomeipConfig.toStdString().c_str(), 1);

    m_app = vsomeip::runtime::get()->create_application("Telemetry_Server");
    m_app->init();

    LogManager        *lm  = m_logMgr.get();
    std::atomic<size_t> *cnt = &m_totalLogged;

    m_app->register_message_handler(
        GUI_SERVICE_ID, GUI_INSTANCE_ID, GUI_METHOD_ID,
        [this, &cpuSource, lm, cnt]
        (std::shared_ptr<vsomeip::message> request)
        {
            std::string cpuData;
            if (!cpuSource.readSource(cpuData)) return;

            // Log through the full LogManager pipeline (→ GUISink → GUI)
            auto msgOpt = LogFormatter<CpuPolicy>::formatDataToLogMsg(cpuData);
            if (msgOpt) { ++(*cnt); lm->log(std::move(*msgOpt)); }

            // Send SOME/IP response back to the RPi client
            auto payload = vsomeip::runtime::get()->create_payload();
            payload->set_data(
                reinterpret_cast<const vsomeip::byte_t*>(cpuData.data()),
                static_cast<vsomeip::length_t>(cpuData.size()));
            auto response = vsomeip::runtime::get()->create_response(request);
            response->set_payload(payload);
            m_app->send(response);
        });

    m_app->offer_service(GUI_SERVICE_ID, GUI_INSTANCE_ID);
    m_app->start();   // blocks until stop() calls m_app->stop()
}
