#include "debugdialog.h"
#include "ui_debugdialog.h"

DebugDialog::DebugDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DebugDialog)
{
    ui->setupUi(this);
}

DebugDialog::~DebugDialog()
{
    delete ui;
}

void DebugDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void DebugDialog::displayDebug(const QString &message)
{
    QTextCursor cursor(ui->debugConsole->textCursor());
    cursor.movePosition((QTextCursor::End));
    cursor.insertText(message);
    cursor.insertText("\n");

    ui->debugConsole->ensureCursorVisible();
}
