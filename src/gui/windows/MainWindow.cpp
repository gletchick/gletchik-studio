#include "gui/windows/mainwindow.h"
#include "ui_mainwindow.h"

#include "gui/fileview.h"
#include "core/project/projectmanager.h"
#include "gui/sidebar/sidebar.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSplitter>
#include <QHBoxLayout>
#include <QDebug>

namespace {
    constexpr int SPLITTER_HANDLE_WIDTH = 1;
    constexpr int EDITOR_MIN_WIDTH = 100;
    constexpr int TERMINAL_MIN_HEIGHT = 50;
    constexpr int WINDOW_DEFAULT_WIDTH = 1000;
    constexpr int WINDOW_DEFAULT_HEIGHT = 700;

    const QString WINDOW_TITLE = "Gletchik Studio";
    const QString TERMINAL_PLACEHOLDER = "Terminal >_";
}

namespace gs {

    MainWindow::MainWindow(QWidget *parent)
        : BaseWindow(parent)
        , ui(new Ui::MainWindow) {

        setObjectName("MainWindow");
        resize(WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT);

        setupBaseWindow(WINDOW_TITLE);
    }

    MainWindow::~MainWindow() {
        delete ui;
    }

    void MainWindow::loadProject(const QString &path) {
        if (path.isEmpty()) return;

        m_fileExplorer->setRootPath(path);
        setWindowTitle(WINDOW_TITLE + " - " + path);
    }

    void MainWindow::setupWorkspace(QWidget *contentWidget) {
        ui->setupUi(contentWidget);

        // 1. Инициализация сплиттеров
        m_vSplitter = new QSplitter(Qt::Vertical, contentWidget);
        m_vSplitter->setHandleWidth(SPLITTER_HANDLE_WIDTH);
        m_vSplitter->setChildrenCollapsible(false);

        QWidget *upperWidget = new QWidget(contentWidget);
        QHBoxLayout *upperLayout = new QHBoxLayout(upperWidget);
        upperLayout->setContentsMargins(0, 0, 0, 0);
        upperLayout->setSpacing(0);

        Sidebar *sidebar = new Sidebar(upperWidget);
        upperLayout->addWidget(sidebar);

        m_hSplitter = new QSplitter(Qt::Horizontal, upperWidget);
        m_hSplitter->setHandleWidth(SPLITTER_HANDLE_WIDTH);
        m_hSplitter->setChildrenCollapsible(false);

        // 2. Инициализация компонентов
        m_fileExplorer = new FileExplorerWidget(upperWidget);

        // Создаем наш FileView
        m_editor = new FileView(upperWidget);
        m_editor->setObjectName("codeEditor");
        m_editor->setMinimumWidth(EDITOR_MIN_WIDTH);

        m_hSplitter->addWidget(m_fileExplorer);
        m_hSplitter->addWidget(m_editor);
        m_hSplitter->setStretchFactor(0, 0);
        m_hSplitter->setStretchFactor(1, 1);

        upperLayout->addWidget(m_hSplitter);

        m_terminal = new QTextEdit(contentWidget);
        m_terminal->setObjectName("terminalWidget");
        m_terminal->setPlaceholderText(TERMINAL_PLACEHOLDER);
        m_terminal->setMinimumHeight(TERMINAL_MIN_HEIGHT);

        m_vSplitter->addWidget(upperWidget);
        m_vSplitter->addWidget(m_terminal);
        m_vSplitter->setStretchFactor(0, 1);
        m_vSplitter->setStretchFactor(1, 0);

        if (ui->mainVerticalLayout) {
            ui->mainVerticalLayout->addWidget(m_vSplitter);
        }

        // --- ЛОГИКА АВТОСОХРАНЕНИЯ И ПЕРЕКЛЮЧЕНИЯ ---

        connect(sidebar, &Sidebar::projectToggled, this, &MainWindow::onProjectToggled);
        connect(sidebar, &Sidebar::terminalToggled, this, &MainWindow::onTerminalToggled);

        connect(m_fileExplorer, &FileExplorerWidget::fileSelected, this, [this](const QString &newPath) {
            // Если в редакторе уже открыт какой-то файл
            if (!m_editor->currentFilePath().isEmpty()) {
                // Если текст был изменен (isModified), сохраняем его
                if (m_editor->document()->isModified()) {
                    qDebug() << "Auto-saving:" << m_editor->currentFilePath();
                    m_editor->saveCurrentFile();
                }
            }

            // Загружаем новый файл
            qDebug() << "Opening new file:" << newPath;
            if (!m_editor->loadFile(newPath)) {
                QMessageBox::warning(this, "Error", "Could not open file:\n" + newPath);
            }
        });
    }

    void MainWindow::onProjectToggled(bool checked) {
        if (m_fileExplorer) m_fileExplorer->setVisible(checked);
    }

    void MainWindow::onTerminalToggled(bool checked) {
        if (m_terminal) m_terminal->setVisible(checked);
    }

    void MainWindow::onOpenProjectTriggered() {
        QString dir = QFileDialog::getExistingDirectory(this, "Select Project", QDir::homePath());
        if (dir.isEmpty()) return;

        if (ProjectManager::instance().openProject(dir)) {
            loadProject(dir);
        } else {
            QMessageBox::warning(this, "Error", "Selected directory is not a valid project.");
        }
    }

} // namespace gs