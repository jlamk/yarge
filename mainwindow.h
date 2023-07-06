#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "codeeditor.h"
#include "tab.h"
#include "yarge_yara.h"
#include "outputdialog.h"
#include "searchdialog.h"
#include "settings.h"

#include <QMainWindow>
#include <QVector>
#include <QString>
#include <QSplitter>
#include <QCloseEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QStringList arguments,QWidget *parent = nullptr);
    ~MainWindow();
    YARGE_YARA *yarge_yara;

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

    void on_actionScan_file_triggered();

    void on_actionScan_Directory_triggered();

    void on_actionPreferences_triggered();

protected:
    void closeEvent(QCloseEvent *event);
    void mousePressEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton) {
            // Store the initial mouse position relative to the window
            dragPosition = event->globalPos() - frameGeometry().topLeft();
            event->accept();
        }
    }

    void mouseMoveEvent(QMouseEvent *event) override
    {
        if (event->buttons() & Qt::LeftButton) {
            // Calculate the new position of the window based on the mouse movement
            move(event->globalPos() - dragPosition);
            event->accept();
        }
    }

private:
    Ui::MainWindow *ui;
    TABWIDGET *tabwidget;
    OutputDialog *output_dialog;
    QSplitter *main_layout;
    SearchDialog *find;
    TAB *CurrentTab;
    Settings *settings;
    QPoint dragPosition;

    void NewPage( TAB *tab );
    void DeletePage( const QString &filename );
    void findString(QString s, bool reverse, bool casesens, bool words);
};
#endif // MAINWINDOW_H
