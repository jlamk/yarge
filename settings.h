#ifndef H_SETTINGS
#define H_SETTINGS

#include <QString>
#include <QSettings>
#include <QFont>

class Settings
{
public:
    QFont font;
    int lastX;
    int lastY;
    int lastWidth;
    int lastHeight;

    void loadConfig();
    void saveConfig();

private:

};

#endif
