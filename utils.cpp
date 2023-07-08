
#include <QRandomGenerator>
#include <QString>
#include <QFileInfo>

QString generateRandomString(int length)
{
    const QString characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    QString randomString;

    for (int i = 0; i < length; ++i) {
        int randomIndex = QRandomGenerator::global()->bounded(characters.length());
        QChar randomChar = characters.at(randomIndex);
        randomString.append(randomChar);
    }

    return randomString;
}

bool fileExists( QString path) {
    QFileInfo check_file(path);
    if (check_file.exists() && check_file.isFile()) {
        return true;
    } else {
        return false;
    }
}

QByteArray LoadFromFile( const QString &resourcePath )
{
    QFile file(resourcePath);
    if (!file.open(QIODevice::ReadOnly))
        return QByteArray();

    QByteArray data;
    const int bufferSize = 8192;  // Adjust the buffer size as needed

    while (!file.atEnd()) {
        QByteArray chunk = file.read(bufferSize);
        data.append(chunk);
    }

    file.close();
    return data;
}

bool SaveToFile(const QByteArray& buf, const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly))
        return false;

    qint64 bytesWritten = file.write(buf);
    file.close();

    return bytesWritten == buf.size();
}

