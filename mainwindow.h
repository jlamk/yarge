#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "codeeditor.h"
#include "tab.h"
#include "yarge_yara.h"
#include "outputdialog.h"
#include "searchdialog.h"

#include <QMainWindow>
#include <QVector>
#include <QString>
#include <QSplitter>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QStringList arguments,QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void FindText( QString str, bool casesensitive );

private slots:
    void on_actionNew_triggered();
    void tabCloseRequested( int index );
    void currentChanged(int index);

    void on_actionOpen_triggered();

    void on_actionSave_As_triggered();

    void on_actionSave_triggered();

    void on_actionExit_triggered();

    void on_actionFile_Md5_hash_triggered();

    void on_actionCompile_current_rule_triggered();

    void on_action_triggered();

    void on_actionLocate_triggered();

private:
    Ui::MainWindow *ui;
    TABWIDGET *tabwidget;
    YARGE_YARA *yarge_yara;
    OutputDialog *output_dialog;
    QSplitter *main_layout;
    SearchDialog *find;

    void NewPage( TAB *tab );
    void DeletePage( const QString &title );
    void findString(QString s, bool reverse, bool casesens, bool words);
};
#endif // MAINWINDOW_H
