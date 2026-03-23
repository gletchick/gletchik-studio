#include "gui/sidebar/sidebar.h"
#include <QVBoxLayout>
#include <QToolButton>

namespace gs {

    Sidebar::Sidebar(QWidget *parent) : QWidget(parent) {
        setFixedWidth(38);
        setObjectName("leftSidebar");

        auto *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 5, 0, 8);
        layout->setSpacing(2);

        // Создаем кнопки
        auto *btnProject = createButton(":/icons/project.svg", "Project Explorer");
        auto *btnRun = createButton(":/icons/run.svg", "Run Application");
        auto *btnTerminal = createButton(":/icons/terminal.svg", "Terminal Control");

        // НОВАЯ КНОПКА ВЫХОДА
        auto *btnExit = createButton(":/icons/exit.svg", "Close Project");
        btnExit->setCheckable(false); // Она не должна залипать, просто нажал-вышел
        btnExit->setObjectName("btnExitSidebar"); // Для отдельного стиля в QSS

        // 1. Проводник в самом верху
        btnProject->setChecked(true);
        layout->addWidget(btnProject);

        // 2. Пружина (отталкивает всё остальное вниз)
        layout->addStretch();

        // 3. Кнопки инструментов внизу
        layout->addWidget(btnRun);
        layout->addWidget(btnTerminal);

        // 4. Кнопка выхода в самом-самом низу
        layout->addWidget(btnExit);

        // Коннекты
        connect(btnProject, &QToolButton::toggled, this, &Sidebar::projectToggled);
        connect(btnTerminal, &QToolButton::toggled, this, &Sidebar::terminalToggled);
        connect(btnRun, &QToolButton::clicked, this, &Sidebar::runClicked);

        // Сигнал для MainWindow (нужно добавить в sidebar.h)
        connect(btnExit, &QToolButton::clicked, this, &Sidebar::exitClicked);
    }

    QToolButton* Sidebar::createButton(const QString& iconPath, const QString& tooltip) {
        auto *btn = new QToolButton(this);
        btn->setIcon(QIcon(iconPath));
        btn->setIconSize(QSize(18, 18));
        btn->setToolTip(tooltip);
        btn->setFixedSize(38, 38);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setFocusPolicy(Qt::NoFocus);
        btn->setCheckable(true);
        return btn;
    }

} // namespace gs