#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H


#include <QObject>
#include <QCoreApplication>
#include <QLoggingCategory>
#include <QMessageLogContext>
#include <QTextStream>
#include <QDateTime>
#include <QString>
#include <QFile>
#include <QMutex>
#include <QDebug>
#include <memory>


class MessageHandler final : public QObject
{
    Q_OBJECT

public:
    enum class MsgType : int
    {
        FatalMsg = 0,
        CriticalMsg = 1,
        WarningMsg = 2,
        InfoMsg = 3,
        DebugMsg = 4,
    };

    static MessageHandler* getInstance(const QString &fileName = "log.txt",
                                       bool writeFlag = true,
                                       MsgType level = MsgType::DebugMsg,
                                       QObject *parent = nullptr);

    MessageHandler(const MessageHandler&) = delete;
    MessageHandler& operator= (const MessageHandler&) = delete;
    MessageHandler(MessageHandler&&) = delete;
    MessageHandler& operator= (MessageHandler&&) = delete;

    ~MessageHandler();

    /*!
     * \brief setWriteFileFlag - метод определяет куда надо выводить данные
     * (в консоль либо в указанный файл)
     * \param flag: false - данные выводяться в консоль
     * true - данные выводяться в указаный файл
     */
    //void setWriteFileFlag(bool flag) noexcept;
    void setWriteFileFlag(const bool flag);

    /*!
     * \brief setDebugLevel - метод определяет уровень данных для вывода
     * \param level
     */
    void setDebugLevel(const MsgType &level);

    /*!
     * \brief messageHandler - метод для переопределения вывода потока данных
     * \param type
     * \param context
     * \param msg
     */
    static void messageHendler(QtMsgType type,
                               const QMessageLogContext& context,
                               const QString& msg) noexcept;

signals:
    void criticalError();
    void criticalErrorInfo(const QString& msg);

private:
    explicit MessageHandler(const QString& fileName, bool writeFlag,
                            const MsgType& level, QObject *parent = nullptr);

    //static inline MessageHandler* instance = nullptr;
    static MessageHandler* instance;

    void openFileLog();

    void writeMessageType(QTextStream& out, const QMessageLogContext& context,
                      const QString& msg, const QtMsgType& type);

    void writeMessage(QTextStream& out, const QMessageLogContext& context,
                      const QString &msg, const QString& msgTypeStr, const MsgType &msgLevel);

    QString fileNameLog;
    std::unique_ptr<QFile> fileLog;
    QMutex mutex;

    bool writeFileFlag{};
    MsgType debugLevel{};
};

#endif // MESSAGE_HANDLER_H
