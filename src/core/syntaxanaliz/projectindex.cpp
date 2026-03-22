#include "core/syntaxanaliz/projectindex.h"

#include <QJsonArray>

namespace gs {

void ProjectIndex::loadFromJson(const QJsonObject &root) {
    m_fileEntries.clear();
    QJsonObject filesObj = root[kFiles].toObject();

    for (auto it = filesObj.begin(); it != filesObj.end(); ++it) {
        QString path = it.key();
        QJsonObject data = it.value().toObject();

        FileData fd;
        fd.hash = data[kHash].toString();
        
        QJsonArray clsArr = data[kClasses].toArray();
        for (const auto &v : clsArr) fd.classes.insert(v.toString());

        QJsonArray methArr = data[kMethods].toArray();
        for (const auto &v : methArr) fd.methods.insert(v.toString());

        m_fileEntries[path] = fd;
    }
    rebuildGlobalIndex();
}

QJsonObject ProjectIndex::toJson() const {
    QJsonObject root;
    QJsonObject filesObj;

    for (auto it = m_fileEntries.begin(); it != m_fileEntries.end(); ++it) {
        QJsonObject fileData;
        fileData[kHash] = it.value().hash;
        fileData[kClasses] = QJsonArray::fromStringList(it.value().classes.values());
        fileData[kMethods] = QJsonArray::fromStringList(it.value().methods.values());
        filesObj[it.key()] = fileData;
    }

    root[kFiles] = filesObj;
    return root;
}

void ProjectIndex::updateFile(const QString &filePath, const QString &hash, 
                              const QSet<QString> &classes, const QSet<QString> &methods) {
    FileData fd;
    fd.hash = hash;
    fd.classes = classes;
    fd.methods = methods;

    m_fileEntries[filePath] = fd;
    
    // Вместо полной пересборки можно точечно обновлять сеты, 
    // но для начала rebuildGlobalIndex безопаснее.
    rebuildGlobalIndex();
}

void ProjectIndex::rebuildGlobalIndex() {
    m_globalClasses.clear();
    m_globalMethods.clear();

    for (const auto &fd : m_fileEntries) {
        m_globalClasses.unite(fd.classes);
        m_globalMethods.unite(fd.methods);
    }
}

bool ProjectIndex::isClass(const QString &name) const {
    return m_globalClasses.contains(name);
}

bool ProjectIndex::isMethod(const QString &name) const {
    return m_globalMethods.contains(name);
}

} // namespace gs