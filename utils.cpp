
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
