#include <QWindow>
#include <QLabel>
#include <QHBoxLayout>

#include "gui/windows/basewindow.h"

namespace gs {

BaseWindow::BaseWindow(QWidget *parent) : QWidget(parent) {
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setMouseTracking(true);
}

void BaseWindow::setupBaseWindow(const QString &title) {
    QVBoxLayout *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    initTitleBar(title, rootLayout);

    m_contentWidget = new QWidget(this);
    rootLayout->addWidget(m_contentWidget, 1);

    setupWorkspace(m_contentWidget);
}

void BaseWindow::initTitleBar(const QString &title, QVBoxLayout *mainLayout) {
    QWidget *titleBarWidget = new QWidget(this);
    titleBarWidget->setObjectName("titleBarWidget");
    titleBarWidget->setFixedHeight(35);

    QHBoxLayout *titleLayout = new QHBoxLayout(titleBarWidget);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(0);

    QLabel *titleLabel = new QLabel(title, titleBarWidget);
    titleLabel->setObjectName("titleLabel");

    QPushButton *btnMinimize = new QPushButton("_", titleBarWidget);
    m_btnMaximize = new QPushButton("▢", titleBarWidget);
    QPushButton *btnClose = new QPushButton("✕", titleBarWidget);

    btnMinimize->setObjectName("btnMinimize");
    m_btnMaximize->setObjectName("btnMaximize");
    btnClose->setObjectName("btnClose");

    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();
    titleLayout->addWidget(btnMinimize);
    titleLayout->addWidget(m_btnMaximize);
    titleLayout->addWidget(btnClose);

    mainLayout->addWidget(titleBarWidget);

    connect(btnClose, &QPushButton::clicked, this, &BaseWindow::onBtnCloseClicked);
    connect(m_btnMaximize, &QPushButton::clicked, this, &BaseWindow::onBtnMaximizedClicked);
    connect(btnMinimize, &QPushButton::clicked, this, &BaseWindow::onBtnMinimizedClicked);
}

void BaseWindow::onBtnCloseClicked() {
    close();
}

void BaseWindow::onBtnMinimizedClicked() {
    showMinimized();
}

void BaseWindow::onBtnMaximizedClicked() {
    if (isMaximized()) {
        showNormal();
        m_btnMaximize->setText("▢");
    } else {
        showMaximized();
        m_btnMaximize->setText("❐");
    }
}

void BaseWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_resizeDir = getResizeDir(event->pos());
        if (m_resizeDir == None) {
            QWidget *child = childAt(event->pos());
            if (child && (child->objectName() == "titleBarWidget" || child->objectName() == "titleLabel" || child->inherits("QLabel"))) {
                windowHandle()->startSystemMove();
            }
        }
    }
}

void BaseWindow::mouseMoveEvent(QMouseEvent *event) {
    if (m_resizeDir != None) {
        QPoint p = event->globalPos();
        QRect g = geometry();
        int minSize = 300;
        switch (m_resizeDir) {
            case Right: g.setRight(p.x()); break;
            case Left: if (p.x() < g.right() - minSize) g.setLeft(p.x()); break;
            case Bottom: g.setBottom(p.y()); break;
            case Top: if (p.y() < g.bottom() - minSize) g.setTop(p.y()); break;
            case BottomRight: g.setRight(p.x()); g.setBottom(p.y()); break;
            default: break;
        }
        setGeometry(g);
    } else {
        updateCursorShape(event->pos());
    }
}

void BaseWindow::mouseReleaseEvent(QMouseEvent *event) {
    m_resizeDir = None;
    event->accept();
}

BaseWindow::ResizeDir BaseWindow::getResizeDir(const QPoint &pos) {
    int x = pos.x();
    int y = pos.y();
    int w = width();
    int h = height();
    bool left = (x <= m_boundary);
    bool right = (x >= w - m_boundary);
    bool top = (y <= m_boundary);
    bool bottom = (y >= h - m_boundary);

    if (top && left) return TopLeft;
    if (top && right) return TopRight;
    if (bottom && left) return BottomLeft;
    if (bottom && right) return BottomRight;
    if (top) return Top;
    if (bottom) return Bottom;
    if (left) return Left;
    if (right) return Right;

    return None;
}

void BaseWindow::updateCursorShape(const QPoint &pos) {
    ResizeDir dir = getResizeDir(pos);
    switch (dir) {
        case Top: case Bottom: setCursor(Qt::SizeVerCursor); break;
        case Left: case Right: setCursor(Qt::SizeHorCursor); break;
        case TopLeft: case BottomRight: setCursor(Qt::SizeFDiagCursor); break;
        default: setCursor(Qt::ArrowCursor); break;
    }
}

} // namespace gs