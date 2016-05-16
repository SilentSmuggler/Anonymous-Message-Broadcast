#include <QtGui/QInputDialog>
#include <QtGui/QMessageBox>
#include <QtNetwork/QSsl>

#include "anonymousclient.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    settings = new QSettings("CPSC_6136", "AnonymousClient");

    ui->setupUi(this);
    ui->messageEdit->setDisabled(true);

    debugConsole = new DebugDialog(this);

    connect(ui->action_Debug, SIGNAL(toggled(bool)), SLOT(debug(bool)));
    connect(debugConsole, SIGNAL(finished(int)), ui->action_Debug, SLOT(toggle()));

    connectButton = new QAction(QIcon(":/icons/Chat.png"), tr("Connect"), this);
    connectButton->setEnabled(true);
    disconnectButton = new QAction(QIcon(":/icons/Call.png"), tr("Disconnect"), this);
    disconnectButton->setEnabled(false);

    connect(connectButton, SIGNAL(triggered()), SLOT(connectToHost()));
    connect(disconnectButton, SIGNAL(triggered()), SLOT(disconnectFromhost()));

    ui->mainToolBar->addAction(connectButton);
    ui->mainToolBar->addAction(disconnectButton);

    connect(ui->messageEdit, SIGNAL(returnPressed()), SLOT(returnPressed()));
    connect(ui->actionSet_Hostname, SIGNAL(triggered()), SLOT(setHostname()));
    connect(ui->action_Set_Port, SIGNAL(triggered()), SLOT(setPort()));
    connect(ui->action_About, SIGNAL(triggered()), SLOT(about()));
    connect(ui->actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::debug(bool status)
{
    if (status) {
        debugConsole->show();

    } else if (!status) {
        debugConsole->hide();
    }
}

void MainWindow::connectToHost()
{
    client = new AnonymousClient(this);

    connect(client, SIGNAL(sslErrors(QList<QSslError>)), SLOT(sslErrors(QList<QSslError>)));
    connect(client, SIGNAL(messageAvailable(QString)), SLOT(displayMessage(QString)));
    connect(client, SIGNAL(debugAvailable(QString)), debugConsole, SLOT(displayDebug(QString)));
    connect(client, SIGNAL(setPhase0()), SLOT(phase0()));
    connect(client, SIGNAL(setPhase1()), SLOT(phase1()));
    connect(client, SIGNAL(disconnected()), SLOT(disconnectFromhost()));
    connectButton->setEnabled(false);
    disconnectButton->setEnabled(true);

    displayMessage(tr("Connecting..."));
    client->connectToHostEncrypted(settings->value("HostAddress", "localhost").toString(), settings->value("port", 9001).toInt());
}

void MainWindow::disconnectFromhost()
{
    connectButton->setEnabled(true);
    disconnectButton->setEnabled(false);
    
    client->disconnectFromHost();
}

void MainWindow::displayMessage(const QString &message)
{
    QTextCursor cursor(ui->messageDisplay->textCursor());
    cursor.movePosition((QTextCursor::End));
    cursor.insertText(message);
    cursor.insertText("\n");

    ui->messageDisplay->ensureCursorVisible();
}

void MainWindow::returnPressed()
{
    QString text = ui->messageEdit->text();
    if (text.isEmpty())
        return;

    if (client->state() == QSslSocket::ConnectedState)
        client->returnPressed(text);

    ui->messageEdit->clear();
    ui->messageEdit->setDisabled(true);
}

void MainWindow::setHostname()
{
    bool ok;

    QString hostAddress = QInputDialog::getText(this, tr("Set Server Hostname"), tr("Server Hostname:"),
                                                QLineEdit::Normal, settings->value("HostAddress", "localhost").toString(), &ok);

    if (ok && !hostAddress.isEmpty())
        settings->setValue("HostAddress", hostAddress);
}

void MainWindow::setPort()
{
    bool ok;

    int port = QInputDialog::getInt(this, tr("Set Server Port"), tr("Server Port:"),
                                                settings->value("port", 9001).toInt(), 1, 65535, 1, &ok);

    if (ok)
        settings->setValue("port", port);
}

void MainWindow::phase0()
{
    this->setCursor(Qt::ArrowCursor);
    ui->centralWidget->setCursor(Qt::ArrowCursor);
    ui->messageEdit->setEnabled(true);
}

void MainWindow::phase1()
{
    if (ui->messageEdit->isEnabled()) {
        ui->messageEdit->setDisabled(true);
    }

    this->setCursor(Qt::BusyCursor);
    ui->centralWidget->setCursor(Qt::WaitCursor);
}

void MainWindow::about()
{
    QMessageBox::information(this, tr("About"), tr("Created by Coby Pritchard"));
}

void MainWindow::sslErrors(QList<QSslError> error)
{
    for (int i =0; i < error.size(); i++) {
        displayMessage(error.at(i).errorString());
        if (error.at(i).error() == QSslError::SelfSignedCertificate)
            client->ignoreSslErrors();
    }
}
