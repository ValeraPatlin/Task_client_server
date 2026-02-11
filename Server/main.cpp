#include <QCoreApplication>
#include "../Message_handler/message_handler.h"
#include "server.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Server server;
    quint16 port = 2345;

    if (server.start(port))
    {
        qDebug() << "The server is running successfully. Press Ctrl+C to exit";
        MessageHandler::getInstance("./server.log", true, MessageHandler::MsgType::DebugMsg);
        return a.exec();
    }
    else
    {
        qCritical() << "The server could not be started. Check the port number and permissions";
        return -1;
    }
}
