#include <qcoreapplication.h>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QProcess>


#include "core/execution/pluginmanager.h"
#include "core/languages/java/javalanguageprovider.h"
#include "core/processes/nativeprocess.h"
#include "gui/windows/mainwindow.h"

#include "core/project/projectmanager.h"
#include "gui/windows/startwindow.h"

namespace gs {

    StartWindow::StartWindow(QWidget *parent)
        : BaseWindow(parent) {
        setObjectName("StartWindow");
        resize(850, 550);

        setupBaseWindow("Gletchick Studio IDE");
        initializePlugins();
    }

    void StartWindow::setupWorkspace(QWidget *contentWidget) {
        contentWidget->setObjectName("startWorkspace");
        QVBoxLayout *mainLayout = new QVBoxLayout(contentWidget);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);

        // Центрирующий контейнер
        QWidget *centerContainer = new QWidget(contentWidget);
        QVBoxLayout *centerLayout = new QVBoxLayout(centerContainer);
        centerLayout->setContentsMargins(50, 20, 50, 0);
        centerLayout->addStretch(2);

        QLabel *titleLabel = new QLabel("Welcome to Gletchick Studio", centerContainer);
        titleLabel->setObjectName("welcomeTitle");
        titleLabel->setAlignment(Qt::AlignCenter);
        centerLayout->addWidget(titleLabel);

        QLabel *descLabel = new QLabel("The next generation of coding experience.", centerContainer);
        descLabel->setObjectName("welcomeDesc");
        descLabel->setAlignment(Qt::AlignCenter);
        centerLayout->addWidget(descLabel);

        centerLayout->addSpacing(50);

        // Ряд с кнопками
        QHBoxLayout *actionsLayout = new QHBoxLayout();
        actionsLayout->setSpacing(30);
        actionsLayout->addStretch(1);
        createActionButtons(centerContainer, actionsLayout);
        actionsLayout->addStretch(1);

        centerLayout->addLayout(actionsLayout);
        centerLayout->addStretch(3);

        mainLayout->addWidget(centerContainer, 1);

        // Нижняя панель с шестеренкой слева
        QWidget *bottomBar = new QWidget(contentWidget);
        bottomBar->setFixedHeight(60);
        QHBoxLayout *bottomLayout = new QHBoxLayout(bottomBar);
        bottomLayout->setContentsMargins(20, 0, 20, 15);

        QPushButton *btnSettings = new QPushButton(bottomBar);
        btnSettings->setIcon(QIcon(":/icons/settings.svg"));
        btnSettings->setIconSize(QSize(24, 24));
        btnSettings->setFixedSize(40, 40);
        btnSettings->setObjectName("btnSettings");
        btnSettings->setCursor(Qt::PointingHandCursor);

        bottomLayout->addWidget(btnSettings); // Добавляем первым (слева)
        bottomLayout->addStretch(1);

        mainLayout->addWidget(bottomBar);
    }

void StartWindow::createActionButtons(QWidget *container, QLayout *layout)
    {
        // 1. NEW PROJECT
        layout->addWidget(createLargeButton(":/icons/create-project.svg", "New Project", container, [this]() {
            QFileDialog dialog(this);
            dialog.setWindowFlags(Qt::Window | Qt::FramelessWindowHint); // Убираем белую рамку
            dialog.setWindowTitle("Select Directory for New Project");
            dialog.setFileMode(QFileDialog::Directory);
            dialog.setOption(QFileDialog::ShowDirsOnly, true);
            dialog.setOption(QFileDialog::DontUseNativeDialog, true); // Включаем наш QSS

            if (dialog.exec()) {
                QString dir = dialog.selectedFiles().first();
                if (ProjectManager::instance().createProject(dir)) {
                    MainWindow *mainWin = new MainWindow();
                    mainWin->loadProject(dir);
                    mainWin->show();
                    this->close();
                }
            }
        }));

        // 2. OPEN PROJECT
        layout->addWidget(createLargeButton(":/icons/open-project.svg", "Open Project", container, [this]() {
            QFileDialog dialog(this);
            dialog.setWindowFlags(Qt::Window | Qt::FramelessWindowHint); // Убираем белую рамку
            dialog.setWindowTitle("Open Project");
            dialog.setFileMode(QFileDialog::Directory);
            dialog.setOption(QFileDialog::ShowDirsOnly, true);
            dialog.setOption(QFileDialog::DontUseNativeDialog, true); // Включаем наш QSS

            if (dialog.exec()) {
                QString dir = dialog.selectedFiles().first();
                if (ProjectManager::instance().openProject(dir)) {
                    MainWindow *mainWin = new MainWindow();
                    mainWin->loadProject(dir);
                    mainWin->show();
                    this->close();
                }
            }
        }));

        // 3. GET FROM VCS
        layout->addWidget(createLargeButton(":/icons/project-from-git.svg", "Get from VCS", container, [this]() {
            bool ok;
            QString repoUrl = QInputDialog::getText(this, "Clone", "Repository URL:", QLineEdit::Normal, "https://github.com/", &ok);

            if (ok && !repoUrl.isEmpty()) {
                QString dir = QFileDialog::getExistingDirectory(this, "Select Directory");
                if (!dir.isEmpty()) {
                    QProcess *git = new QProcess(this);

                    QStringList args;
                    args << "clone" << repoUrl << dir;

                    connect(git, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                            this, [this, dir, git](int exitCode, QProcess::ExitStatus exitStatus) {

                        if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
                            MainWindow *mainWin = new MainWindow();
                            mainWin->loadProject(dir);
                            mainWin->show();
                            this->close();
                        } else {
                            QString errorLog = git->readAllStandardError();
                            QMessageBox::critical(this, "Error", "Git clone failed!\n" + errorLog);
                        }

                        git->deleteLater();
                    });

                    git->start("git", args);
                }
            }
        }));
    }

    QWidget* StartWindow::createLargeButton(const QString &iconPath, const QString &text, QWidget *parent, std::function<void()> onClick) {
        QWidget *tileContainer = new QWidget(parent);
        QVBoxLayout *tileLayout = new QVBoxLayout(tileContainer);
        tileLayout->setContentsMargins(0, 0, 0, 0);
        tileLayout->setSpacing(12);
        tileLayout->setAlignment(Qt::AlignCenter);

        QPushButton *btn = new QPushButton(tileContainer);
        btn->setObjectName("actionTile");
        btn->setFixedSize(63, 63);
        btn->setIcon(QIcon(iconPath));
        btn->setIconSize(QSize(33, 33));
        btn->setCursor(Qt::PointingHandCursor);

        // Соединяем сигнал нажатия с переданным действием
        if (onClick) {
            connect(btn, &QPushButton::clicked, onClick);
        }

        QLabel *textLabel = new QLabel(text, tileContainer);
        textLabel->setObjectName("tileLabel");
        textLabel->setAlignment(Qt::AlignCenter);
        textLabel->setWordWrap(true);
        textLabel->setFixedWidth(100);

        tileLayout->addWidget(btn, 0, Qt::AlignCenter);
        tileLayout->addWidget(textLabel, 0, Qt::AlignCenter);

        return tileContainer;
    }

    void StartWindow::initializePlugins() {
        qDebug() << "[Plugins] Starting initialization...";
        QString pluginsPath = QCoreApplication::applicationDirPath() + "/plugins";
        qDebug() << "[Plugins] Searching in:" << pluginsPath;
        // Создаем базовый процесс, если он требуется провайдерам для работы (например, для запуска компилятора)
        auto defaultProcess = std::make_shared<NativeProcess>();
        auto javaProvider = std::make_shared<JavaLanguageProvider>(defaultProcess);
        PluginManager::registerBuiltInProvider(javaProvider);
        // Загружаем все доступные языковые провайдеры (Java и т.д.)
        PluginManager::loadAllFromDir(pluginsPath.toStdString(), defaultProcess);
        qDebug() << "[Plugins] Initialization finished.";
    }

    void StartWindow::cloneAndOpen(const QString &url, const QString &path) {
        QProcess *gitProcess = new QProcess(this);

        qDebug() << "Cloning" << url << "to" << path;

        connect(gitProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, [this, path, gitProcess](int exitCode, QProcess::ExitStatus exitStatus) {

            if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
                if (ProjectManager::instance().openProject(path)) {
                    MainWindow *mainWin = new MainWindow();
                    mainWin->loadProject(path);
                    mainWin->show();
                    this->close();
                }
            } else {
                QString errorLog = gitProcess->readAllStandardError();
                QMessageBox::critical(this, "Git Error", "Failed to clone repository!\n" + errorLog);
            }
            gitProcess->deleteLater();
        });

        gitProcess->start("git", QStringList() << "clone" << url << path);
    }

} // namespace gs