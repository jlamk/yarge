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
#include <QHBoxLayout>
#include <QStandardPaths>

void MainWindow::currentChanged(int index)
{
    if (index == -1) {
        return;
    }
    currentIndex = index;
}

void MainWindow::tabCloseRequested( int index )
{
    if (index == -1) {
        return;
    }
    TAB* tab = qobject_cast<TAB*>(tabwidget->widget(index));
    if ( tab != NULL )
    {
        if ( tab->highlighter != NULL )
        {
            delete tab->highlighter;
            tab->highlighter = NULL;
        }
        if ( tab->editor != NULL )
        {
            delete tab->editor;
            tab->editor = NULL;
        }
        if ( tab->tlayout != NULL )
        {
            delete tab->tlayout;
            tab->tlayout = NULL;
        }

        this->tabwidget->removeTab(index);
        if ( tab != NULL )
        {
            delete tab;
        }
    }
}

void MainWindow::NewPage( QString filename )
{
    TAB* tab = new TAB();
    tab->tlayout = new QHBoxLayout();
    tab->setLayout(tab->tlayout);

    tab->editor = new CodeEditor();
    tab->highlighter = new Highlighter(tab->editor->document());
    tab->editor->setFont(settings->font);
    tab->layout()->addWidget(tab->editor);

    QFileInfo info(filename);
    tab->tlayout->addWidget(tab->editor);

    int index = tabwidget->addTab(tab,info.baseName());

    if ( !fileExists(filename) && this->settings->ShowDefaultTemplate )
    {
        const QString template_file = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/default_template.txt";
        tab->editor->document()->setPlainText( LoadFromFile(template_file) );
        this->tabwidget->setTabText(index,"default_template.txt");
        this->tabwidget->setTabToolTip(index,template_file);
    }
    else
    {
        tab->editor->document()->setPlainText( LoadFromFile(filename) );
        QFileInfo info(filename);
        this->tabwidget->setTabText(index,info.baseName());
        this->tabwidget->setTabToolTip(index,filename);
    }

    this->tabwidget->setCurrentIndex(index);
}

MainWindow::MainWindow(QStringList arguments, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Yarge Editor " + QString::fromStdString(APP_VERSION));

    //Load settings
    this->settings = new Settings();
    this->settings->loadConfig();

    restoreGeometry(settings->geometryData);

    // Initialize YARGE_YARA
    this->yarge_yara = new YARGE_YARA();

    // Create and configure tab widget
    tabwidget = new QTabWidget();
    tabwidget->setTabsClosable(true);
    connect(tabwidget, SIGNAL(tabCloseRequested(int)), this, SLOT(tabCloseRequested(int)));
    connect(tabwidget, SIGNAL(currentChanged(int)), this, SLOT(currentChanged(int)));
    tabwidget->setStyleSheet("CodeEditor{ background-color:#fdf6e3;}");
    ui->centralwidget->layout()->setContentsMargins(0, 0, 0, 0);

    // Create main layout
    main_layout = new QSplitter(Qt::Vertical);
    main_layout->addWidget(tabwidget);

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

    //Save Default Template
    const QString template_file = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/default_template.txt";
    if ( !fileExists(template_file) )
    {
        SaveToFile(LoadFromFile(":/rc/templates/default.txt"),template_file);
    }

    // Open file from command line argument
    if (arguments.count() > 1 )
    {
        QString filePath = arguments[1];
        NewPage(filePath);
    }
    else
        on_actionNew_triggered();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    settings->geometryData = saveGeometry();
    settings->saveConfig();

    event->accept();
}

MainWindow::~MainWindow()
{
    if ( this->output_dialog != NULL )
        delete this->output_dialog;

    if ( this->find != NULL )
        delete this->find;

    if ( this->yarge_yara != NULL )
        delete this->yarge_yara;

    if ( this->settings )
        delete this->settings;

    if ( this->main_layout != NULL )
        delete this->main_layout;


    delete ui;
}

void MainWindow::on_actionNew_triggered()
{
    static int index;
    index++;
    QString title = "New File_"+QString::number(index);
    NewPage(title);
}

void MainWindow::on_actionOpen_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open File");

    if (!filePath.isEmpty()) {
        NewPage(filePath);
    }
}

void MainWindow::on_actionSave_As_triggered()
{
    if ( qobject_cast<TAB*>(tabwidget->widget(currentIndex)) == nullptr ) return;
    TAB *tab = qobject_cast<TAB*>(tabwidget->widget(currentIndex));
    if ( tab->editor == NULL ) return;
    QString filePath = QFileDialog::getSaveFileName(this, "Save File","newrule.yar");

    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << tab->editor->toPlainText();

            file.close();
        }
        QFileInfo finfo(filePath);
        this->tabwidget->setTabText(this->tabwidget->currentIndex(), finfo.baseName() );
        this->tabwidget->setTabToolTip(this->tabwidget->currentIndex(), filePath);
    }
}

void MainWindow::on_actionSave_triggered()
{
    TAB *tab = qobject_cast<TAB*>(tabwidget->widget(currentIndex));
    if ( tab == nullptr ) return;

    if ( tab->editor == NULL ) return;

    QString filePath;

    if ( !fileExists(this->tabwidget->tabToolTip(currentIndex) ) )
    {
        filePath = QFileDialog::getSaveFileName(this, "Save File","newrule.yar");
        if (!filePath.isEmpty()) {
            QFile file(filePath);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                out << tab->editor->toPlainText();
                file.close();
            }
            QFileInfo finfo(filePath);

            this->tabwidget->setTabText(this->tabwidget->currentIndex(), finfo.baseName() );
            this->tabwidget->setTabToolTip(this->tabwidget->currentIndex(), filePath);

            ui->statusBar->showMessage("Saved "+this->tabwidget->tabToolTip(currentIndex));
        }

    }
    else
    {
        QFile file(this->tabwidget->tabToolTip(currentIndex));
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << tab->editor->toPlainText();
            file.close();
        }

        ui->statusBar->showMessage("Saved "+this->tabwidget->tabToolTip(currentIndex));
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
    if ( qobject_cast<TAB*>(tabwidget->widget(currentIndex)) == NULL ) return;
    TAB *tab = qobject_cast<TAB*>(tabwidget->widget(currentIndex));

    QTextDocument::FindFlags flag;
    if (reverse) flag |= QTextDocument::FindBackward;
    if (casesens) flag |= QTextDocument::FindCaseSensitively;
    if (words) flag |= QTextDocument::FindWholeWords;

    QTextCursor cursor = tab->editor->textCursor();
    // here , you save the cursor position
    QTextCursor cursorSaved = cursor;

    if (!tab->editor->find(s, flag))
    {
        //nothing is found | jump to start/end
        cursor.movePosition(reverse?QTextCursor::End:QTextCursor::Start);

        /* following line :
            - the cursor is set at the beginning/end of the document (if search is reverse or not)
            - in the next "find", if the word is found, now you will change the cursor position
            */
        tab->editor->setTextCursor(cursor);

        if (!tab->editor->find(s, flag))
        {
            //no match in whole document
            QMessageBox msgBox;
            msgBox.setText(tr("Not found."));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.exec();

            // word not found : we set the cursor back to its initial position
            tab->editor->setTextCursor(cursorSaved);
        }
    }
}

void MainWindow::on_actionCompile_current_rule_triggered()
{
    TAB *tab = qobject_cast<TAB*>(tabwidget->widget(currentIndex));
    if ( tab == NULL ) return;
    if ( tab->editor == NULL ) return;
    if ( tab->editor->toPlainText().isEmpty() )
    {
        this->output_dialog ->setVisible(false);
        return;
    }

    this->yarge_yara->Compile( tab->editor->toPlainText());
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
    TAB *tab = qobject_cast<TAB*>(tabwidget->widget(currentIndex));
    if ( tab == NULL ) return;
    if ( tab->editor != NULL )
        findString(str,false,casesensitive,false);
}

void MainWindow::on_actionLocate_triggered()
{
    static bool dialog_visible = false;

    find->setVisible( dialog_visible? false : true );
}

void MainWindow::on_actionScan_file_triggered()
{
    TAB *tab = qobject_cast<TAB*>(tabwidget->widget(currentIndex));
    if ( tab == NULL ) return;

    QString filePath = QFileDialog::getOpenFileName(this, "Open File to Scan");
    if (filePath.isEmpty())
        return;

    MATCH_DATA_LIST* data = new MATCH_DATA_LIST();
    DATA_PTR* data_ptr = new DATA_PTR();
    data_ptr->filename = filePath;
    data_ptr->match_data_list_ptr = data;

    yarge_yara->Scan_File(data_ptr, tab->editor->toPlainText(), filePath);

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
    TAB *tab = qobject_cast<TAB*>(tabwidget->widget(currentIndex));
    if ( tab == NULL ) return;

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
            yarge_yara->Scan_File(data_ptr, tab->editor->toPlainText(), fileInfo.filePath());

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


void MainWindow::on_actionClose_Window_triggered()
{
    emit this->tabwidget->tabCloseRequested(currentIndex);
}

