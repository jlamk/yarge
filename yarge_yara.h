#ifndef YARGE_YARA_H
#define YARGE_YARA_H

#include <QObject>
#include <QString>

#include <yara.h>

/*
class ERROR_DATA
{
public:
    int error_level;
    QString file_name;
    int line_number;
    YR_RULE rule;
    QString message;
};
*/

class YARGE_YARA : public QObject
{
    Q_OBJECT
public:
    explicit YARGE_YARA(QObject *parent = nullptr);
    void Compile( QString text  );
    QString output;

signals:

};

#endif // YARGE_YARA_H
