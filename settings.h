#ifndef H_SETTINGS
#define H_SETTINGS

#include <QString>
#include <QSettings>
#include <QFont>
#include <QByteArray>

class Settings
{
public:
    QFont font;
    QByteArray geometryData;

    void loadConfig();
    void saveConfig();

private:

};

#endif
