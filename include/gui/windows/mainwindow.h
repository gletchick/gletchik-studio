#pragma once

#include "basewindow.h"
#include <QSplitter>
#include <QTextEdit>

#include "gui/fileexplorerwidget.h"

namespace Ui {
    class MainWindow;
}

namespace gs {

    class MainWindow : public BaseWindow {
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow() override;

        void loadProject(const QString &path);

    protected:
        void setupWorkspace(QWidget *contentWidget) override;

    private slots:
        void onProjectToggled(bool checked);
        void onTerminalToggled(bool checked);

    private:
        Ui::MainWindow *ui;

        FileExplorerWidget *m_fileExplorer;
        QTextEdit *m_terminal;
        QSplitter *m_hSplitter;
        QSplitter *m_vSplitter;

        void onOpenProjectTriggered();
    };

} // namespace gs