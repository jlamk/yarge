#ifndef TAB_H
#define TAB_H

#include "codeeditor.h"
#include "tab.h"
#include "utils.h"
#include "highlighter.h"
#include "tab.h"
#include "settings.h"

#include <QString>
#include <QTabWidget>
#include <QVector>
#include <QWidget>
#include <QHBoxLayout>

class TAB : public QTabWidget
{
    Q_OBJECT
public:
    QHBoxLayout *tlayout;
    CodeEditor *editor;
    Highlighter *highlighter;
    bool isModified;
};

#endif // TAB_H
