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
        void setRootPath(const QString &path);

    signals:
        void fileSelected(const QString &filePath);

    private slots:
        void showContextMenu(const QPoint &pos);
        void onDoubleClicked(const QModelIndex &index);

    private:
        QTreeView *m_treeView;
        QFileSystemModel *m_fileModel;
    };

} // namespace gs