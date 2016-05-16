#include "clientsocket.h"

ClientSocket::ClientSocket(QObject *parent)
    : QSslSocket(parent), connectionSlot(0)
{
    connect(this, SIGNAL(readyRead()), SLOT(readData()));
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(error(QAbstractSocket::SocketError)));
}

void ClientSocket::setSlot(int connectionSlot)
{
    this->connectionSlot = connectionSlot;
}

int ClientSocket::getSlot()
{
    return connectionSlot;
}

void ClientSocket::readData()
{
    QByteArray receivedMessage;
    QByteArray fullMessage;

    do {
        receivedMessage = readAll();

        if (!receivedMessage.isEmpty())
            fullMessage += receivedMessage;

    } while (!receivedMessage.isEmpty());

    emit messageReceived(&fullMessage);
}

void ClientSocket::error(QAbstractSocket::SocketError)
{
    qDebug("Error: %s", qPrintable(this->errorString()));
}
