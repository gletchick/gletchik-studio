#pragma once

#include <QWidget>
#include <QTreeView>
#include <QFileSystemModel>
#include <QVBoxLayout>
#include <QMenu>

namespace gs {

    class FileExplorerWidget : public QWidget {
        Q_OBJECT

    public:
        explicit FileExplorerWidget(QWidget *parent = nullptr);
        
        // Метод для установки корневой директории проекта
        void setRootPath(const QString &path);

    private slots:
        // Слот для отображения контекстного меню при клике ПКМ
        void showContextMenu(const QPoint &pos);

    private:
        QTreeView *m_treeView;
        QFileSystemModel *m_fileModel;
    };

} // namespace gs