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
 * The AnonymousClient class represents a tcp connection
 */

#include "anonymousclient.h"

static const int SlotSizeBytes = 64;

AnonymousClient::AnonymousClient(QObject *parent)
        : QSslSocket(parent), peerCount(0), keySize(0)
{
    connect(this, SIGNAL(connected()), SLOT(connectedToHost()));
    connect(this, SIGNAL(readyRead()), SLOT(readyRead()));
    connect(this, SIGNAL(disconnected()), SLOT(disconnectedFromHost()));
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(error(QAbstractSocket::SocketError)));
}

void AnonymousClient::connectedToHost()
{
    emit messageAvailable(tr("Connected to Host"));
}

void AnonymousClient::disconnectedFromHost()
{
    emit messageAvailable(tr("Disconnected from Host"));

    deleteLater();
}

void AnonymousClient::readyRead()
{
    QByteArray receivedMessage;
    receivedMessage = readAll();

    emit debugAvailable("XML Received: " + receivedMessage);

    int messageLength = 0;
    do {
        messageLength = receivedMessage.indexOf("</anonymous>");
        xml.addData(receivedMessage.left(messageLength + 13));
        parseXml();

        receivedMessage.remove(0, messageLength + 13);
    } while (!receivedMessage.isEmpty());
}

void AnonymousClient::parseXml()
{
    QString currentTag;

    while (!xml.atEnd()) {
        xml.readNext();

        // Start of an Element
        if (xml.isStartElement()) {
            currentTag = xml.name().toString();

        // End of an Element
        } else if (xml.isEndElement()) {
            currentTag.clear();

        // Characters
        } else if (xml.isCharacters() && !xml.isWhitespace()) {
            if (currentTag == "phase") {
                if (xml.text() == "0") {
                    phase0();

                } else if (xml.text() == "1") {
                    phase1();

                } else if (xml.text() == "2") {
                    phase2();
                }

            } else if (currentTag == "peers") {
                peerCount = xml.text().toString().toInt();
                keySize = SlotSizeBytes * peerCount;

            } else if (currentTag == "slot") {
                slot = xml.text().toString().toInt();

            } else if (currentTag == "random") {
                QByteArray receivedXML(QCA::hexToArray(xml.text().toString().toUtf8()));

                if (sumMessageReceived.isEmpty()) {
                    sumMessageReceived = receivedXML;

                }else {
                    for (int i = 0; i < keySize; ++i)
                        sumMessageReceived[i] = sumMessageReceived[i] + receivedXML[i];
                }

            } else if (currentTag == "sum") {              
                QByteArray receivedXML(QCA::hexToArray(xml.text().toString().toUtf8()));

                if (sumOutputReceived.isEmpty()) {
                    sumOutputReceived = receivedXML;

                }else {
                    for (int i = 0; i < keySize; ++i)
                        sumOutputReceived[i] = sumOutputReceived[i] + receivedXML[i] - 32;
                }
            }
        }
    }

    if (xml.error() && xml.error() != QXmlStreamReader::PrematureEndOfDocumentError) {
        QString errorMessage = "XML ERROR:" + xml.errorString();
        emit messageAvailable(QString(errorMessage));
    }

    xml.clear();
}

// Phase 0 is accept user input
void AnonymousClient::phase0()
{
    emit debugAvailable("Phase 0: User Input Enabled.");

    if (!messageSend.isEmpty()) {
        messageSend.clear();
    }

    // Display Previous Round results
    if (!sumOutputReceived.isEmpty()) {

        for (int i = 0; i < peerCount; ++i) {
            QString messageToDisplay = sumOutputReceived.left(SlotSizeBytes);

            if (!messageToDisplay.isEmpty() && messageToDisplay.at(0) != ' ')
                emit messageAvailable(messageToDisplay);

            sumOutputReceived.remove(0, SlotSizeBytes);
        }

        sumOutputReceived.clear();
    }

    emit setPhase0();
}

// Phase 1 is to generate random strings for each peer
void AnonymousClient::phase1()
{
    emit debugAvailable("Phase 1: User Input Disabled till next round.");

    if (sumMessageReceived.isEmpty()) {
        if (peerCount > 0) {
            int slotPosition = SlotSizeBytes * slot;

            QXmlStreamWriter stream(this);
            stream.writeStartDocument();
            stream.writeStartElement("anonymous");

            QCA::Initializer init;

            QByteArray messageArray(keySize, ' ');
            if (!messageSend.isEmpty())
                messageArray.replace(slotPosition,messageSend.length(), messageSend);

            emit debugAvailable("Plain Text Used: " + messageArray.trimmed());

            QCA::SecureArray plainText(messageArray), randomMessage(keySize), sumRandomMessages(keySize), cipherText(keySize);;

            for (int h = 0; h < (peerCount - 1); ++h) {
                randomMessage = QCA::Random::randomArray(keySize);
                stream.writeTextElement("random", QCA::arrayToHex(randomMessage.toByteArray()));

                emit debugAvailable("Random Message Generated: " + QCA::arrayToHex(randomMessage.toByteArray().trimmed()));

                for (int i = 0; i < keySize; ++i)
                    sumRandomMessages[i] += randomMessage[i];
            }

            for (int i = 0; i < keySize; ++i)
                    cipherText[i] =  plainText[i] - sumRandomMessages[i];

            emit debugAvailable("CipherText : " + QCA::arrayToHex(cipherText.toByteArray().trimmed()));

            stream.writeTextElement("random", QCA::arrayToHex(cipherText.toByteArray()));
            stream.writeEndElement(); // anonymous
            stream.writeEndDocument();
        }
    }

    emit setPhase1();
}

// Phase 2 is to Sum all messages to produce output message
void AnonymousClient::phase2()
{
    emit debugAvailable("Phase 2: Calculating Messages for Current Round.");

    if (!sumMessageReceived.isEmpty()) {

        QXmlStreamWriter stream(this);
        stream.writeStartDocument();
        stream.writeStartElement("anonymous");
        stream.writeTextElement("sum", QCA::arrayToHex(sumMessageReceived));
        stream.writeEndElement(); // anonymous
        stream.writeEndDocument();

        sumMessageReceived.clear();
    }

    emit setPhase2();
}

void AnonymousClient::returnPressed(QString message)
{
    messageSend = message.toUtf8();

    emit debugAvailable("User Input: " + message);
}

void AnonymousClient::error(QAbstractSocket::SocketError)
{
    emit messageAvailable(tr("Error: ") + this->errorString());
}
