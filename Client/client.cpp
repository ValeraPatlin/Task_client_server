#include "client.h"

Q_DECLARE_LOGGING_CATEGORY(SentClient)
Q_DECLARE_LOGGING_CATEGORY(infoClient)
Q_LOGGING_CATEGORY(SentClient, "sent client");
Q_LOGGING_CATEGORY(infoClient, "info client");

Client::Client(QObject *parent)
    : QTcpSocket{parent}
{
    connect(this, &Client::connected, this, &Client::onConnected);
    connect(this, &Client::disconnected, this, &Client::onDisconnected);
    connect(this, &Client::readyRead, this, &Client::onReadyRead);
    connect(this, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
                    this, &Client::onSocketError);
}


void Client::setSizePackageData(const int size)
{
    sizePackageData = size;
}

bool Client::connectToServer(const QString &address, quint16 port)
{
    qInfo(infoClient) << "An attempt to connect to ip" << address << "port: " << port;
    connectToHost(address, port);
    return true;
}

void Client::disconnectFromServer()
{
    if (state() == ConnectedState)
    {
        qInfo(infoClient) << "Disconnecting from the server";
        disconnectFromHost();
    }
}

void Client::onConnected()
{
    qInfo(infoClient) << "Successfully connected to the server";
    emit connectedToServer(true);
    emit printInfo("connect  Ok!\n", QColor("#5fe915"));
}

void Client::onDisconnected()
{
    qInfo(infoClient) << "Disconnect from the server";
    emit disconnectedFromServer(false);
    emit printInfo("disconnected  Ok!\n", QColor("#5fe915"));
}

void Client::onSocketError(QAbstractSocket::SocketError socketError)
{
    qWarning(infoClient) << "Error socket: " << errorString();
    emit printInfo("Error socket: " + errorString() + "\n", QColor("#ff0000"));
}



void Client::sendData(const QByteArray &data)
{
    if (state() == ConnectedState)
    {
        QByteArray byte;
        int index{};

        for (int i = 0; i < data.size(); ++i)
        {
            byte.append(data.at(i));
            ++index;

            if (index == sizePackageData || i == data.size() - 1)
            {
                write(byte);
                flush();
                qDebug(SentClient) << "Sent:" << byte.toHex(' ');
                index = 0;
                byte.clear();
            }
        }
    }
    else
    {
        qWarning(infoClient) << "Unable to send data: the client is not connected";
    }
}

void Client::onReadyRead()
{
    QByteArray receivedData = readAll();
    qDebug(SentClient) << "Received from server:" << receivedData.toHex(' ');
    emit receivedDataFromServer(receivedData);
}
