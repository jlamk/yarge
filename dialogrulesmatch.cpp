#include "dialogrulesmatch.h"
#include "ui_dialogrulesmatch.h"

#include <QDebug>

DialogRulesMatch::DialogRulesMatch(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogRulesMatch)
{
    ui->setupUi(this);
}

DialogRulesMatch::~DialogRulesMatch()
{
    delete ui;
}

void DialogRulesMatch::load()
{
    QTreeWidgetItem* item = new QTreeWidgetItem();

    QIcon icon(":/images/img02.png");
    item->setIcon(0, icon);

    QString title = this->data_ptr->filename;
    item->setText(0, title);
    ui->treeWidget->addTopLevelItem(item);

    foreach (MATCH_DATA data, this->data_ptr->match_data_list_ptr->matchs_list)
    {
        QTreeWidgetItem* rule_matched = new QTreeWidgetItem();
        rule_matched->setText(0, data.ruleName);
        item->addChild(rule_matched);

        foreach (MATCH_STRING v, data.strings_match)
        {
            QTreeWidgetItem* strings = new QTreeWidgetItem();
            QString text = v.identifier + QString(" = ") + v.value;
            strings->setText(0, text);
            rule_matched->addChild(strings);
        }
    }

    ui->treeWidget->expandAll();
}
