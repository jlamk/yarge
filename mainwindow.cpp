#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "codeeditor.h"
#include "hash.h"
#include "tab.h"
#include "outputdialog.h"
#include "about.h"
#include "searchdialog.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QSaveFile>
#include <QSplitter>
#include <QDebug>
#include <QDialog>
#include <QInputDialog>
#include <QMessageBox>

TAB *CurrentTab;

void MainWindow::currentChanged(int index)
{
    if (index == -1) {
         return;
     }

    QString title = this->tabwidget->tabText(index);
    QString filename = this->tabwidget->tabToolTip(index);

    foreach( TAB *tab, this->tabwidget->pages )
    {
        if ( tab->title == title )
        {
            CurrentTab = tab;
            break;
        }
    }
}

void MainWindow::tabCloseRequested( int index )
{
   if (index == -1) {
        return;
    }
    QString title = this->tabwidget->tabText(index);
    DeletePage(title);
}

void MainWindow::DeletePage( const QString &title )
{
    int index = 0;
    foreach (TAB *tab, this->tabwidget->pages) {
        if ( tab == NULL ) continue;
        if ( tab->title == title )
        {
            tab->editor->deleteLater();
            this->tabwidget->pages.remove(index);
            break;
        }
        index++;
    }
}

void MainWindow::NewPage( TAB *tab )
{
    int index = this->tabwidget->ya_addTab(tab);
    CurrentTab = tab;
    this->tabwidget->setTabToolTip(index,tab->filename);
    this->tabwidget->setTabText( index, tab->title );
    this->tabwidget->setCurrentIndex(index);
}

MainWindow::MainWindow(QStringList arguments, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Yarge Editor "+QString::fromStdString(APP_VERSION));
    this->yarge_yara = new YARGE_YARA();

    tabwidget = new TABWIDGET();
    tabwidget->setTabsClosable(true);
    connect( tabwidget,SIGNAL( tabCloseRequested(int) ), this, SLOT( tabCloseRequested(int) )  );
    connect( tabwidget,SIGNAL( currentChanged(int) ), this, SLOT( currentChanged(int) )  );

    this->tabwidget->setStyleSheet("color:black; background-color:#fdf6e3;");
    ui->centralwidget->layout()->setContentsMargins(0,0,0,0);

    main_layout = new QSplitter( Qt::Vertical );

    main_layout->addWidget(tabwidget);

    if ( arguments.count() == 2 )
    {
        QString filePath = arguments[1];
        if (!filePath.isEmpty()) {
            QFile file(filePath);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                QString fileContent = in.readAll();

                QFileInfo info(filePath);
                TAB* tab = new TAB();
                tab->title = info.baseName();
                tab->filename= filePath;
                NewPage(tab);
                CurrentTab->editor->document()->setPlainText(fileContent);
                file.close();
            }
        }
    }
    else
    {
        TAB *tab = new TAB();
        tab->title = "New File";
        NewPage(tab);
        tab->editor->document()->setPlainText("import \"pe\"\n\nrule RuleName {\n\nmeta:\n      author = \"name\"\n      my_identifier_1 = \"identifier\"\n\nstrings:\n      $a = \"malware\"\n\ncondition:\n       any of them \n\n}");
    }


    this->output_dialog = new OutputDialog();
    main_layout->addWidget(this->output_dialog);
    this->output_dialog ->setVisible(false);

    find = new SearchDialog();
    main_layout->addWidget(find);
    find->setVisible(false);
    connect( find,SIGNAL( Find( QString,bool ) ),this, SLOT( FindText( QString,bool  ) )  );

    setCentralWidget(main_layout);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionNew_triggered()
{
    static int index;
    index++;
    TAB *tab = new TAB();
    tab->title = "New File_"+QString::number(index);
    NewPage(tab);
}

void MainWindow::on_actionOpen_triggered()
{
    static QString lastpath;
    QString filePath = QFileDialog::getOpenFileName(this, "Open File");

    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString fileContent = in.readAll();

            QFileInfo info(filePath);
            TAB* tab = new TAB();
            tab->title = info.baseName();
            tab->filename= filePath;
            NewPage(tab);
            CurrentTab->editor->document()->setPlainText(fileContent);
            file.close();
        }
    }
}

void MainWindow::on_actionSave_As_triggered()
{
    if ( CurrentTab->editor == NULL ) return;
    QString filePath = QFileDialog::getSaveFileName(this, "Save File");
    CurrentTab->filename = filePath;
    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << CurrentTab->editor->toPlainText();
            file.close();
        }
    }
}

void MainWindow::on_actionSave_triggered()
{
    if ( CurrentTab->editor == NULL ) return;

    QString filePath;

    if ( CurrentTab->filename == "" )
    {
        filePath = QFileDialog::getSaveFileName(this, "Save File");
        if (!filePath.isEmpty()) {
            QFile file(filePath);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                out << CurrentTab->editor->toPlainText();
                file.close();
            }
            CurrentTab->filename  = filePath;
    }

    }
    else
    {
        QFile file(CurrentTab->filename);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << CurrentTab->editor->toPlainText();
            file.close();
        }
    }
    ui->statusBar->showMessage("Saved at "+CurrentTab->filename);
}

void MainWindow::on_actionExit_triggered()
{
    qApp->exit(0);
}

void MainWindow::on_actionFile_Md5_hash_triggered()
{
     QString filePath = QFileDialog::getOpenFileName(this, "Open File");
     if ( filePath.isEmpty() ) return;
     QString hash = calculateFileHash(filePath);
     QInputDialog dialog;
     dialog.setLabelText(filePath);
     dialog.setTextValue(hash);
     dialog.exec();
}

void MainWindow::findString(QString s, bool reverse, bool casesens, bool words)
{
    QTextDocument::FindFlags flag;
        if (reverse) flag |= QTextDocument::FindBackward;
        if (casesens) flag |= QTextDocument::FindCaseSensitively;
        if (words) flag |= QTextDocument::FindWholeWords;

        QTextCursor cursor = CurrentTab->editor->textCursor();
        // here , you save the cursor position
        QTextCursor cursorSaved = cursor;

        if (!CurrentTab->editor->find(s, flag))
        {
            //nothing is found | jump to start/end
            cursor.movePosition(reverse?QTextCursor::End:QTextCursor::Start);

            /* following line :
            - the cursor is set at the beginning/end of the document (if search is reverse or not)
            - in the next "find", if the word is found, now you will change the cursor position
            */
            CurrentTab->editor->setTextCursor(cursor);

            if (!CurrentTab->editor->find(s, flag))
            {
                //no match in whole document
                QMessageBox msgBox;
                msgBox.setText(tr("String not found."));
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.setDefaultButton(QMessageBox::Ok);
                msgBox.exec();

                // word not found : we set the cursor back to its initial position
                CurrentTab->editor->setTextCursor(cursorSaved);
            }
        }
}

void MainWindow::on_actionCompile_current_rule_triggered()
{
    if ( CurrentTab->editor->toPlainText().isEmpty() )
    {
        this->output_dialog ->setVisible(false);
        return;
    }

    this->yarge_yara->Compile( CurrentTab->editor->toPlainText() );
    if ( yarge_yara->output == "" )
    {
        ui->statusBar->showMessage( "Rules compiled successfully!" );
        this->output_dialog ->setVisible(false);
        return;
    }
    this->output_dialog ->setVisible(true);
    this->output_dialog->SetText(this->yarge_yara->output );
    this->yarge_yara->output = "";
}


void MainWindow::on_action_triggered()
{
    About about;
    about.exec();
}

void MainWindow::FindText( QString str, bool casesensitive )
{
    findString(str,false,casesensitive,false);
}

void MainWindow::on_actionLocate_triggered()
{
    static bool dialog_visible = false;

    find->setVisible( dialog_visible? false : true );
}

