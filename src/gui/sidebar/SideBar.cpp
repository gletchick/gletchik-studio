#include "gui/sidebar/sidebar.h"

namespace gs {

    Sidebar::Sidebar(QWidget *parent) : QWidget(parent) {
        setFixedWidth(50);
        setObjectName("leftSidebar");

        auto *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        // Создаем кнопки
        auto *btnProject = createButton(":/icons/project.svg", "Project Explorer");
        auto *btnTerminal = createButton(":/icons/terminal.svg", "Terminal");
        auto *btnRun = createButton(":/icons/run.svg", "Run Application");

        btnRun->setCheckable(false);

        // Включаем панели по умолчанию при запуске
        btnProject->setChecked(true);
        btnTerminal->setChecked(true);

        layout->addWidget(btnProject);
        layout->addWidget(btnTerminal);
        layout->addWidget(btnRun);
        layout->addStretch();

        // Подключаем переключатели к сигналам сайдбара
        connect(btnProject, &QToolButton::toggled, this, &Sidebar::projectToggled);
        connect(btnTerminal, &QToolButton::toggled, this, &Sidebar::terminalToggled);
        connect(btnRun, &QToolButton::clicked, this, &Sidebar::runClicked);
    }

    QToolButton* Sidebar::createButton(const QString& iconPath, const QString& tooltip) {
        auto *btn = new QToolButton(this);
        btn->setIcon(QIcon(iconPath));
        btn->setIconSize(QSize(24, 24));
        btn->setToolTip(tooltip);
        btn->setFixedSize(50, 50); // Делаем кнопку по размеру сайдбара
        btn->setCursor(Qt::PointingHandCursor);
        btn->setFocusPolicy(Qt::NoFocus);

        btn->setCheckable(true);
        // ВАЖНО: Убираем setAutoExclusive(true), чтобы панели открывались независимо друг от друга!

        return btn;
    }

} // namespace gs