#include <QLabel>

#include "gui/windows/startwindow.h"

namespace gs {

StartWindow::StartWindow(QWidget *parent)
    : BaseWindow(parent) {
    setObjectName("StartWindow");
    resize(850, 550);

    setupBaseWindow("Welcome to IntelliJ IDEA");
}

void StartWindow::setupWorkspace(QWidget *contentWidget) {
    QVBoxLayout *mainLayout = new QVBoxLayout(contentWidget);
    mainLayout->setContentsMargins(0, 0, 0, 20);
    mainLayout->setSpacing(0);

    // Центральная область с приветствием и кнопками
    QWidget *centerContainer = new QWidget(contentWidget);
    QVBoxLayout *centerLayout = new QVBoxLayout(centerContainer);
    centerLayout->setContentsMargins(50, 0, 50, 0);
    centerLayout->addStretch(1);

    QLabel *titleLabel = new QLabel("Welcome to IntelliJ IDEA", centerContainer);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 32px; font-weight: 500; margin-bottom: 10px;");
    centerLayout->addWidget(titleLabel);

    QLabel *descLabel = new QLabel("Create a new project to start from scratch.\nOpen existing project from disk or version control.", centerContainer);
    descLabel->setAlignment(Qt::AlignCenter);
    descLabel->setStyleSheet("color: #808080; font-size: 14px; margin-bottom: 40px;");
    centerLayout->addWidget(descLabel);

    QHBoxLayout *actionsLayout = new QHBoxLayout();
    actionsLayout->setSpacing(20);
    actionsLayout->addStretch(1);
    createActionButtons(centerContainer, actionsLayout);
    actionsLayout->addStretch(1);
    
    centerLayout->addLayout(actionsLayout);
    centerLayout->addStretch(2);

    mainLayout->addWidget(centerContainer, 1);

    // Нижняя панель с перенесенной кнопкой настроек
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->setContentsMargins(20, 0, 20, 0);

    QPushButton *btnSettings = new QPushButton("⚙", contentWidget);
    btnSettings->setFixedSize(32, 32);
    btnSettings->setObjectName("btnSettings");
    btnSettings->setCursor(Qt::PointingHandCursor);

    // Смещаем кнопку согласно стрелке (например, в центр или чуть правее центра)
    bottomLayout->addStretch(1); 
    bottomLayout->addWidget(btnSettings);
    bottomLayout->addStretch(1);

    mainLayout->addLayout(bottomLayout);
}

void StartWindow::createActionButtons(QWidget *container, QLayout *layout) {
    QPushButton *btnNew = createLargeButton("+", "New Project", container);
    QPushButton *btnOpen = createLargeButton("📁", "Open", container);
    QPushButton *btnVCS = createLargeButton("🌿", "Clone Repository", container);

    layout->addWidget(btnNew);
    layout->addWidget(btnOpen);
    layout->addWidget(btnVCS);
}

QPushButton* StartWindow::createLargeButton(const QString &icon, const QString &text, QWidget *parent) {
    QPushButton *btn = new QPushButton(parent);
    btn->setObjectName("actionTile");
    btn->setFixedSize(140, 120);

    btn->setCursor(Qt::PointingHandCursor);
    
    QVBoxLayout *btnLayout = new QVBoxLayout(btn);
    
    QLabel *iconLabel = new QLabel(icon, btn);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet("font-size: 24px; border: none; background: transparent;");
    
    QLabel *textLabel = new QLabel(text, btn);
    textLabel->setAlignment(Qt::AlignCenter);
    textLabel->setWordWrap(true);
    textLabel->setStyleSheet("font-size: 12px; border: none; background: transparent;");

    btnLayout->addWidget(iconLabel);
    btnLayout->addWidget(textLabel);
    
    return btn;
}

} // namespace gs