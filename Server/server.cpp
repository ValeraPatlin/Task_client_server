#include "server.h"

Q_DECLARE_LOGGING_CATEGORY(SentServer)
Q_DECLARE_LOGGING_CATEGORY(infoServer)
Q_LOGGING_CATEGORY(SentServer, "sent server");
Q_LOGGING_CATEGORY(infoServer, "info server");

Server::Server(QObject *parent)
    : QTcpServer{parent}
{
    connect(this, &Server::newConnection, this, &Server::slotNewConnection);
}

bool Server::start(const quint16 port)
{
    if (QTcpServer::listen(QHostAddress::Any, port))
    {
        qDebug(infoServer) << "The server is running on the port: " << port;
        return true;
    }
    else
    {
        qCritical(infoServer) << "Server startup error: " << errorString();
        return false;
    }
}

void Server::slotNewConnection()
{
    QTcpSocket *clientSocket = nextPendingConnection();

    if (clientSocket)
    {
        qDebug(infoServer) << "New client connection: " << clientSocket->peerAddress().toString()
                           << " port:" << clientSocket->peerPort();

        connect(clientSocket, &QTcpSocket::readyRead, this, &Server::onReadyRead);
        connect(clientSocket, &QTcpSocket::disconnected, this, &Server::onClientDisconnected);
        connect(clientSocket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
                this, &Server::onSocketError);
    }
    else
    {
        qWarning(infoServer) << "Failed to create a QTcpSocket for a new connection";
    }
}

void Server::onReadyRead()
{
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) return;

    QByteArray receivedData = clientSocket->readAll();

    QByteArray processedData;
    processedData.reserve(receivedData.size());

    for (int i = 0; i < receivedData.size(); ++i)
    {
        quint8 inputByte = static_cast<quint8>(receivedData[i]);
        quint8 processedByte = processByte(inputByte);
        processedData.append(static_cast<char>(processedByte));
    }

    clientSocket->write(processedData);
    clientSocket->flush();

    qDebug(SentServer) << "Received:" << receivedData.toHex(' ')
                       << "Processed:" << processedData.toHex(' ');
}

void Server::onClientDisconnected()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (clientSocket)
    {
        qDebug(infoServer) << "The client has disconnected: " << clientSocket->peerAddress().toString()
                           << " port: " << clientSocket->peerPort();
        clientSocket->deleteLater();
    }
}

void Server::onSocketError(QAbstractSocket::SocketError socketError)
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (clientSocket)
    {
        qWarning(infoServer) << "Ошибка сокета для клиента " << clientSocket->peerAddress().toString()
                             << " port:" << clientSocket->peerPort() << ":" << clientSocket->errorString();
        clientSocket->close();
    }
}

quint8 Server::Server::processByte(quint8 inputByte)
{
    quint8 msbInverted = inputByte ^ 0x80;
    quint8 lsbInverted = msbInverted ^ 0x01;
    quint8 result = lsbInverted ^ 0xA5;
    return result;
}
