#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "codeeditor.h"
#include "hash.h"
#include "tab.h"
#include "outputdialog.h"
#include "about.h"
#include "searchdialog.h"
#include "dialogrulesmatch.h"
#include "utils.h"
#include "dialogsettings.h"
#include "settings.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QSaveFile>
#include <QSplitter>
#include <QDebug>
#include <QDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QDirIterator>

void MainWindow::currentChanged(int index)
{
    if (index == -1) {
        return;
    }

    QString filename = this->tabwidget->tabToolTip(index);

    foreach(TAB* tab, this->tabwidget->pages)
    {
        if (tab->filename == filename)
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

    QString title = this->tabwidget->tabToolTip(index);
    DeletePage(title);
}

void MainWindow::DeletePage(const QString& filename)
{
    int index = 0;
    foreach (TAB* tab, this->tabwidget->pages)
    {
        if (tab == nullptr)
            continue;

        if (tab->filename == filename)
        {
            tab->editor->deleteLater();
            tab->editor = NULL;
            this->tabwidget->pages.remove(index);
            break;
        }

        index++;
    }
}

void MainWindow::NewPage( TAB *tab )
{
    int index = this->tabwidget->ya_addTab(this->settings,tab);
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
    setWindowTitle("Yarge Editor " + QString::fromStdString(APP_VERSION));

    this->settings = new Settings();
    this->settings->loadConfig();

    setGeometry(settings->lastX, settings->lastY, settings->lastWidth, settings->lastHeight);

    // Initialize YARGE_YARA
    this->yarge_yara = new YARGE_YARA();

    // Create and configure tab widget
    tabwidget = new TABWIDGET();
    tabwidget->setTabsClosable(true);
    connect(tabwidget, SIGNAL(tabCloseRequested(int)), this, SLOT(tabCloseRequested(int)));
    connect(tabwidget, SIGNAL(currentChanged(int)), this, SLOT(currentChanged(int)));
    tabwidget->setStyleSheet("CodeEditor{ color:black; background-color:#fdf6e3;}");
    ui->centralwidget->layout()->setContentsMargins(0, 0, 0, 0);

    // Create main layout
    main_layout = new QSplitter(Qt::Vertical);
    main_layout->addWidget(tabwidget);

    // Open file from command line argument
    if (arguments.count() == 2)
    {
        QString filePath = arguments[1];
        if (!filePath.isEmpty())
        {
            QFile file(filePath);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                QTextStream in(&file);
                QString fileContent = in.readAll();

                QFileInfo info(filePath);
                TAB* tab = new TAB();
                tab->title = info.baseName();
                tab->filename = filePath;
                NewPage(tab);
                CurrentTab->editor->document()->setPlainText(fileContent);
                file.close();
            }
        }
    }
    else
    {
        // Create new file tab with default content
        TAB* tab = new TAB();
        tab->title = "New File";
        NewPage(tab);
        tab->editor->document()->setPlainText("import \"pe\"\n\nrule RuleName {\n\nmeta:\n\tauthor = \"JLAMK\"\n\tdesc = \"Yarge Editor\"\n\n\nstrings:\n\t$a = \"Yarge\"\n\t$b = \"Editor\"\n\ncondition:\n\tany of them \n\n}");
    }

    // Create and configure output dialog
    this->output_dialog = new OutputDialog();
    main_layout->addWidget(this->output_dialog);
    this->output_dialog->setVisible(false);

    // Create and configure search dialog
    find = new SearchDialog();
    main_layout->addWidget(find);
    find->setVisible(false);
    connect(find, SIGNAL(Find(QString, bool)), this, SLOT(FindText(QString, bool)));

    // Set the main layout as the central widget
    setCentralWidget(main_layout);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    this->settings->lastX = pos().x();
    this->settings->lastY = pos().y();
    this->settings->lastWidth = size().width();
    this->settings->lastHeight = size().height();
    settings->saveConfig();

    event->accept();
}

MainWindow::~MainWindow()
{
    if ( this->output_dialog )
        delete this->output_dialog;

    if ( this->find )
        delete this->find;

    if ( this->yarge_yara )
        delete this->yarge_yara;

    if ( this->settings )
        delete this->settings;

    if ( this->main_layout )
        delete this->main_layout;

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
            QFileInfo finfo(filePath);
            CurrentTab->title = finfo.baseName();
            CurrentTab->filename = filePath;
            this->tabwidget->setTabText(this->tabwidget->currentIndex(), finfo.baseName() );
            this->tabwidget->setTabToolTip(this->tabwidget->currentIndex(), filePath);
            file.close();
        }
    }
}

void MainWindow::on_actionSave_As_triggered()
{
    if ( CurrentTab->editor == NULL ) return;
    QString filePath = QFileDialog::getSaveFileName(this, "Save File","newrule.yar");
    CurrentTab->filename = filePath;
    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << CurrentTab->editor->toPlainText();

            file.close();
        }
        QFileInfo finfo(filePath);
        CurrentTab->title = finfo.baseName();
        this->tabwidget->setTabText(this->tabwidget->currentIndex(), finfo.baseName() );
        this->tabwidget->setTabToolTip(this->tabwidget->currentIndex(), filePath);
    }
}

void MainWindow::on_actionSave_triggered()
{
    if ( CurrentTab->editor == NULL ) return;

    QString filePath;

    if ( !fileExists(CurrentTab->filename) )
    {
        filePath = QFileDialog::getSaveFileName(this, "Save File","newrule.yar");
        if (!filePath.isEmpty()) {
            QFile file(filePath);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                out << CurrentTab->editor->toPlainText();
                file.close();
            }
            CurrentTab->filename  = filePath;

            QFileInfo finfo(filePath);
            CurrentTab->title = finfo.baseName();
            this->tabwidget->setTabText(this->tabwidget->currentIndex(), finfo.baseName() );
            this->tabwidget->setTabToolTip(this->tabwidget->currentIndex(), filePath);

            ui->statusBar->showMessage("Saved "+CurrentTab->filename);
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

        ui->statusBar->showMessage("Saved "+CurrentTab->filename);
    }

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
            msgBox.setText(tr("Not found."));
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
    if ( CurrentTab->editor == NULL ) return;
    if ( CurrentTab->editor->toPlainText().isEmpty() )
    {
        this->output_dialog ->setVisible(false);
        return;
    }

    this->yarge_yara->Compile( CurrentTab->editor->toPlainText());
    if ( !yarge_yara->output.isEmpty() )
    {
        this->output_dialog ->setVisible(true);
        this->output_dialog->SetText(this->yarge_yara->output );
        this->yarge_yara->output = "";
        return;
    }

    ui->statusBar->showMessage( "Rules compiled successfully!" );
    this->output_dialog ->setVisible(false);

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

void MainWindow::on_actionScan_file_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open File to Scan");
    if (filePath.isEmpty())
        return;

    MATCH_DATA_LIST* data = new MATCH_DATA_LIST();
    DATA_PTR* data_ptr = new DATA_PTR();
    data_ptr->filename = filePath;
    data_ptr->match_data_list_ptr = data;

    yarge_yara->Scan_File(data_ptr, CurrentTab->editor->toPlainText(), filePath);

    if (data_ptr->match_data_list_ptr->matchs_list.count() == 0)
    {
        delete data;
        delete data_ptr;
        QMessageBox::information(this, "Yarge Editor", "No matching signatures found in the selected file.");
        return;
    }

    DialogRulesMatch dialog;
    dialog.SetData(data_ptr);
    dialog.load();
    dialog.exec();

    delete data;
    delete data_ptr;

}

void MainWindow::on_actionScan_Directory_triggered()
{
    QString directoryPath = QFileDialog::getExistingDirectory(
                nullptr,
                "Select Directory to Scan",
                QString(),
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
                );
    if (directoryPath.isEmpty())
        return;

    QDirIterator it(directoryPath, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    ui->statusBar->showMessage("Scan started.");
    DialogRulesMatch dialog;

    while (it.hasNext())
    {
        QString entry = it.next();
        QFileInfo fileInfo(entry);

        if (fileInfo.isFile())
        {
            MATCH_DATA_LIST* data = new MATCH_DATA_LIST();
            DATA_PTR* data_ptr = new DATA_PTR();
            data_ptr->filename = fileInfo.filePath();
            data_ptr->match_data_list_ptr = data;
            yarge_yara->Scan_File(data_ptr, CurrentTab->editor->toPlainText(), fileInfo.filePath());

            if (data_ptr->match_data_list_ptr->matchs_list.count() > 0)
            {
                dialog.SetData(data_ptr);
                dialog.load();
            }

            delete data;
            delete data_ptr;
        }
    }
    ui->statusBar->showMessage("Scan complete!");
    dialog.exec();

}

void MainWindow::on_actionPreferences_triggered()
{
    DialogSettings dialog;
    dialog.LoadSet(this->tabwidget,this->settings);
    dialog.exec();
}

