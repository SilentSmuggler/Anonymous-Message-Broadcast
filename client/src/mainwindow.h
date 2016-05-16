#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtCore/QSettings>
#include <QtNetwork/QSslError>

#include "debugdialog.h"

class AnonymousClient;

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    DebugDialog *debugConsole;
    QSettings *settings;
    QAction *connectButton;
    QAction *disconnectButton;
    Ui::MainWindow *ui;
    AnonymousClient *client;

private slots:
    void debug(bool);
    void connectToHost();
    void disconnectFromhost();
    void displayMessage(const QString &);
    void returnPressed();
    void setHostname();
    void setPort();
    void phase0();
    void phase1();
    void about();
    void sslErrors(QList<QSslError>);
};

#endif // MAINWINDOW_H
