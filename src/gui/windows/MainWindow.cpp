#include "ui_mainwindow.h"
#include "gui/sidebar/sidebar.h"
#include "gui/windows/mainwindow.h"

namespace gs {

MainWindow::MainWindow(QWidget *parent)
    : BaseWindow(parent)
    , ui(new Ui::MainWindow) {

    setObjectName("MainWindow");
    resize(1000, 700);

    setupBaseWindow("My Application");
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setupWorkspace(QWidget *contentWidget) {
    ui->setupUi(contentWidget);

    m_vSplitter = new QSplitter(Qt::Vertical, contentWidget);
    m_vSplitter->setHandleWidth(1);
    m_vSplitter->setChildrenCollapsible(false);

    QWidget *upperWidget = new QWidget(contentWidget);
    QHBoxLayout *upperLayout = new QHBoxLayout(upperWidget);
    upperLayout->setContentsMargins(0, 0, 0, 0);
    upperLayout->setSpacing(0);

    Sidebar *sidebar = new Sidebar(contentWidget);
    upperLayout->addWidget(sidebar);

    m_hSplitter = new QSplitter(Qt::Horizontal, upperWidget);
    m_hSplitter->setHandleWidth(1);
    m_hSplitter->setChildrenCollapsible(false);

    m_fileExplorer = new QWidget(contentWidget);
    m_fileExplorer->setObjectName("fileExplorer");
    m_fileExplorer->setMinimumWidth(50);

    QTextEdit *editor = new QTextEdit(contentWidget);
    editor->setObjectName("codeEditor");
    editor->setPlaceholderText("// Пишите код здесь...");
    editor->setMinimumWidth(100);

    m_hSplitter->addWidget(m_fileExplorer);
    m_hSplitter->addWidget(editor);

    m_hSplitter->setStretchFactor(0, 0);
    m_hSplitter->setStretchFactor(1, 1);

    upperLayout->addWidget(m_hSplitter);

    m_terminal = new QTextEdit(contentWidget);
    m_terminal->setObjectName("terminalWidget");
    m_terminal->setPlaceholderText("Terminal >_");
    m_terminal->setMinimumHeight(50);

    m_vSplitter->addWidget(upperWidget);
    m_vSplitter->addWidget(m_terminal);

    m_vSplitter->setStretchFactor(0, 1);
    m_vSplitter->setStretchFactor(1, 0);

    if (ui->mainVerticalLayout) {
        ui->mainVerticalLayout->addWidget(m_vSplitter);
    }

    connect(sidebar, &Sidebar::projectToggled, this, &MainWindow::onProjectToggled);
    connect(sidebar, &Sidebar::terminalToggled, this, &MainWindow::onTerminalToggled);
}

void MainWindow::onProjectToggled(bool checked) {
    m_fileExplorer->setVisible(checked);
}

void MainWindow::onTerminalToggled(bool checked) {
    m_terminal->setVisible(checked);
}

} // namespace gs