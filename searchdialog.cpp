#include "searchdialog.h"
#include "ui_searchdialog.h"

SearchDialog::SearchDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SearchDialog)
{
    ui->setupUi(this);
}

SearchDialog::~SearchDialog()
{
    delete ui;
}

void SearchDialog::on_pushButton_clicked()
{
    close();
}


void SearchDialog::on_lineEdit_returnPressed()
{
    emit Find( ui->lineEdit->text(), ui->checkBox->checkState() );
}

