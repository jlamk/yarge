#include "settings.h"

#include <QSettings>
#include <QDebug>
#include <QString>
#include <QStandardPaths>
#include <QFile>
#include <QSettings>
#include <QVariant>
#include <QFont>

void Settings::loadConfig()
{
    const QString config_file = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/config";
    QSettings settings(config_file,QSettings::IniFormat);

    if ( QFile::exists(config_file) )
    {
        QString fontString = settings.value("Editor/Font").toString();
        this->font.fromString(fontString);
        this->geometryData = settings.value("window/Geometry").toByteArray();
    }
    else
    {
        QFont font;
        font.setFamily("DejaVu Sans");
        font.setPointSize(14);
        settings.setValue( "Editor/Font", font.toString());
        this->font = font;

        settings.sync();
    }
}

void Settings::saveConfig()
{
    const QString config_file = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/config";
    QSettings settings(config_file,QSettings::IniFormat);
    settings.setValue("Editor/Font",font.toString() );

    settings.setValue("window/Geometry", this->geometryData);

    settings.sync();
}

