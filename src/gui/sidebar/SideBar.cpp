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

        auto *btnProject = createButton(":/icons/project.svg", "Project Explorer");
        auto *btnRun = createButton(":/icons/run.svg", "Run Application");
        auto *btnTerminal = createButton(":/icons/terminal.svg", "Terminal Control");

        auto *btnExit = createButton(":/icons/exit.svg", "Close Project");
        btnExit->setCheckable(false);
        btnExit->setObjectName("btnExitSidebar");

        btnProject->setChecked(true);
        layout->addWidget(btnProject);

        layout->addStretch();

        layout->addWidget(btnRun);
        layout->addWidget(btnTerminal);

        layout->addWidget(btnExit);

        connect(btnProject, &QToolButton::toggled, this, &Sidebar::projectToggled);
        connect(btnTerminal, &QToolButton::toggled, this, &Sidebar::terminalToggled);
        connect(btnRun, &QToolButton::clicked, this, &Sidebar::runClicked);

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