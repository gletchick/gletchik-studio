#include "gui/windows/startwindow.h"
#include <qcoreapplication.h>
#include <QLabel>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QProcess>
#include <QProgressDialog>
#include <QStandardPaths>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include "core/execution/pluginmanager.h"
#include "core/languages/java/javalanguageprovider.h"
#include "core/processes/nativeprocess.h"
#include "gui/windows/mainwindow.h"
#include "core/project/projectmanager.h"

namespace gs {

    namespace {
        const QSize kWindowSize = QSize(850, 550);
        const QSize kLargeButtonSize = QSize(63, 63);
        const QSize kLargeIconSize = QSize(33, 33);
        const QSize kSettingsIconSize = QSize(24, 24);
        const QSize kSettingsButtonSize = QSize(40, 40);

        const int kBottomBarHeight = 60;
        const int kTextLabelWidth = 100;
        const int kSpacingLarge = 50;
        const int kSpacingMedium = 30;
        const int kSpacingSmall = 12;
        const int kGitStartTimeoutMs = 3000;

        const QString kWindowTitle = "Gletchick Studio IDE";
        const QString kRepoHint = "https://github.com/gletchick/theme";
        const QString kDefaultGitPath = "/usr/bin/git";

        const QString kIconNewProject = ":/icons/create-project.svg";
        const QString kIconOpenProject = ":/icons/open-project.svg";
        const QString kIconVcs = ":/icons/project-from-git.svg";
        const QString kIconSettings = ":/icons/settings.svg";
    }

    StartWindow::StartWindow(QWidget *parent)
        : BaseWindow(parent) {
        setObjectName("StartWindow");
        resize(kWindowSize);
        setupBaseWindow(kWindowTitle);
        initializePlugins();
    }

    void StartWindow::handleNewProject() {
        QString dir = selectProjectDirectory("Select Directory for New Project");
        if (!dir.isEmpty()) {
            initializeProjectSession(dir, true);
        }
    }

    void StartWindow::handleOpenProject() {
        QString dir = selectProjectDirectory("Open Project");
        if (!dir.isEmpty()) {
            initializeProjectSession(dir, false);
        }
    }

    void StartWindow::initializeProjectSession(const QString &path, bool isNew) {
        bool success = false;

        if (isNew) {
            success = ProjectManager::instance().createProject(path);
        } else {
            success = ProjectManager::instance().openProject(path);
        }

        if (success) {
            MainWindow *mainWin = new MainWindow();
            mainWin->loadProject(path);
            mainWin->show();
            this->close();
        } else {
            QMessageBox::warning(this, "Project Error", "Failed to " + QString(isNew ? "create" : "open") + " project at: " + path);
        }
    }

    void StartWindow::executeClone(const QString &url, const QString &path) {
        QString gitPath = QStandardPaths::findExecutable("git");
        if (gitPath.isEmpty()) {
            gitPath = kDefaultGitPath;
        }

        QProcess *git = new QProcess(this);
        QProgressDialog *progress = new QProgressDialog("Cloning...", "Cancel", 0, 0, this);
        progress->setWindowModality(Qt::WindowModal);

        setupGitEnvironment(git);

        connect(git, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, [this, path, git, progress](int exitCode, QProcess::ExitStatus exitStatus) {
            progress->accept();
            if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
                initializeProjectSession(path, false);
            } else {
                QString err = git->readAllStandardError();
                QMessageBox::critical(this, "Error", "Clone failed: " + (err.isEmpty() ? "Unknown error" : err));
            }
            git->deleteLater();
        });

        git->start(gitPath, {"clone", url, path});

        if (!git->waitForStarted(kGitStartTimeoutMs)) {
            QMessageBox::critical(this, "Fatal Error", QString("Could not start git: %1").arg(git->errorString()));
            git->deleteLater();
            progress->deleteLater();
        } else {
            progress->exec();
        }
    }

    void StartWindow::setupWorkspace(QWidget *contentWidget) {
        contentWidget->setObjectName("startWorkspace");
        QVBoxLayout *mainLayout = new QVBoxLayout(contentWidget);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);

        mainLayout->addWidget(createCenterContainer(contentWidget), 1);
        mainLayout->addWidget(createBottomBar(contentWidget));
    }

    QWidget* StartWindow::createCenterContainer(QWidget *parent) {
        QWidget *container = new QWidget(parent);
        QVBoxLayout *layout = new QVBoxLayout(container);
        layout->setContentsMargins(50, 20, 50, 0);

        layout->addStretch(2);
        setupWelcomeLabels(container, layout);
        layout->addSpacing(kSpacingLarge);
        setupActionsLayout(container, layout);
        layout->addStretch(3);

        return container;
    }

    void StartWindow::setupWelcomeLabels(QWidget *container, QVBoxLayout *layout) {
        QLabel *titleLabel = new QLabel("Welcome to Gletchick Studio", container);
        titleLabel->setObjectName("welcomeTitle");
        titleLabel->setAlignment(Qt::AlignCenter);

        QLabel *descLabel = new QLabel("The next generation of coding experience.", container);
        descLabel->setObjectName("welcomeDesc");
        descLabel->setAlignment(Qt::AlignCenter);

        layout->addWidget(titleLabel);
        layout->addWidget(descLabel);
    }

    void StartWindow::setupActionsLayout(QWidget *container, QVBoxLayout *layout) {
        QHBoxLayout *actionsLayout = new QHBoxLayout();
        actionsLayout->setSpacing(kSpacingMedium);
        actionsLayout->addStretch(1);
        createActionButtons(container, actionsLayout);
        actionsLayout->addStretch(1);
        layout->addLayout(actionsLayout);
    }

    QWidget* StartWindow::createBottomBar(QWidget *parent) {
        QWidget *bottomBar = new QWidget(parent);
        bottomBar->setFixedHeight(kBottomBarHeight);
        QHBoxLayout *layout = new QHBoxLayout(bottomBar);
        layout->setContentsMargins(20, 0, 20, 15);

        QPushButton *btnSettings = new QPushButton(bottomBar);
        btnSettings->setIcon(QIcon(kIconSettings));
        btnSettings->setIconSize(kSettingsIconSize);
        btnSettings->setFixedSize(kSettingsButtonSize);
        btnSettings->setObjectName("btnSettings");
        btnSettings->setCursor(Qt::PointingHandCursor);

        layout->addWidget(btnSettings);
        layout->addStretch(1);
        return bottomBar;
    }

    void StartWindow::createActionButtons(QWidget *container, QLayout *layout) {
        layout->addWidget(createLargeButton(kIconNewProject, "New Project", container, [this]() {
            handleNewProject();
        }));

        layout->addWidget(createLargeButton(kIconOpenProject, "Open Project", container, [this]() {
            handleOpenProject();
        }));

        layout->addWidget(createLargeButton(kIconVcs, "Get from VCS", container, [this]() {
            handleVcsClone();
        }));
    }

    QString StartWindow::selectProjectDirectory(const QString &title) {
        QFileDialog dialog(this);
        dialog.setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
        dialog.setWindowTitle(title);
        dialog.setFileMode(QFileDialog::Directory);
        dialog.setOption(QFileDialog::ShowDirsOnly, true);
        dialog.setOption(QFileDialog::DontUseNativeDialog, true);

        if (dialog.exec()) {
            return dialog.selectedFiles().first();
        }
        return QString();
    }

    void StartWindow::handleVcsClone() {
        bool ok;
        QString repoUrl = QInputDialog::getText(this, "Clone", "Repository URL:", QLineEdit::Normal, kRepoHint, &ok);

        if (ok && !repoUrl.isEmpty()) {
            QString dir = QFileDialog::getExistingDirectory(this, "Select Directory");
            if (!dir.isEmpty()) {
                executeClone(repoUrl, dir);
            }
        }
    }

    void StartWindow::setupGitEnvironment(QProcess *process) {
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert("GIT_TERMINAL_PROMPT", "0");
        env.insert("GIT_SSH_COMMAND", "ssh -o StrictHostKeyChecking=no");
        process->setProcessEnvironment(env);
    }

    QWidget* StartWindow::createLargeButton(const QString &iconPath, const QString &text, QWidget *parent, std::function<void()> onClick) {
        QWidget *tileContainer = new QWidget(parent);
        QVBoxLayout *tileLayout = new QVBoxLayout(tileContainer);
        tileLayout->setContentsMargins(0, 0, 0, 0);
        tileLayout->setSpacing(kSpacingSmall);
        tileLayout->setAlignment(Qt::AlignCenter);

        QPushButton *btn = new QPushButton(tileContainer);
        btn->setObjectName("actionTile");
        btn->setFixedSize(kLargeButtonSize);
        btn->setIcon(QIcon(iconPath));
        btn->setIconSize(kLargeIconSize);
        btn->setCursor(Qt::PointingHandCursor);

        if (onClick) {
            connect(btn, &QPushButton::clicked, onClick);
        }

        QLabel *textLabel = new QLabel(text, tileContainer);
        textLabel->setObjectName("tileLabel");
        textLabel->setAlignment(Qt::AlignCenter);
        textLabel->setWordWrap(true);
        textLabel->setFixedWidth(kTextLabelWidth);

        tileLayout->addWidget(btn, 0, Qt::AlignCenter);
        tileLayout->addWidget(textLabel, 0, Qt::AlignCenter);

        return tileContainer;
    }

    void StartWindow::initializePlugins() {
        QString pluginsPath = QCoreApplication::applicationDirPath() + "/plugins";
        auto defaultProcess = std::make_shared<NativeProcess>();
        auto javaProvider = std::make_shared<JavaLanguageProvider>(defaultProcess);

        PluginManager::registerBuiltInProvider(javaProvider);
        PluginManager::loadAllFromDir(pluginsPath.toStdString(), defaultProcess);
    }

} // namespace gs