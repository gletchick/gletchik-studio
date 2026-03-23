#include "gui/customiconprovider.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QMenu>
#include <QAction>

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

        // ОТКЛЮЧАЕМ редактирование по двойному клику
        m_treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);

        // Скрываем колонки с размером, типом и датой
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

        // Открываем только если это файл (пустые файлы теперь тоже пройдут)
        if (!path.isEmpty() && QFileInfo(path).isFile()) {
            qDebug() << "[Explorer] Double click opening:" << path;
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

    // Определяем рабочую папку
    QString targetPath = m_fileModel->filePath(index.isValid() ? index : m_treeView->rootIndex());
    if (targetPath.isEmpty()) targetPath = m_fileModel->rootPath();

    QFileInfo targetInfo(targetPath);
    QDir dir(targetInfo.isDir() ? targetPath : targetInfo.absolutePath());

    if (selected == newFileAction) {
        // --- ЗАМЕНА СТАТИКИ НА ОБЪЕКТ ДЛЯ ТЕМНОЙ РАМКИ ---
        QInputDialog dialog(this);
        dialog.setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
        dialog.setWindowTitle(DIALOG_NEW_FILE_TITLE);
        dialog.setLabelText(DIALOG_NEW_FILE_LABEL);

        if (dialog.exec() == QDialog::Accepted) {
            QString name = dialog.textValue();
            if (!name.isEmpty()) {
                QString fullFilePath = dir.absoluteFilePath(name);
                QFile file(fullFilePath);

                // Твоя логика сигналов и создания
                m_treeView->blockSignals(true);

                if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
                    file.write("");
                    file.flush();
                    file.close();

                    qDebug() << "[Explorer] New file created:" << fullFilePath;

                    m_fileModel->fetchMore(m_fileModel->index(dir.absolutePath()));
                    emit fileSelected(fullFilePath);
                } else {
                    // Темный MessageBox
                    QMessageBox mb(QMessageBox::Critical, "Error", "Can't create file: " + file.errorString(), QMessageBox::Ok, this);
                    mb.setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
                    mb.exec();
                }

                m_treeView->blockSignals(false);
            }
        }
    }
    else if (selected == newDirAction) {
        // --- ЗАМЕНА СТАТИКИ НА ОБЪЕКТ ДЛЯ ТЕМНОЙ РАМКИ ---
        QInputDialog dialog(this);
        dialog.setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
        dialog.setWindowTitle(DIALOG_NEW_DIR_TITLE);
        dialog.setLabelText(DIALOG_NEW_DIR_LABEL);

        if (dialog.exec() == QDialog::Accepted) {
            QString name = dialog.textValue();
            if (!name.isEmpty()) {
                if (!dir.mkdir(name)) {
                    QMessageBox mb(QMessageBox::Critical, "Error", "Could not create directory", QMessageBox::Ok, this);
                    mb.setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
                    mb.exec();
                }
            }
        }
    }
    else if (selected == renameAction && index.isValid()) {
        m_treeView->edit(index);
    }
    else if (selected == deleteAction && index.isValid()) {
        // Темный вопрос об удалении
        QMessageBox mb(QMessageBox::Question, DIALOG_DELETE_TITLE, DIALOG_DELETE_TEXT, QMessageBox::Yes | QMessageBox::No, this);
        mb.setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
        if (mb.exec() == QMessageBox::Yes) {
            m_fileModel->remove(index);
        }
    }
}

} // namespace gs