#pragma once

#include <QWidget>
#include <QMouseEvent>
#include <QPushButton>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QSplitter>
#include <QTextEdit>

#include "core/utils/threadsafequeue.h"
#include "gui/terminalwidget.h"

namespace Ui {
    class MainWindow;
}

namespace gs {
    class JavaBuildController;

    class MainWindow : public QWidget {
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

    protected:
        void mousePressEvent(QMouseEvent *event) override;
        void mouseMoveEvent(QMouseEvent *event) override;
        void mouseReleaseEvent(QMouseEvent *event) override;

    private slots:
        void onBtnCloseClicked();
        void onBtnMaximizedClicked();
        void onBtnMinimizedClicked();

        void onProjectToggled(bool checked);
        void onTerminalToggled(bool checked);

        void onRunClicked();
        void onTerminalInput(const QString& text);
        void processOutputQueue();

    private:
        Ui::MainWindow *ui;

        enum ResizeDir { None, Top, Bottom, Left, Right, TopLeft, TopRight, BottomLeft, BottomRight };
        ResizeDir m_resizeDir = None;
        const int m_boundary = 8;

        QPushButton *m_btnMaximize;


        ResizeDir getResizeDir(const QPoint &pos);
        void updateCursorShape(const QPoint &pos);
        void setupTitleBar();
        void setupCentralArea();

        QWidget *m_fileExplorer;
        TerminalWidget *m_terminal;
        QSplitter *m_hSplitter;
        QSplitter *m_vSplitter;

        ThreadSafeQueue m_outputQueue;
        QTimer *m_updateTimer;
        std::shared_ptr<JavaBuildController> m_controller;
    };

} // namespace gs