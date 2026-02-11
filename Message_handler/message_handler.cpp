#include "message_handler.h"

Q_DECLARE_LOGGING_CATEGORY(InfoMessageHendler)
Q_LOGGING_CATEGORY(InfoMessageHendler, "Info log message");

MessageHandler* MessageHandler::instance = nullptr;

MessageHandler::MessageHandler(const QString &fileName,
                               bool writeFlag, const MsgType &level, QObject *parent)
    : QObject{parent}, fileNameLog{fileName},
      fileLog{std::make_unique<QFile>(fileName)},
      writeFileFlag{writeFlag},
      debugLevel{level}
{
    openFileLog();
}

MessageHandler *MessageHandler::getInstance(const QString &fileName,
                                            bool writeFlag, MessageHandler::MsgType level,
                                            QObject *parent)
{
    if (instance == nullptr)
    {
        instance = new MessageHandler{fileName, writeFlag, level, parent};

        connect(instance, &MessageHandler::criticalError,
                QCoreApplication::instance(), &QCoreApplication::quit, Qt::QueuedConnection);

        qInstallMessageHandler(MessageHandler::messageHendler);
    }
    return instance;
}

MessageHandler::~MessageHandler()
{
    instance = nullptr;
}

void MessageHandler::openFileLog()
{
    if (writeFileFlag && !fileLog->isOpen())
    {
        if(!fileLog->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        {
            emit criticalErrorInfo("ERROR!!! Log file is not open! FileName: "
                                   + fileNameLog + "description: " + fileLog->errorString());
        }
    }
}


void MessageHandler::setWriteFileFlag(const bool flag)
{
    QMutexLocker locker(&mutex);
    writeFileFlag = flag;

    if (writeFileFlag && !fileLog->isOpen())
    {
        openFileLog();
    }
}

void MessageHandler::setDebugLevel(const MessageHandler::MsgType &level)
{
    QMutexLocker locker(&mutex);
    debugLevel = level;
}


void MessageHandler::messageHendler(QtMsgType type,
                                    const QMessageLogContext &context,
                                    const QString &msg) noexcept
{
    if (!MessageHandler::getInstance()->fileLog.get())
    {
        qDebug(InfoMessageHendler) << "Logger not initialized. Logging to console via qDebug.";

        QTextStream out(stdout);
        MessageHandler::getInstance()->writeMessageType(out, context, msg, type);
        return;
    }

    QMutexLocker locker(&MessageHandler::getInstance()->mutex);

    if (MessageHandler::getInstance()->writeFileFlag)
    {
        if (!MessageHandler::getInstance()->fileLog->isOpen())
        {
            QTextStream out(stdout);
            MessageHandler::getInstance()->writeMessageType(out, context, msg, type);

            qWarning(InfoMessageHendler) << "Log file '" << MessageHandler::getInstance()->fileNameLog
                       << "' is not open. Logging to console instead.";
        }
        else
        {
            QTextStream out(MessageHandler::getInstance()->fileLog.get());
            MessageHandler::getInstance()->writeMessageType(out, context, msg, type);
        }
    }
    else
    {
        QTextStream out(stdout);
        MessageHandler::getInstance()->writeMessageType(out, context, msg, type);
    }
}


void MessageHandler::writeMessageType(QTextStream &out, const QMessageLogContext &context,
                                      const QString &msg, const QtMsgType &type)
{
    QString msgTypeStr;
    MsgType msgLevel;

    switch (type)
    {
        case QtDebugMsg:
        {
            msgLevel = MsgType::DebugMsg;
            msgTypeStr = "DEBUG";
            break;
        }

        case QtInfoMsg:
        {
            msgLevel = MsgType::InfoMsg;
            msgTypeStr = "INFO";
            break;
        }

        case QtWarningMsg:
        {
            msgLevel = MsgType::WarningMsg;
            msgTypeStr = "WARNING";
            break;
        }

        case QtCriticalMsg:
        {
            msgLevel = MsgType::CriticalMsg;
            msgTypeStr = "CRITICAL";
            break;
        }

        case QtFatalMsg:
        {
            msgLevel = MsgType::FatalMsg;
            msgTypeStr = "FATAL";
            break;
        }

        default:
        {
            msgLevel = MsgType::DebugMsg;
            msgTypeStr = "UNKNOWN";
            break;
        }
    }
    writeMessage(out, context, msg, msgTypeStr, msgLevel);
}

void MessageHandler::writeMessage(QTextStream &out, const QMessageLogContext &context,
                                  const QString &msg, const QString &msgTypeStr,
                                  const MessageHandler::MsgType &msgLevel)
{
    if (debugLevel >= msgLevel)
    {
        QString formattedMsg;
        if (msgLevel == MsgType::CriticalMsg || msgLevel == MsgType::FatalMsg)
        {
            formattedMsg = "\n\n" + QString(50, '*').toUpper() + "\n"
                           + "\t\t CRITICAL ERROR!!! \n"
                           + QString(50, '*').toUpper() + "\n";
        }

        formattedMsg += QString("[%1] ").arg(static_cast<int>(msgLevel));

        formattedMsg.replace(QString("[%1]").arg(static_cast<int>(msgLevel)), msgTypeStr);

        formattedMsg += QString("Timestamp: %1 ")
                        .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"));

        formattedMsg += QString("Category: %1, ").arg(context.category);

        formattedMsg += QString("File: %1:%2, ").arg(context.file).arg(context.line);

        formattedMsg += QString("Function: %1, ").arg(context.function);

        formattedMsg += QString("Msg: %1\n").arg(msg);

        out << formattedMsg;
        out.flush();
    }

    if (msgLevel == MsgType::CriticalMsg || msgLevel == MsgType::FatalMsg)
    {
        emit criticalError();
        emit criticalErrorInfo("Critical Error: " + msg);
    }
}
