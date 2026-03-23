#pragma once

#include "basewindow.h"
#include <QSplitter>
#include <QTextEdit>

#include "core/execution/ibuildcontroller.h"
#include "core/utils/threadsafequeue.h"
#include "gui/fileexplorerwidget.h"
#include "gui/fileview.h"
#include "gui/terminalwidget.h"
#include "sdk/ilanguageprovider.h"

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
        void onOpenProjectTriggered();
        void onRunClicked();
        void processOutputQueue();
        void onRefreshIndexTriggered();

    private:
        Ui::MainWindow *ui;

        FileExplorerWidget *m_fileExplorer;
        FileView *m_editor;
        TerminalWidget *m_terminal;
        QSplitter *m_hSplitter;
        QSplitter *m_vSplitter;

        std::shared_ptr<IBuildController> m_buildController;
        std::shared_ptr<ILanguageProvider> m_javaProvider;

        ThreadSafeQueue m_outputQueue;
        QTimer *m_updateTimer;

        void onTerminalInput(const QString& text);
    };

} // namespace gs