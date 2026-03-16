
#include "gui/customiconprovider.h"

namespace gs {

    CustomIconProvider::CustomIconProvider() {
        // Укажите путь к вашему SVG в ресурсах
        m_customFolderIcon = QIcon(":/icons/project.svg");
    }

    QIcon CustomIconProvider::icon(const QFileInfo &info) const {
        if (info.isDir()) {
            return m_customFolderIcon;
        }
        
        // Для файлов возвращаем стандартную системную иконку
        return QFileIconProvider::icon(info);
    }

    QIcon CustomIconProvider::icon(IconType type) const {
        if (type == QFileIconProvider::Folder) {
            return m_customFolderIcon;
        }
        
        return QFileIconProvider::icon(type);
    }

} // namespace gs