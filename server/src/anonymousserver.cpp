/**
 * @file
 * @author  Coby Pritchard <pritchard_coby@colstate.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * The AnonymousServer class represents a tcp server
 */

#include <QtNetwork/QSsl>
#include <QtXml/QXmlStreamWriter>

#include "clientsocket.h"
#include "anonymousserver.h"

static const int PhaseChange = 20 * 1000;

AnonymousServer::AnonymousServer(int port, QObject *parent)
    : QTcpServer(parent), phase(0)
{
    listen(QHostAddress::Any, port);

    phaseTimer = new QTimer(this);

    connect(phaseTimer, SIGNAL(timeout()), SLOT(changePhase()));
    phaseTimer->start(PhaseChange);
}

void AnonymousServer::incomingConnection(int socketDescriptor)
{
    ClientSocket *clientSocket = new ClientSocket(this);
    if (clientSocket->setSocketDescriptor(socketDescriptor)) {

        peers.insert(clientSocket->peerAddress(), clientSocket);
        peerCount = peers.count();
        clientSocket->setSlot(peerCount - 1);

        connect(clientSocket, SIGNAL(encrypted()), SLOT(encryptionReady()));
        connect(clientSocket, SIGNAL(messageReceived(QByteArray*)), SLOT(readyRead(QByteArray*)));
        connect(clientSocket, SIGNAL(disconnected()), SLOT(disconnected()));

        clientSocket->setLocalCertificate(tr("./certs/server.crt"));
        clientSocket->setPrivateKey(tr("./certs/server.key"));
        clientSocket->startServerEncryption();

        emit connectionChange(peerCount);

    } else {
        delete clientSocket;
    }
}

void AnonymousServer::encryptionReady()
{
    QList<ClientSocket *> connections = peers.values();
    foreach (ClientSocket *clientSocket, connections) {
        QXmlStreamWriter stream(clientSocket);
        stream.writeStartDocument();
        stream.writeStartElement("anonymous");
        stream.writeTextElement("phase", QString::number(phase));
        stream.writeTextElement("peers", QString::number(peerCount));
        stream.writeTextElement("slot", QString::number(clientSocket->getSlot()));
        stream.writeEndElement(); // anonymous
        stream.writeEndDocument();
    }
}

void AnonymousServer::changePhase()
{
    if (phase < 2) {
        phase++;

    } else if (phase == 2){
        phase = 0;
    }

    QList<ClientSocket *> connections = peers.values();
    foreach (ClientSocket *clientSocket, connections) {
        QXmlStreamWriter stream(clientSocket);
        stream.writeStartDocument();
        stream.writeStartElement("anonymous");
        stream.writeTextElement("phase", QString::number(phase));
        stream.writeTextElement("peers", QString::number(peerCount));
        stream.writeTextElement("slot", QString::number(clientSocket->getSlot()));
        stream.writeEndElement(); // anonymous
        stream.writeEndDocument();
     }
}

void AnonymousServer::readyRead(QByteArray *message)
{
    QByteArray receivedMessage;
    receivedMessage = *message;

    emit debugAvailable("XML Received: " + receivedMessage);

    int messageLength = 0;
    do {
        messageLength = receivedMessage.indexOf("</anonymous>");
        xml.addData(receivedMessage.left(messageLength + 13));
        parseXml();

        receivedMessage.remove(0, messageLength + 13);
    } while (!receivedMessage.isEmpty());
}

void AnonymousServer::parseXml()
{
    int i = 0;
    ClientSocket *clientSocket;
    QString currentTag, randomMessage, sumMessage;
    QList<ClientSocket *> connections = peers.values();

    while (!xml.atEnd()) {
        xml.readNext();

        // Start of an Element
        if (xml.isStartElement()) {
            if (xml.name() == "random") {           
                clientSocket = connections.value(i);
            }
            currentTag = xml.name().toString();

        // End of an Element
        } else if (xml.isEndElement()) {
            if (xml.name() == "random") {
                randomMessage.clear();
                i++;
            }
            currentTag.clear();

        // Characters
        } else if (xml.isCharacters() && !xml.isWhitespace()) {
            if (currentTag == "random") {
                randomMessage = xml.text().toString();
                QXmlStreamWriter stream(clientSocket);
                stream.writeStartDocument();
                stream.writeStartElement("anonymous");
                stream.writeTextElement("random", randomMessage.toAscii());
                stream.writeEndElement(); // anonymous
                stream.writeEndDocument();

            } else if (currentTag == "sum") {
                foreach (ClientSocket *clientSocket, connections) {
                    sumMessage = xml.text().toString();
                    QXmlStreamWriter stream(clientSocket);
                    stream.writeStartDocument();
                    stream.writeStartElement("anonymous");
                    stream.writeTextElement("sum", sumMessage.toAscii());
                    stream.writeEndElement(); // anonymous
                    stream.writeEndDocument();
                }
            }
        }
    }

    if (xml.error() && xml.error() != QXmlStreamReader::PrematureEndOfDocumentError) {
        QString errorMessage = "XML ERROR:" + xml.errorString();
        emit debugAvailable(QString(errorMessage));
    }

    xml.clear();
}

void AnonymousServer::disconnected()
{
    if (ClientSocket *clientSocket = qobject_cast<ClientSocket *>(sender())) {
        if (peers.contains(clientSocket->peerAddress())) {
            peers.remove(clientSocket->peerAddress());
            peerCount = peers.count();
            clientSocket->deleteLater();

            emit connectionChange(peerCount);
        }
    }
}
