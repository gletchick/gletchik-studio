#pragma once

#include <QWidget>
#include <QMouseEvent>
#include <QPushButton>
#include <QVBoxLayout>

namespace gs {

    class BaseWindow : public QWidget {
        Q_OBJECT

    public:
        explicit BaseWindow(QWidget *parent = nullptr);
        ~BaseWindow() override = default;

    protected:
        void setupBaseWindow(const QString &title);
        virtual void setupWorkspace(QWidget *contentWidget) = 0;

        void mousePressEvent(QMouseEvent *event) override;
        void mouseMoveEvent(QMouseEvent *event) override;
        void mouseReleaseEvent(QMouseEvent *event) override;

    private slots:
        void onBtnCloseClicked();
        void onBtnMaximizedClicked();
        void onBtnMinimizedClicked();

    private:
        enum ResizeDir { None, Top, Bottom, Left, Right, TopLeft, TopRight, BottomLeft, BottomRight };
        ResizeDir m_resizeDir = None;
        const int m_boundary = 8;

        QPushButton *m_btnMaximize;
        QWidget *m_contentWidget;

        void initTitleBar(const QString &title, QVBoxLayout *mainLayout);
        ResizeDir getResizeDir(const QPoint &pos);
        void updateCursorShape(const QPoint &pos);
    };

} // namespace gs