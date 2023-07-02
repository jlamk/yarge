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
        this->lastX = settings.value("window/posX", lastX).toInt();
        this->lastY = settings.value("window/posY", lastY).toInt();
        this->lastWidth = settings.value("window/width", lastWidth).toInt();
        this->lastHeight = settings.value("window/height", lastHeight).toInt();
    }
    else
    {
        QFont font;
        font.setFamily("DejaVu Sans");
        font.setPointSize(14);
        settings.setValue( "Editor/Font", font.toString());
        settings.setValue("window/posX", 0);
        settings.setValue("window/posY", 0);
        settings.setValue("window/width", 800);
        settings.setValue("window/height", 600);

        this->lastX = settings.value("window/posX", lastX).toInt();
        this->lastY = settings.value("window/posY", lastY).toInt();
        this->lastWidth = settings.value("window/width", lastWidth).toInt();
        this->lastHeight = settings.value("window/height", lastHeight).toInt();

        this->font = font;

        settings.sync();
    }
}

void Settings::saveConfig()
{
    const QString config_file = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/config";
    QSettings settings(config_file,QSettings::IniFormat);
    settings.setValue("Editor/Font",font.toString() );
    settings.setValue("window/posX", lastX);
    settings.setValue("window/posY", lastY);
    settings.setValue("window/width", lastWidth);
    settings.setValue("window/height", lastHeight);
    settings.sync();
}

