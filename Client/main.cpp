#include "window.h"
#include "../Message_handler/message_handler.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MessageHandler::getInstance("client.log", true, MessageHandler::MsgType::DebugMsg);

    Window w;
    w.show();

    return a.exec();
}
