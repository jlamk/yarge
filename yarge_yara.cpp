#include "yarge_yara.h"

#include <yara.h>
#include <QDebug>

YARGE_YARA::YARGE_YARA(QObject *parent)
    : QObject{parent}
{
    yr_initialize();
}

void callback_function(
    int error_level,
    const char* file_name,
    int line_number,
    const YR_RULE* rule,
    const char* message,
    void* user_data)
{
    QString *output = (QString*)user_data;
    char buf[1024];
    sprintf(buf," %s at line %d\n",message,line_number);
    *output += buf;
}

void YARGE_YARA::Compile( QString text )
{
    if ( text.isEmpty() ) return;
    YR_RULES *rules;
    YR_COMPILER *compiler = NULL;

    if (yr_compiler_create(&compiler) != ERROR_SUCCESS)
    {
        qDebug() << "Error initializing yara compiler";
        return;
    }

    QString output;
    yr_compiler_set_callback( compiler, callback_function, (void*)&output);

    int r = yr_compiler_add_string(compiler, text.toStdString().c_str(), "my_rule_namespace");

    if (r != ERROR_SUCCESS)
    {
        this->output = output;
        qInfo() << output;
        yr_compiler_destroy(compiler);
        return;
    }

    if (yr_compiler_get_rules(compiler, &rules) != ERROR_SUCCESS)
    {
        yr_compiler_destroy(compiler);
    }

}
