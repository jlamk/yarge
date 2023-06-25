#include "hash.h"
#include <QCryptographicHash>

#include <QString>
#include <QByteArray>
#include <QDataStream>
#include <QFile>

QString calculateFileHash(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        // Handle error: unable to open file
        return QString();
    }

    QCryptographicHash hash(QCryptographicHash::Md5);

    // Read the file in chunks to avoid loading the entire file into memory
    const qint64 bufferSize = 8192;  // Adjust the buffer size as per your requirements
    QByteArray buffer;
    buffer.resize(bufferSize);

    while (!file.atEnd())
    {
        const qint64 bytesRead = file.read(buffer.data(), bufferSize);
        if (bytesRead == -1)
        {
            // Handle error: unable to read from file
            return QString();
        }

        hash.addData(buffer.data(), bytesRead);
    }

    file.close();

    return hash.result().toHex();
}
