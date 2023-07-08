#include "dialogsettings.h"
#include "ui_dialogsettings.h"

#include <QDebug>
#include <QStandardPaths>


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

void DialogSettings::LoadSet(QTabWidget *pages, Settings *set )
{
    ui->fontComboBox->setCurrentText( set->font.family());
    ui->spinBox->setValue( set->font.pointSize() );
    ui->checkBox->setChecked(set->ShowDefaultTemplate);
    this->tabs = pages;
    this->set = set;
}

void DialogSettings::on_buttonBox_accepted()
{
    QFont font;
    font.setFamily( ui->fontComboBox->currentText() );
    font.setPointSize( ui->spinBox->value());
    this->set->font = font;
    this->set->ShowDefaultTemplate = ui->checkBox->checkState();
    const QString config_file = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/config";
    QSettings settings(config_file,QSettings::IniFormat);
    settings.setValue("editor/Font", font.toString() );
    settings.setValue("general/ShowDefaultTemplate",ui->checkBox->checkState());
    settings.sync();

    for( int i = 0; i < tabs->count(); i++ )
    {
        TAB *tab = qobject_cast<TAB*>(tabs->widget(i));
        tab->editor->setFont( font );
    }

    close();
}

