#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtCore/QSettings>

#include "debugdialog.h"

class AnonymousServer;

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
    Ui::MainWindow *ui;
    AnonymousServer *server;

private slots:
    void debug(bool);
    void updateCounter(const int &peerCount);
    void setPort();
    void about();
};

#endif // MAINWINDOW_H
