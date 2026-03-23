#include "gui/windows/mainwindow.h"
#include "ui_mainwindow.h"

#include "gui/fileview.h"
#include "core/project/projectmanager.h"
#include "gui/sidebar/sidebar.h"
#include "core/languages/java/javalanguageprovider.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSplitter>
#include <QHBoxLayout>
#include <QTimer>
#include <QFileInfo>
#include <QMetaObject>
#include <thread>
#include <QShortcut>

#include "core/languages/java/javabuildbontroller.h"
#include "gui/terminalwidget.h"

namespace {
    constexpr int SPLITTER_HANDLE_WIDTH = 1;
    constexpr int EDITOR_MIN_WIDTH = 100;
    constexpr int TERMINAL_MIN_HEIGHT = 50;
    constexpr int WINDOW_DEFAULT_WIDTH = 1000;
    constexpr int WINDOW_DEFAULT_HEIGHT = 700;

    const QString WINDOW_TITLE = "Gletchik Studio";
    const QString TERMINAL_PLACEHOLDER = "Terminal >_";

    const QString MSG_ERR_OPEN_FILE = "Could not open file:\n";
    const QString MSG_ERR_NOT_PROJECT = "Selected directory is not a valid project.";
    const QString MSG_NO_FILE_OPENED = "No file opened to run.\n";
    const QString MSG_FILE_NOT_RUNNABLE = "Current file is not runnable (missing main method).\n";
    const QString MSG_BUILD_STARTING = "Starting build process...\n";
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

        m_buildController = std::make_shared<JavaBuildController>();
        m_javaProvider = std::make_shared<JavaLanguageProvider>(nullptr);

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

        m_fileExplorer = new FileExplorerWidget(upperWidget);

        m_editor = new FileView(upperWidget);
        m_editor->setObjectName("codeEditor");
        m_editor->setMinimumWidth(EDITOR_MIN_WIDTH);

        m_hSplitter->addWidget(m_fileExplorer);
        m_hSplitter->addWidget(m_editor);
        m_hSplitter->setStretchFactor(0, 0);
        m_hSplitter->setStretchFactor(1, 1);

        upperLayout->addWidget(m_hSplitter);

        m_terminal = new TerminalWidget(contentWidget);
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
        connect(sidebar, &Sidebar::runClicked, this, &MainWindow::onRunClicked);

        connect(m_fileExplorer, &FileExplorerWidget::fileSelected, this, [this](const QString &newPath) {
            if (!m_editor->currentFilePath().isEmpty()) {
                if (m_editor->document()->isModified()) {
                    m_editor->saveCurrentFile();
                }
            }

            if (!m_editor->loadFile(newPath)) {
                QMessageBox::warning(this, "Error", MSG_ERR_OPEN_FILE + newPath);
            }
        });

        connect(m_terminal, &TerminalWidget::inputReady, this, &MainWindow::onTerminalInput);

        m_updateTimer = new QTimer(this);
        connect(m_updateTimer, &QTimer::timeout, this, &MainWindow::processOutputQueue);
        m_updateTimer->start(50);

        QShortcut *refreshShortcut = new QShortcut(QKeySequence("Ctrl+R"), this);
        connect(refreshShortcut, &QShortcut::activated, this, &MainWindow::onRefreshIndexTriggered);

        if (auto worker = ProjectManager::instance().getWorker()) {
            connect(worker, &IndexWorker::fileIndexed, this, [this](const QString& path) {
                if (m_editor->currentFilePath() == path) {
                    m_editor->rehighlight();
                }
            });

            connect(worker, &IndexWorker::indexingFinished, this, [this]() {
                m_terminal->appendOutput("Background indexing finished.\n", false);
            });
        }
    }

    void MainWindow::onTerminalInput(const QString& text) {
        if (m_buildController) {
            m_buildController->writeInput(text.toStdString());
        }
    }

    void MainWindow::onRunClicked() {
        const QString currentFilePath = m_editor->currentFilePath();
        if (currentFilePath.isEmpty()) {
            m_terminal->appendOutput(MSG_NO_FILE_OPENED, true);
            return;
        }

        if (m_editor->document()->isModified()) {
            m_editor->saveCurrentFile();
        }

        const QString content = m_editor->document()->toPlainText();
        if (!m_javaProvider->isRunnable(currentFilePath.toStdString(), content.toStdString())) {
            m_terminal->appendOutput(MSG_FILE_NOT_RUNNABLE, true);
            return;
        }

        m_terminal->clearTerminal();
        m_terminal->appendOutput(MSG_BUILD_STARTING, false);

        if (!m_terminal->isVisible()) {
            m_terminal->setVisible(true);
        }

        const std::string projPath = QFileInfo(currentFilePath).absolutePath().toStdString();
        const std::string sourceFile = currentFilePath.toStdString();

        // Запускаем через поток и очередь (как в твоем "рабочем" примере)
        std::thread([this, projPath, sourceFile]() {
            m_buildController->runProject(
                projPath,
                sourceFile,
                StepType::Run,
                [this](const std::string& text, bool isError) {
                    // Маркируем поток вывода
                    std::string prefix = isError ? "E:" : "O:";
                    m_outputQueue.push(prefix + text);
                }
            );
            m_outputQueue.push("O:\n--- Process Finished ---\n");
        }).detach();
    }

    void MainWindow::processOutputQueue() {
        // Выгребаем всё из очереди за один тик таймера
        while (auto msg = m_outputQueue.tryPop()) {
            QString text = QString::fromStdString(*msg);
            bool isError = text.startsWith("E:");
            m_terminal->appendOutput(text.mid(2), isError);
        }
    }

    void MainWindow::onProjectToggled(bool checked) {
        if (m_fileExplorer) m_fileExplorer->setVisible(checked);
    }

    void MainWindow::onTerminalToggled(bool checked) {
        if (m_terminal) m_terminal->setVisible(checked);
    }

    void MainWindow::onOpenProjectTriggered() {
        QFileDialog dialog(this);
        dialog.setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

        dialog.setWindowTitle("Select Project");
        dialog.setFileMode(QFileDialog::Directory);
        dialog.setOption(QFileDialog::DontUseNativeDialog, true); // Обязательно!

        if (dialog.exec()) {
            QStringList dirs = dialog.selectedFiles();
            if (!dirs.isEmpty()) {
                QString dir = dirs.first();
                if (ProjectManager::instance().openProject(dir)) {
                    loadProject(dir);
                } else {
                    QMessageBox::warning(this, "Error", MSG_ERR_NOT_PROJECT);
                }
            }
        }
    }

    void MainWindow::onRefreshIndexTriggered() {
        QString currentPath = m_editor->currentFilePath();
        if (currentPath.isEmpty()) return;

        ProjectManager::instance().processFile(currentPath);
        m_terminal->appendOutput("Queued for indexing: " + currentPath + "\n", false);
    }
} // namespace gs