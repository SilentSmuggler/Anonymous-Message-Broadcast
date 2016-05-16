#include <QtGui/QInputDialog>
#include <QtGui/QMessageBox>

#include "anonymousserver.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    settings = new QSettings("CPSC_6136", "AnonymousServer");

    ui->setupUi(this);
    debugConsole = new DebugDialog(this);

    connect(ui->action_Debug, SIGNAL(toggled(bool)), SLOT(debug(bool)));
    connect(debugConsole, SIGNAL(finished(int)), ui->action_Debug, SLOT(toggle()));

    connect(ui->actionSet_Port, SIGNAL(triggered()), SLOT(setPort()));
    connect(ui->action_About, SIGNAL(triggered()), SLOT(about()));
    connect(ui->actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    server = new AnonymousServer(settings->value("port", 9001).toInt(), this);

    connect(server, SIGNAL(connectionChange(int)), SLOT(updateCounter(int)));
    connect(server, SIGNAL(debugAvailable(QString)), debugConsole, SLOT(displayDebug(QString)));
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

void MainWindow::updateCounter(const int &peerCount)
{
    ui->lcdNumber->display(peerCount);
}

void MainWindow::setPort()
{
    bool ok;

    int port = QInputDialog::getInt(this, tr("Set Listeing Port"), tr("Port:"),
                                                settings->value("port", 9001).toInt(), 1, 65535, 1, &ok);

    if (ok) {
        int reply = QMessageBox::warning(this, tr("Restart Required"),
                                         tr("The listening port has been modified.\n"
                                            "This requires a restart of the Server.\n"
                                            "Do you want to save the changes?"),
                                            QMessageBox::Save, QMessageBox::Cancel);

        if (reply == QMessageBox::Save)
            settings->setValue("port", port);
    }
}

void MainWindow::about()
{
	QMessageBox::information(this, tr("About"), tr("Created by Coby Pritchard"));
}
