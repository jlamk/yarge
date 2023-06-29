#include "tab.h"



int TABWIDGET::ya_addTab( TAB *tab )
{
    static int nextTabId = 0;
    nextTabId++;
    tab->filename = generateRandomString(40);
    tab->id = nextTabId;
    tab->editor = new CodeEditor();
    tab->highlighter = new Highlighter(tab->editor->document());

    this->pages.append(tab);

    return QTabWidget::addTab(tab->editor, tab->title);
}
