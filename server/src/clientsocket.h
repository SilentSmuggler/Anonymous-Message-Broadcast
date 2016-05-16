#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <QtNetwork/QSslSocket>

class ClientSocket : public QSslSocket
 {
     Q_OBJECT

public:
    ClientSocket(QObject *parent = 0);
    void setSlot(int connectionSlot);
    int getSlot();

private:
    int connectionSlot;

private slots:
    void readData();
    void error(QAbstractSocket::SocketError err);

signals:
    void messageReceived(QByteArray *message);
};

#endif // CLIENTSOCKET_H
