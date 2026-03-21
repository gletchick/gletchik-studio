#include <QFileDialog>
#include <QMessageBox>

#include "gui/windows/mainwindow.h"
#include "ui_mainwindow.h"
#include "core/project/projectmanager.h"
#include "gui/sidebar/sidebar.h"

namespace {
    constexpr int SPLITTER_HANDLE_WIDTH = 1;
    constexpr int EDITOR_MIN_WIDTH = 100;
    constexpr int TERMINAL_MIN_HEIGHT = 50;
    constexpr int WINDOW_DEFAULT_WIDTH = 1000;
    constexpr int WINDOW_DEFAULT_HEIGHT = 700;

    const QString WINDOW_TITLE = "My Application";
    const QString CODE_EDITOR_PLACEHOLDER = "// Пишите код здесь...";
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

    void MainWindow::loadProject(const QString &path) {
        if (path.isEmpty()) return;

        // Обновляем дерево файлов
        m_fileExplorer->setRootPath(path);

        // Обновляем заголовок окна для информативности
        setWindowTitle(WINDOW_TITLE + " - " + path);
    }

    MainWindow::~MainWindow() {
        delete ui;
    }

    void MainWindow::setupWorkspace(QWidget *contentWidget) {
        ui->setupUi(contentWidget);

        m_vSplitter = new QSplitter(Qt::Vertical, contentWidget);
        m_vSplitter->setHandleWidth(SPLITTER_HANDLE_WIDTH);
        m_vSplitter->setChildrenCollapsible(false);

        QWidget *upperWidget = new QWidget(contentWidget);
        QHBoxLayout *upperLayout = new QHBoxLayout(upperWidget);
        upperLayout->setContentsMargins(0, 0, 0, 0);
        upperLayout->setSpacing(0);

        Sidebar *sidebar = new Sidebar(contentWidget);
        upperLayout->addWidget(sidebar);

        m_hSplitter = new QSplitter(Qt::Horizontal, upperWidget);
        m_hSplitter->setHandleWidth(SPLITTER_HANDLE_WIDTH);
        m_hSplitter->setChildrenCollapsible(false);

        // Инициализируем наш кастомный класс
        m_fileExplorer = new FileExplorerWidget(contentWidget);

        QTextEdit *editor = new QTextEdit(contentWidget);
        editor->setObjectName("codeEditor");
        editor->setPlaceholderText(CODE_EDITOR_PLACEHOLDER);
        editor->setMinimumWidth(EDITOR_MIN_WIDTH);

        m_hSplitter->addWidget(m_fileExplorer);
        m_hSplitter->addWidget(editor);

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

        connect(sidebar, &Sidebar::projectToggled, this, &MainWindow::onProjectToggled);
        connect(sidebar, &Sidebar::terminalToggled, this, &MainWindow::onTerminalToggled);

        connect(m_fileExplorer, &FileExplorerWidget::fileSelected, this, [editor](const QString &filePath) {
            // Заглушка: здесь будет логика открытия файла и чтения его содержимого в editor
            editor->append(QString("Opening: %1").arg(filePath));
        });
    }

    void MainWindow::onProjectToggled(bool checked) {
        m_fileExplorer->setVisible(checked);
    }

    void MainWindow::onTerminalToggled(bool checked) {
        m_terminal->setVisible(checked);
    }

    void MainWindow::onOpenProjectTriggered() {
        QString dir = QFileDialog::getExistingDirectory(this, "Select Project", QDir::homePath());

        if (dir.isEmpty()) return;

        // 1. Пытаемся открыть проект через менеджер
        // Если папки .gs нет, можно предложить создать проект (createProject)
        if (ProjectManager::instance().openProject(dir)) {

            // 2. Если менеджер успешно подгрузил индекс, обновляем UI
            m_fileExplorer->setRootPath(dir);
        } else {
            QMessageBox::warning(this, "Error", "Selected directory is not a valid project (no .gs folder found).");
        }
    }

} // namespace gs