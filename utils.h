#ifndef H_UTILS
#define H_UTILS
#include <QString>
#include <QFileInfo>
#include <QByteArray>

bool fileExists( QString path);
QString generateRandomString(int length);
QByteArray LoadFromFile( const QString &resourcePath );
bool SaveToFile(const QByteArray& buf, const QString& filePath);

#endif
