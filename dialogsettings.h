#ifndef DIALOGSETTINGS_H
#define DIALOGSETTINGS_H

#include "settings.h"
#include "tab.h"

#include <QDialog>
#include <QString>

namespace Ui {
class DialogSettings;
}

class DialogSettings : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSettings(QWidget *parent = nullptr);
    ~DialogSettings();
    void LoadSet(TABWIDGET *pages, Settings *set );

private slots:
    void on_buttonBox_accepted();

private:
    Ui::DialogSettings *ui;
    TABWIDGET *tabs;
    Settings *set;
};

#endif // DIALOGSETTINGS_H
