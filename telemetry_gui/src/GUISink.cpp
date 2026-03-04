#include "GUISink.hpp"
#include "LogMessage.hpp"

GUISink::GUISink(QObject *parent) : QObject(parent) {}

void GUISink::writeMessage(const LogMessage &msg) {
    emit messageReceived(
        QString::fromStdString(msg.app_name),
        QString::fromStdString(msg.context),
        QString::fromStdString(msg.severity),
        QString::fromStdString(msg.time),
        QString::fromStdString(msg.text)
    );
}
