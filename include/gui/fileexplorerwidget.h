#pragma once

#include <QWidget>
#include <QTreeView>
#include <QFileSystemModel>
#include <QVBoxLayout>
#include <QMenu>
#include <QModelIndex>

// Подключаем провайдер иконок, чтобы хранить его как поле класса
#include "gui/customiconprovider.h"

namespace gs {

    class FileExplorerWidget : public QWidget {
        Q_OBJECT // Самый важный макрос для работы сигналов

    public:
        explicit FileExplorerWidget(QWidget *parent = nullptr);
        // Деструктор нужен для очистки ресурсов (провайдера иконок)
        ~FileExplorerWidget() override = default;

        void setRootPath(const QString &path);

        signals:
            // Убедись, что тип аргумента совпадает с тем, что ты шлешь в .cpp
            void fileSelected(const QString &filePath);

    private slots:
        void showContextMenu(const QPoint &pos);
        void onDoubleClicked(const QModelIndex &index);

    private:
        QTreeView *m_treeView = nullptr;
        QFileSystemModel *m_fileModel = nullptr;

        // Храним провайдер здесь, чтобы Qt корректно управлял его памятью
        CustomIconProvider *m_iconProvider = nullptr;
    };

} // namespace gs