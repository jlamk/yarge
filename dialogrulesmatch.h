#ifndef DIALOGRULESMATCH_H
#define DIALOGRULESMATCH_H

#include "yarge_yara.h"
#include <QDialog>
#include <QVector>

namespace Ui {
class DialogRulesMatch;
}

class DialogRulesMatch : public QDialog
{
    Q_OBJECT

public:
    explicit DialogRulesMatch(QWidget *parent = nullptr);
    ~DialogRulesMatch();
    void load();
    void SetData( DATA_PTR* d ){ this->data_ptr = d; }

private:
    Ui::DialogRulesMatch *ui;
    DATA_PTR *data_ptr;
};

#endif // DIALOGRULESMATCH_H
