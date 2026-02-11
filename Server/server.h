#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QByteArray>
#include <QLoggingCategory>
#include <QDebug>


class Server : public QTcpServer
{
    Q_OBJECT

public:
    explicit Server(QObject *parent = nullptr);
    ~Server() = default;

    bool start(const quint16 port);

private slots:
    void slotNewConnection();
    void onReadyRead();
    void onClientDisconnected();
    void onSocketError(QAbstractSocket::SocketError socketError);

private:
    quint8 processByte(quint8 inputByte);
};

#endif // SERVER_H
