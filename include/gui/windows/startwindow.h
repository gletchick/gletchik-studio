#pragma once

#include "gui/windows/basewindow.h"
#include <QString>
#include <functional>
#include  <QProcess>

namespace gs {

    class StartWindow : public BaseWindow {
        Q_OBJECT

    public:
        explicit StartWindow(QWidget *parent = nullptr);

    protected:
        void setupWorkspace(QWidget *contentWidget) override;

    private:
        void initializePlugins();
        void createActionButtons(QWidget *container, QLayout *layout);
        QWidget* createLargeButton(const QString &iconPath, const QString &text, QWidget *parent, std::function<void()> onClick);

        void handleNewProject();
        void handleOpenProject();
        void handleVcsClone();

        void executeClone(const QString &url, const QString &path);
        void setupGitEnvironment(QProcess *process);

        // Общая логика инициализации сессии проекта
        void initializeProjectSession(const QString &path, bool isNew = false);

        QString selectProjectDirectory(const QString &title);
        QWidget* createCenterContainer(QWidget *parent);

        void setupWelcomeLabels(QWidget *container, QVBoxLayout *layout);
        void setupActionsLayout(QWidget *container, QVBoxLayout *layout);
        QWidget* createBottomBar(QWidget *parent);
    };

} // namespace gs