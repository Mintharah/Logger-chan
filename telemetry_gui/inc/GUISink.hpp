#pragma once
#include "ILogSink.hpp"
#include <QObject>
#include <QString>

// Sits alongside ConsoleSinkImpl inside LogManager.
// writeMessage() is called from the LogManager worker thread;
// the QueuedConnection ensures the slot runs safely on the GUI thread.
class GUISink : public QObject, public ILogSink {
    Q_OBJECT
public:
    explicit GUISink(QObject *parent = nullptr);

    void writeMessage(const LogMessage &msg) override;

signals:
    void messageReceived(const QString &app, const QString &ctx,
                         const QString &sev, const QString &time,
                         const QString &text);
};
