#include "gui/sidebar/sidebar.h"

namespace gs {

    // sidebar.cpp
    Sidebar::Sidebar(QWidget *parent) : QWidget(parent) {
        setFixedWidth(38); // Уменьшаем ширину сайдбара
        setObjectName("leftSidebar");

        auto *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 5, 0, 8); // Отступ снизу
        layout->setSpacing(2);

        auto *btnProject = createButton(":/icons/project.svg", "Project Explorer");
        auto *btnTerminal = createButton(":/icons/terminal.svg", "Terminal");
        auto *btnRun = createButton(":/icons/run.svg", "Run Application");

        btnProject->setChecked(true);
        // 1. Кнопку папки оставляем в самом верху
        layout->addWidget(btnProject);

        // 2. Добавляем "пружину", которая вытолкнет остальные кнопки вниз
        layout->addStretch();

        // 3. Кнопка Пуск (будет выше терминала)
        layout->addWidget(btnRun);

        // 4. Кнопка Терминал (в самом низу)
        layout->addWidget(btnTerminal);

        connect(btnProject, &QToolButton::toggled, this, &Sidebar::projectToggled);
        connect(btnTerminal, &QToolButton::toggled, this, &Sidebar::terminalToggled);
        connect(btnRun, &QToolButton::clicked, this, &Sidebar::runClicked);
    }

    QToolButton* Sidebar::createButton(const QString& iconPath, const QString& tooltip) {
        auto *btn = new QToolButton(this);
        btn->setIcon(QIcon(iconPath));
        btn->setIconSize(QSize(18, 18)); // Иконки чуть меньше
        btn->setToolTip(tooltip);
        btn->setFixedSize(38, 38); // Кнопки теперь компактнее
        btn->setCursor(Qt::PointingHandCursor);
        btn->setFocusPolicy(Qt::NoFocus);
        btn->setCheckable(true);
        return btn;
    }

} // namespace gs