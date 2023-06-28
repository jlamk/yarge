#ifndef TAB_H
#define TAB_H

#include "codeeditor.h"
#include "tab.h"
#include "utils.h"

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
};

class TABWIDGET : public QTabWidget {

public:
    QVector<TAB*> pages;

    int ya_addTab( TAB *tab )
    {
        static int nextTabId = 0;
        nextTabId++;
        tab->filename = generateRandomString(40);
        tab->id = nextTabId;
        tab->editor = new CodeEditor();
        pages.append(tab);

        return QTabWidget::addTab(tab->editor, tab->title);
    }

private:

};

#endif // TAB_H
