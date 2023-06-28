#ifndef YARGE_YARA_H
#define YARGE_YARA_H

#include <QObject>
#include <QString>
#include <QVector>

class MATCH_STRING
{
public:
    QString identifier;
    QString value;
};

class MATCH_DATA
{
public:
    QString ruleName;
    QString filename;
    QVector<MATCH_STRING> strings_match;
};

class MATCH_DATA_LIST
{
public:
    QVector<MATCH_DATA> matchs_list;
};

class DATA_PTR
{
public:
    QString filename;
    MATCH_DATA_LIST *match_data_list_ptr;
};


class YARGE_YARA : public QObject
{
    Q_OBJECT
public:
    explicit YARGE_YARA(QObject *parent = nullptr);
    void* Compile(  QString text );
    QString output;

    void Scan_File(DATA_PTR *data, const QString &textRule, const QString &filename );

signals:

};

#endif // YARGE_YARA_H
