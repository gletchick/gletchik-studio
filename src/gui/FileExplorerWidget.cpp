#include "gui/customiconprovider.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QDebug>

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
        m_fileModel->setIconProvider(new CustomIconProvider());

        m_treeView = new QTreeView(this);
        m_treeView->setModel(m_fileModel);
        m_treeView->setIndentation(TREE_INDENTATION);
        m_treeView->setAnimated(true);
        m_treeView->setSortingEnabled(true);
        m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);
        m_treeView->setHeaderHidden(true);

        for (int i = 1; i < m_fileModel->columnCount(); ++i) {
            m_treeView->hideColumn(i);
        }

        layout->addWidget(m_treeView);

        connect(m_treeView, &QTreeView::customContextMenuRequested, this, &FileExplorerWidget::showContextMenu);
        connect(m_treeView, &QTreeView::doubleClicked, this, &FileExplorerWidget::onDoubleClicked);
    }

    void FileExplorerWidget::setRootPath(const QString &path) {
        m_fileModel->setRootPath(path);
        m_treeView->setRootIndex(m_fileModel->index(path));
    }

    void FileExplorerWidget::onDoubleClicked(const QModelIndex &index) {
        if (!index.isValid()) return;
        QString path = m_fileModel->filePath(index);

        if (!path.isEmpty() && QFileInfo(path).isFile()) {
            emit fileSelected(path);
        }
    }

    void FileExplorerWidget::showContextMenu(const QPoint &pos) {
        QModelIndex index = m_treeView->indexAt(pos);
        QMenu menu(this);

        QAction *newFileAction = menu.addAction(ACTION_NEW_FILE_TEXT);
        QAction *newDirAction = menu.addAction(ACTION_NEW_DIR_TEXT);
        menu.addSeparator();
        QAction *renameAction = menu.addAction(ACTION_RENAME_TEXT);
        QAction *deleteAction = menu.addAction(ACTION_DELETE_TEXT);

        QAction *selected = menu.exec(m_treeView->viewport()->mapToGlobal(pos));

        if (!selected) return;

        // Определяем папку, где будем создавать
        QString targetDirPath;
        if (index.isValid()) {
            QString itemPath = m_fileModel->filePath(index);
            targetDirPath = QFileInfo(itemPath).isDir() ? itemPath : QFileInfo(itemPath).absolutePath();
        } else {
            targetDirPath = m_fileModel->rootPath();
        }

        QDir dir(targetDirPath);

        if (selected == newFileAction) {
            bool ok;
            QString name = QInputDialog::getText(this, DIALOG_NEW_FILE_TITLE, DIALOG_NEW_FILE_LABEL, QLineEdit::Normal, "", &ok);

            if (ok && !name.isEmpty()) {
                QString fullPath = dir.absoluteFilePath(name);

                QFile file(fullPath);
                // Временно блокируем сигналы дерева, чтобы оно не пыталось само открыть файл пока мы его создаем
                m_treeView->blockSignals(true);

                if (file.open(QIODevice::WriteOnly)) {
                    file.close();

                    // Даем модели команду обновиться
                    m_fileModel->fetchMore(m_fileModel->index(targetDirPath));

                    // Эмитим сигнал открытия напрямую
                    qDebug() << "[Explorer] Success creation, opening:" << fullPath;
                    emit fileSelected(fullPath);
                } else {
                    QMessageBox::critical(this, "Error", "Create failed: " + file.errorString());
                }

                m_treeView->blockSignals(false);
            }
        }
        else if (selected == newDirAction) {
            bool ok;
            QString name = QInputDialog::getText(this, DIALOG_NEW_DIR_TITLE, DIALOG_NEW_DIR_LABEL, QLineEdit::Normal, "", &ok);
            if (ok && !name.isEmpty()) {
                dir.mkdir(name);
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