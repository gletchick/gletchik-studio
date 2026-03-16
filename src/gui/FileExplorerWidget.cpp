#include "gui/fileexplorerwidget.h" // Убедись, что путь правильный
#include <QHeaderView>

#include "gui/customiconprovider.h"

namespace gs {

    FileExplorerWidget::FileExplorerWidget(QWidget *parent) : QWidget(parent) {
        setObjectName("fileExplorer");
        setMinimumWidth(50); // Сохраняем твое ограничение для плавности сплиттера

        auto *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        // Инициализируем модель файловой системы
        m_fileModel = new QFileSystemModel(this);
        QString currentPath = QDir::currentPath();
        m_fileModel->setIconProvider(new CustomIconProvider());
        m_fileModel->setRootPath(QDir::currentPath());

        // Настраиваем TreeView
        m_treeView = new QTreeView(this);
        m_treeView->setModel(m_fileModel);
        m_treeView->setRootIndex(m_fileModel->index(currentPath));
        m_treeView->setHeaderHidden(true); // Скрываем заголовки столбцов
        m_treeView->setFocusPolicy(Qt::NoFocus);

        QPalette palette = m_treeView->palette();
        QColor selectColor("#37373d");
        palette.setColor(QPalette::Highlight, selectColor);
        palette.setColor(QPalette::HighlightedText, QColor("#cccccc")); // Цвет текста при выделении
        m_treeView->setPalette(palette);

        // Прячем все столбцы (размер, тип, дата), кроме имени файла
        for (int i = 1; i < m_fileModel->columnCount(); ++i) {
            m_treeView->hideColumn(i);
        }

        // Включаем поддержку кастомного контекстного меню
        m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(m_treeView, &QTreeView::customContextMenuRequested, this, &FileExplorerWidget::showContextMenu);

        layout->addWidget(m_treeView);
    }

    void FileExplorerWidget::setRootPath(const QString &path) {
        m_fileModel->setRootPath(path);
        m_treeView->setRootIndex(m_fileModel->index(path));
    }

    void FileExplorerWidget::showContextMenu(const QPoint &pos) {
        QModelIndex index = m_treeView->indexAt(pos);
        
        QMenu contextMenu(this);
        
        // Добавляем действия (пока без реального функционала)
        QAction *newFileAction = contextMenu.addAction("New File...");
        QAction *newDirAction = contextMenu.addAction("New Directory...");
        contextMenu.addSeparator();
        
        // Если кликнули по конкретному файлу/папке, добавляем действия для них
        if (index.isValid()) {
            QAction *renameAction = contextMenu.addAction("Rename");
            QAction *deleteAction = contextMenu.addAction("Delete");
        }

        // Показываем меню в точке клика
        contextMenu.exec(m_treeView->viewport()->mapToGlobal(pos));
    }

} // namespace gs