#ifndef ANONYMOUSSERVER_H
#define ANONYMOUSSERVER_H

#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtCore/QXmlStreamReader>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QHostAddress>

 class ClientSocket;

 class AnonymousServer : public QTcpServer
 {
     Q_OBJECT

 public:
     AnonymousServer(int port = 9001, QObject *parent = 0);

 protected:
     void incomingConnection(int socketDescriptor);

 private:
     int peerCount, phase;
     quint16 port;
     QMultiHash<QHostAddress, ClientSocket *> peers;
     QTimer *phaseTimer;
     QXmlStreamReader xml;

     void parseXml();

 public slots:
     void changePhase();
     void encryptionReady();
     void readyRead(QByteArray *message);
     void disconnected();

 signals:
     void connectionChange(const int &peerCount);
     void debugAvailable(const QString &message);
 };

#endif // ANONYMOUSSERVER_H
