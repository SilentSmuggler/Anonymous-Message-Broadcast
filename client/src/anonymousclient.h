#ifndef ANONYMOUSCLIENT_H_
#define ANONYMOUSCLIENT_H_

#include <QtCore/QXmlStreamReader>
#include <QtNetwork/QSslSocket>
#include <QtNetwork/QHostAddress>
#include <QtCrypto>

class AnonymousClient : public QSslSocket
{
    Q_OBJECT

public:
        AnonymousClient(QObject *parent = 0);
        void returnPressed(QString message);

private:
        void parseXml();
        void phase0();
        void phase1();
        void phase2();

        int peerCount, keySize, slot;
        QXmlStreamReader xml;
        QHostAddress serverAddress;

protected:
        QByteArray messageSend, sumMessageReceived, sumOutputReceived;

private slots:
        void connectedToHost();
        void disconnectedFromHost();
        void readyRead();
        void error(QAbstractSocket::SocketError err);

signals:
        void messageAvailable(const QString &message);
        void debugAvailable(const QString &message);
        void setPhase0();
        void setPhase1();
        void setPhase2();
};

#endif /* ANONYMOUSCLIENT_H_ */
