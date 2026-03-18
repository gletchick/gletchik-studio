#include <QFile>
#include <QCryptographicHash>

namespace Utils {
    QString calculateFileHash(const QString &filePath) {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            return QString(); // Возвращаем пустую строку при ошибке
        }

        QCryptographicHash hash(QCryptographicHash::Md5);
        if (hash.addData(&file)) {
            return QString(hash.result().toHex());
        }
        
        return QString();
    }
}