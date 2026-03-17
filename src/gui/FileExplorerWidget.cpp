#include "gui/fileexplorerwidget.h"
#include <QHeaderView>
#include <QInputDialog>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include "gui/customiconprovider.h"
#include <QMessageBox>

namespace gs {

    FileExplorerWidget::FileExplorerWidget(QWidget *parent) : QWidget(parent) {
        setObjectName("fileExplorer");
        setMinimumWidth(50);

        auto *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        m_fileModel = new QFileSystemModel(this);
        m_fileModel->setReadOnly(false); // Разрешаем переименование

        QString currentPath = QDir::currentPath();
        m_fileModel->setIconProvider(new CustomIconProvider());
        m_fileModel->setRootPath(currentPath);

        m_treeView = new QTreeView(this);
        m_treeView->setModel(m_fileModel);
        m_treeView->setRootIndex(m_fileModel->index(currentPath));
        m_treeView->setHeaderHidden(true);

        // Прячем лишние колонки
        for (int i = 1; i < m_fileModel->columnCount(); ++i) {
            m_treeView->hideColumn(i);
        }

        m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);

        // Клик ПКМ — вызываем меню
        connect(m_treeView, &QTreeView::customContextMenuRequested, this, &FileExplorerWidget::showContextMenu);

        // Двойной клик — открываем файл
        connect(m_treeView, &QTreeView::doubleClicked, this, [this](const QModelIndex &index) {
            QString path = m_fileModel->filePath(index);
            if (QFileInfo(path).isFile()) {
                emit fileSelected(path);
            }
        });

        layout->addWidget(m_treeView);

        // Чтобы при двойном клике открывался файл
        connect(m_treeView, &QTreeView::doubleClicked, this, [this](const QModelIndex &index) {
            QString path = m_fileModel->filePath(index);
            if (QFileInfo(path).isFile()) {
                emit fileSelected(path);
            }
        });
    }

    void FileExplorerWidget::setRootPath(const QString &path) {
        m_fileModel->setRootPath(path);
        m_treeView->setRootIndex(m_fileModel->index(path));
    }

void FileExplorerWidget::showContextMenu(const QPoint &pos) {
    QModelIndex index = m_treeView->indexAt(pos);
    QMenu contextMenu(this);

    // Эти действия видны всегда (даже если кликнули по пустому месту)
    QAction *newFileAction = contextMenu.addAction("New File...");
    QAction *newDirAction = contextMenu.addAction("New Directory...");
    contextMenu.addSeparator();

    QAction *renameAction = nullptr;
    QAction *deleteAction = nullptr;

    // Если кликнули именно по файлу или папке — добавляем Rename и Delete
    if (index.isValid()) {
        renameAction = contextMenu.addAction("Rename");
        deleteAction = contextMenu.addAction("Delete");
    }

    // ПОКАЗЫВАЕМ МЕНЮ и ждем, что выберет пользователь
    QAction *selected = contextMenu.exec(m_treeView->viewport()->mapToGlobal(pos));

    if (!selected) return; // Если пользователь просто кликнул мимо меню

    // --- ЛОГИКА ОБРАБОТКИ ВЫБОРА ---

    if (selected == newFileAction) {
        bool ok;
        QString name = QInputDialog::getText(this, "New File", "File Name:", QLineEdit::Normal, "", &ok);
        if (ok && !name.isEmpty()) {
            QString path = m_fileModel->filePath(index.isValid() ? index : m_treeView->rootIndex());
            QFileInfo info(path);
            QString dirPath = info.isDir() ? path : info.absolutePath();
            QFile file(dirPath + "/" + name);
            if (file.open(QIODevice::WriteOnly)) file.close();
        }
    }
    else if (selected == newDirAction) {
        bool ok;
        QString name = QInputDialog::getText(this, "New Directory", "Directory Name:", QLineEdit::Normal, "", &ok);
        if (ok && !name.isEmpty()) {
            QString path = m_fileModel->filePath(index.isValid() ? index : m_treeView->rootIndex());
            QFileInfo info(path);
            QString dirPath = info.isDir() ? path : info.absolutePath();
            QDir().mkdir(dirPath + "/" + name);
        }
    }
    else if (selected == renameAction && index.isValid()) {
        m_treeView->edit(index); // ВКЛЮЧАЕТ ПЕРЕИМЕНОВАНИЕ
    }
    else if (selected == deleteAction && index.isValid()) {
        if (QMessageBox::question(this, "Delete", "Удалить безвозвратно?") == QMessageBox::Yes) {
            m_fileModel->remove(index);
        }
    }
}

} // namespace gs