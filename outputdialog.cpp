#include "outputdialog.h"
#include "ui_outputdialog.h"

OutputDialog::OutputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OutputDialog)
{
    ui->setupUi(this);
}

void OutputDialog::SetText( QString text )
{
    ui->plainTextEdit->document()->setPlainText(text);
}

OutputDialog::~OutputDialog()
{
    delete ui;
}
