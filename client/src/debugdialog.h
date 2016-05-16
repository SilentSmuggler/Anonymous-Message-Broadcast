#ifndef DEBUGDIALOG_H
#define DEBUGDIALOG_H

#include <QtGui/QDialog>

namespace Ui {
    class DebugDialog;
}

class DebugDialog : public QDialog {

    Q_OBJECT

public:
    DebugDialog(QWidget *parent = 0);
    ~DebugDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::DebugDialog *ui;

 private slots:
    void displayDebug(const QString &);
};

#endif // DEBUGDIALOG_H
