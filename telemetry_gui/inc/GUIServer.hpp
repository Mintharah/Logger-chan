#pragma once

#include "GUISink.hpp"
#include "LogManager.hpp"

#include <QObject>
#include <QString>
#include <atomic>
#include <memory>
#include <thread>
#include <vsomeip/vsomeip.hpp>

#define GUI_SERVICE_ID   0x1234
#define GUI_INSTANCE_ID  0x5678
#define GUI_METHOD_ID    0x0125

class GUIServer : public QObject {
    Q_OBJECT
public:
    explicit GUIServer(QObject *parent = nullptr);
    ~GUIServer();

    void start(const QString &vsomeipConfig,
               bool   consoleSink,
               bool   fileSink,
               const QString &filePath,
               size_t bufferSize,
               GUISink *guiSink);

    void stop();

    bool   isRunning()   const;
    size_t bufferUsed()  const;
    size_t bufferCap()   const;
    size_t totalLogged() const;

signals:
    void stopped();
    void errorOccurred(const QString &msg);

private:
    void run(const QString &vsomeipConfig,
             bool consoleSink, bool fileSink,
             const QString &filePath, size_t bufferSize,
             GUISink *guiSink);

    std::thread                            m_thread;
    std::atomic<bool>                      m_running{false};
    std::shared_ptr<vsomeip::application>  m_app;
    std::unique_ptr<LogManager>            m_logMgr;
    std::atomic<size_t>                    m_totalLogged{0};
};
