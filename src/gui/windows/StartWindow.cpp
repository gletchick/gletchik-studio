#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIcon>

#include "gui/windows/startwindow.h"

namespace gs {

StartWindow::StartWindow(QWidget *parent)
    : BaseWindow(parent) {
    setObjectName("StartWindow");
    resize(850, 550);

    setupBaseWindow("Gletchick Studio IDE");
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

void StartWindow::createActionButtons(QWidget *container, QLayout *layout) {
    layout->addWidget(createLargeButton(":/icons/create-project.svg", "New Project", container));
    layout->addWidget(createLargeButton(":/icons/project.svg", "Open Project", container));
    layout->addWidget(createLargeButton(":/icons/project-from-git.svg", "Get from VCS", container));
}

    QWidget* gs::StartWindow::createLargeButton(const QString &iconPath, const QString &text, QWidget *parent) {
    // Основной контейнер для вертикального стека (Кнопка + Текст)
    QWidget *tileContainer = new QWidget(parent);
    QVBoxLayout *tileLayout = new QVBoxLayout(tileContainer);
    tileLayout->setContentsMargins(0, 0, 0, 0);
    tileLayout->setSpacing(12);
    tileLayout->setAlignment(Qt::AlignCenter);

    // Сама кнопка
    QPushButton *btn = new QPushButton(tileContainer);
    btn->setObjectName("actionTile");
    btn->setFixedSize(63, 63);
    btn->setIcon(QIcon(iconPath));
    btn->setIconSize(QSize(33, 33));
    btn->setCursor(Qt::PointingHandCursor);

    // Подпись под кнопкой
    QLabel *textLabel = new QLabel(text, tileContainer);
    textLabel->setObjectName("tileLabel");
    textLabel->setAlignment(Qt::AlignCenter);
    textLabel->setWordWrap(true);
    // Ограничиваем ширину текста шириной кнопки, чтобы layout не раздувало
    textLabel->setFixedWidth(100);

    tileLayout->addWidget(btn, 0, Qt::AlignCenter);
    tileLayout->addWidget(textLabel, 0, Qt::AlignCenter);

    return tileContainer;
}

} // namespace gs