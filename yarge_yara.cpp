#include "yarge_yara.h"

#include <yara.h>
#include <QDebug>
#include <yara.h>

YARGE_YARA::YARGE_YARA(QObject *parent)
    : QObject{parent}
{
    yr_initialize();
}

YARGE_YARA::~YARGE_YARA()
{
    yr_finalize();
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

static int callback_match(
        YR_SCAN_CONTEXT* context,
        int message,
        void *message_data,
        void* user_data)
{
    if (message == CALLBACK_MSG_RULE_MATCHING)
    {
        YR_RULE* r = (YR_RULE*) message_data;

        DATA_PTR *data_ptr = (DATA_PTR*)user_data;

        MATCH_DATA_LIST *data = data_ptr->match_data_list_ptr;

        MATCH_DATA newitem;

        newitem.ruleName = r->identifier;
        newitem.filename =data_ptr->filename;
        YR_STRING *s = NULL;

        yr_rule_strings_foreach(r,s)
        {
            MATCH_STRING sstring;
            sstring.identifier = s->identifier;
            sstring.value = QString((char*)s->string);
            newitem.strings_match.append(sstring);
        }

        data->matchs_list.append(newitem);

        return CALLBACK_CONTINUE;
    }

    return CALLBACK_CONTINUE;;
}

void* YARGE_YARA::Compile(  QString text )
{
    if ( text.isEmpty() ) return NULL;

    YR_RULES *rules;
    YR_COMPILER *compiler = NULL;

    if (yr_compiler_create(&compiler) != ERROR_SUCCESS)
    {
        qDebug() << "Error initializing yara compiler";
        return NULL;
    }

    QString output;
    yr_compiler_set_callback( compiler, callback_function, (void*)&output);

    int r = yr_compiler_add_string(compiler, text.toStdString().c_str(), "my_rule_namespace");

    if (r != ERROR_SUCCESS)
    {
        this->output = output;
        yr_compiler_destroy(compiler);
        return NULL;
    }

    if (yr_compiler_get_rules(compiler, &rules) != ERROR_SUCCESS)
    {
        yr_compiler_destroy(compiler);
        return NULL;
    }

    yr_compiler_destroy(compiler);

    return rules;
}

void YARGE_YARA::Scan_File(DATA_PTR *data, const QString &textRule, const QString &filename )
{
    YR_RULES *rules = (YR_RULES*)Compile( textRule );
    yr_rules_scan_file( rules,filename.toStdString().c_str(),SCAN_FLAGS_REPORT_RULES_MATCHING,callback_match, (void*)data,5000);
}
