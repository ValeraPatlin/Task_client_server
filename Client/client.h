#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include <QHostAddress>
#include <QDataStream>
#include <QString>
#include <QColor>
#include <QByteArray>
#include <QLoggingCategory>
#include <QDebug>


class Client : public QTcpSocket
{
    Q_OBJECT

public:
    explicit Client(QObject *parent = nullptr);
    ~Client() = default;

    void setSizePackageData(const int size);

    bool connectToServer(const QString &address, quint16 port);
    void disconnectFromServer();
    void sendData(const QByteArray &data);

signals:
    void connectedToServer(const bool flag);
    void disconnectedFromServer(const bool flag);
    void receivedDataFromServer(const QByteArray& data);
    void printInfo(const QString& test, const QColor& color);

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onSocketError(QAbstractSocket::SocketError socketError);

private:
    int sizePackageData{};
};

#endif // CLIENT_H
