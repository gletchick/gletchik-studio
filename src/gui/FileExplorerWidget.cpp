#include "gui/customiconprovider.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QFile>

#include "gui/fileexplorerwidget.h"

namespace {
    constexpr int MIN_WIDGET_WIDTH = 50;
    constexpr int TREE_INDENTATION = 12;

    const QString ACTION_NEW_FILE_TEXT = "New File...";
    const QString ACTION_NEW_DIR_TEXT = "New Directory...";
    const QString ACTION_RENAME_TEXT = "Rename";
    const QString ACTION_DELETE_TEXT = "Delete";

    const QString DIALOG_NEW_FILE_TITLE = "New File";
    const QString DIALOG_NEW_FILE_LABEL = "File Name:";
    const QString DIALOG_NEW_DIR_TITLE = "New Directory";
    const QString DIALOG_NEW_DIR_LABEL = "Directory Name:";
    const QString DIALOG_DELETE_TITLE = "Delete";
    const QString DIALOG_DELETE_TEXT = "Удалить безвозвратно?";
}

namespace gs {

    FileExplorerWidget::FileExplorerWidget(QWidget *parent) : QWidget(parent) {
        setObjectName("fileExplorer");
        setMinimumWidth(MIN_WIDGET_WIDTH);

        auto *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        m_fileModel = new QFileSystemModel(this);
        m_fileModel->setReadOnly(false);

        QString currentPath = QDir::currentPath();
        m_fileModel->setIconProvider(new CustomIconProvider());
        m_fileModel->setRootPath(currentPath);

        m_treeView = new QTreeView(this);
        m_treeView->setModel(m_fileModel);
        m_treeView->setRootIndex(m_fileModel->index(currentPath));
        m_treeView->setHeaderHidden(true);
        m_treeView->setIndentation(TREE_INDENTATION);

        for (int i = 1; i < m_fileModel->columnCount(); ++i) {
            m_treeView->hideColumn(i);
        }

        m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);

        connect(m_treeView, &QTreeView::customContextMenuRequested, this, &FileExplorerWidget::showContextMenu);
        connect(m_treeView, &QTreeView::doubleClicked, this, &FileExplorerWidget::onDoubleClicked);

        layout->addWidget(m_treeView);
    }

    void FileExplorerWidget::setRootPath(const QString &path) {
        m_fileModel->setRootPath(path);
        m_treeView->setRootIndex(m_fileModel->index(path));
    }

    void FileExplorerWidget::onDoubleClicked(const QModelIndex &index) {
        QString path = m_fileModel->filePath(index);
        if (QFileInfo(path).isFile()) {
            emit fileSelected(path);
        }
    }

    void FileExplorerWidget::showContextMenu(const QPoint &pos) {
        QModelIndex index = m_treeView->indexAt(pos);
        QMenu contextMenu(this);

        QAction *newFileAction = contextMenu.addAction(ACTION_NEW_FILE_TEXT);
        QAction *newDirAction = contextMenu.addAction(ACTION_NEW_DIR_TEXT);
        contextMenu.addSeparator();

        QAction *renameAction = nullptr;
        QAction *deleteAction = nullptr;

        if (index.isValid()) {
            renameAction = contextMenu.addAction(ACTION_RENAME_TEXT);
            deleteAction = contextMenu.addAction(ACTION_DELETE_TEXT);
        }

        QAction *selected = contextMenu.exec(m_treeView->viewport()->mapToGlobal(pos));

        if (!selected) return;

        if (selected == newFileAction) {
            bool ok;
            QString name = QInputDialog::getText(this, DIALOG_NEW_FILE_TITLE, DIALOG_NEW_FILE_LABEL, QLineEdit::Normal, "", &ok);
            if (ok && !name.isEmpty()) {
                QString path = m_fileModel->filePath(index.isValid() ? index : m_treeView->rootIndex());
                QFileInfo info(path);
                QString dirPath = info.isDir() ? path : info.absolutePath();
                QFile file(dirPath + "/" + name);
                if (file.open(QIODevice::WriteOnly)) {
                    file.close();
                }
            }
        }
        else if (selected == newDirAction) {
            bool ok;
            QString name = QInputDialog::getText(this, DIALOG_NEW_DIR_TITLE, DIALOG_NEW_DIR_LABEL, QLineEdit::Normal, "", &ok);
            if (ok && !name.isEmpty()) {
                QString path = m_fileModel->filePath(index.isValid() ? index : m_treeView->rootIndex());
                QFileInfo info(path);
                QString dirPath = info.isDir() ? path : info.absolutePath();
                QDir().mkdir(dirPath + "/" + name);
            }
        }
        else if (selected == renameAction && index.isValid()) {
            m_treeView->edit(index);
        }
        else if (selected == deleteAction && index.isValid()) {
            if (QMessageBox::question(this, DIALOG_DELETE_TITLE, DIALOG_DELETE_TEXT) == QMessageBox::Yes) {
                m_fileModel->remove(index);
            }
        }
    }

} // namespace gs