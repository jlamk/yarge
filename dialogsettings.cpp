#include "dialogsettings.h"
#include "ui_dialogsettings.h"

#include <QDebug>
#include <QStandardPaths>

QString test;

DialogSettings::DialogSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSettings)
{
    ui->setupUi(this);
}

DialogSettings::~DialogSettings()
{
    delete ui;
}

void DialogSettings::LoadSet(TABWIDGET *pages, Settings *set )
{
    ui->fontComboBox->setCurrentText( set->font.family());
    ui->spinBox->setValue( set->font.pointSize() );
    this->tabs = pages;
    this->set = set;
}

void DialogSettings::on_buttonBox_accepted()
{
    QFont font;
    font.setFamily( ui->fontComboBox->currentText() );
    font.setPointSize( ui->spinBox->value());
    this->set->font = font;
    const QString config_file = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/config";
    QSettings settings(config_file,QSettings::IniFormat);
    settings.setValue("Editor/Font", font.toString() );
    settings.sync();

    foreach( TAB *tab, this->tabs->pages )
    {
        tab->editor->setFont( font );
    }


    close();
}

