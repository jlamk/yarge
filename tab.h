#ifndef TAB_H
#define TAB_H

#include "codeeditor.h"
#include "tab.h"
#include "utils.h"
#include "highlighter.h"

#include <QString>
#include <QTabWidget>
#include <QVector>

class TAB
{
public:
    int id;
    QString filename;
    QString title;
    CodeEditor *editor;
    Highlighter *highlighter;
};

class TABWIDGET : public QTabWidget {

public:
    QVector<TAB*> pages;
    int ya_addTab( TAB *tab );

private:

};

#endif // TAB_H
