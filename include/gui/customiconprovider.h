#pragma once

#include <QFileIconProvider>
#include <QIcon>
#include <QFileInfo>

namespace gs {

    class CustomIconProvider : public QFileIconProvider {
    public:
        CustomIconProvider();

        // Основной метод, который запрашивает модель для отрисовки в дереве
        QIcon icon(const QFileInfo &info) const override;
        
        // Метод для стандартных типов (на случай, если инфо недоступно)
        QIcon icon(IconType type) const override;

    private:
        QIcon m_customFolderIcon;
    };

} // namespace gs