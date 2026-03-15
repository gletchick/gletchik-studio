#include "gui/windows/mainwindow.h"
#include "ui_mainwindow.h"
#include <QWindow>
#include <QString>
#include <QLabel>
#include "gui/sidebar/sidebar.h"
#include  <QTimer>
#include <thread>

#include "core/languages/java/javabuildbontroller.h"
#include "sdk/steptype.h"

namespace gs {

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setMouseTracking(true);
    setObjectName("MainWindow");
    resize(1000, 700);

    setupTitleBar();
    setupCentralArea();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setupTitleBar() {
    QWidget *titleBarWidget = new QWidget(this);
    titleBarWidget->setObjectName("titleBarWidget");
    titleBarWidget->setFixedHeight(35);

    QHBoxLayout *titleLayout = new QHBoxLayout(titleBarWidget);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(0);

    QLabel *titleLabel = new QLabel("My Application", titleBarWidget);
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

    if (ui->mainVerticalLayout) {
        ui->mainVerticalLayout->setContentsMargins(0, 0, 0, 0);
        ui->mainVerticalLayout->setSpacing(0);
        ui->mainVerticalLayout->addWidget(titleBarWidget);
    }

    connect(btnClose, &QPushButton::clicked, this, &MainWindow::onBtnCloseClicked);
    connect(m_btnMaximize, &QPushButton::clicked, this, &MainWindow::onBtnMaximizedClicked);
    connect(btnMinimize, &QPushButton::clicked, this, &MainWindow::onBtnMinimizedClicked);
}

void MainWindow::setupCentralArea() {
    m_vSplitter = new QSplitter(Qt::Vertical, this);
    m_vSplitter->setHandleWidth(1);
    m_vSplitter->setChildrenCollapsible(false); // ВАЖНО: Отключаем резкое схлопывание

    // --- ВЕРХНЯЯ ЧАСТЬ ---
    QWidget *upperWidget = new QWidget();
    QHBoxLayout *upperLayout = new QHBoxLayout(upperWidget);
    upperLayout->setContentsMargins(0, 0, 0, 0);
    upperLayout->setSpacing(0);

    Sidebar *sidebar = new Sidebar(this);
    upperLayout->addWidget(sidebar);

    m_hSplitter = new QSplitter(Qt::Horizontal, upperWidget);
    m_hSplitter->setHandleWidth(1);
    m_hSplitter->setChildrenCollapsible(false); // ВАЖНО: Отключаем резкое схлопывание

    m_fileExplorer = new QWidget();
    m_fileExplorer->setObjectName("fileExplorer");
    m_fileExplorer->setMinimumWidth(50); // Уменьшили порог до 50px для плавности

    QTextEdit *editor = new QTextEdit();
    editor->setObjectName("codeEditor");
    editor->setPlaceholderText("// Пишите код здесь...");
    editor->setMinimumWidth(100); // Чтобы редактор тоже нельзя было сжать в 0

    m_hSplitter->addWidget(m_fileExplorer);
    m_hSplitter->addWidget(editor);

    m_hSplitter->setStretchFactor(0, 0);
    m_hSplitter->setStretchFactor(1, 1);

    upperLayout->addWidget(m_hSplitter);

    // --- НИЖНЯЯ ЧАСТЬ ---
    m_terminal = new TerminalWidget(); // <-- Наш кастомный терминал
    m_terminal->setObjectName("terminalWidget");
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

    connect(sidebar, &Sidebar::runClicked, this, &MainWindow::onRunClicked);

    // Связываем ввод пользователя с отправкой в процесс
    connect(m_terminal, &TerminalWidget::inputReady, this, &MainWindow::onTerminalInput);

    // Запускаем таймер для чтения очереди ThreadSafeQueue
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &MainWindow::processOutputQueue);
    m_updateTimer->start(50); // Проверяем очередь каждые 50 мс
}

void MainWindow::onProjectToggled(bool checked) {
    m_fileExplorer->setVisible(checked);
}

void MainWindow::onTerminalToggled(bool checked) {
    m_terminal->setVisible(checked);
}

void MainWindow::onBtnCloseClicked() { close(); }
void MainWindow::onBtnMinimizedClicked() { showMinimized(); }
void MainWindow::onBtnMaximizedClicked() {
    if (isMaximized()) {
        showNormal();
        m_btnMaximize->setText("▢");
    } else {
        showMaximized();
        m_btnMaximize->setText("❐");
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
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

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
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

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    m_resizeDir = None;
    event->accept();
}

MainWindow::ResizeDir MainWindow::getResizeDir(const QPoint &pos) {
    int x = pos.x(); int y = pos.y();
    int w = width(); int h = height();
    bool left = (x <= m_boundary); bool right = (x >= w - m_boundary);
    bool top = (y <= m_boundary); bool bottom = (y >= h - m_boundary);

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

void MainWindow::updateCursorShape(const QPoint &pos) {
    ResizeDir dir = getResizeDir(pos);
    switch (dir) {
        case Top: case Bottom: setCursor(Qt::SizeVerCursor); break;
        case Left: case Right: setCursor(Qt::SizeHorCursor); break;
        case TopLeft: case BottomRight: setCursor(Qt::SizeFDiagCursor); break;
        default: setCursor(Qt::ArrowCursor); break;
    }
}

    void MainWindow::onRunClicked() {
    m_terminal->clearTerminal();
    m_terminal->appendOutput("--- Starting Java Application ---\n", false);

    if (!m_controller) {
        m_controller = std::make_shared<JavaBuildController>();
    } else {
        m_controller->stop(); // Останавливаем предыдущий запуск, если он был
    }

    // Запускаем процесс в фоновом потоке, чтобы не блокировать GUI!
    std::thread([this]() {
        std::string projectPath = ".";        // Путь к корню
        std::string sourceFilePath = "Main.java"; // Точка входа

        m_controller->runProject(projectPath, sourceFilePath, StepType::Run,
            [this](const std::string& text, bool isError) {
                // Префиксируем сообщение, чтобы в UI потоке понимать, ошибка это или нет
                std::string prefix = isError ? "E:" : "O:";
                m_outputQueue.push(prefix + text);
            }
        );

        m_outputQueue.push("O:\n--- Process Finished ---\n");
    }).detach();




}

    void MainWindow::onTerminalInput(const QString& text) {
    if (m_controller) {
        m_controller->writeInput(text.toStdString());
    }
}

    void MainWindow::processOutputQueue() {
    while (auto msg = m_outputQueue.tryPop()) {
        QString text = QString::fromStdString(*msg);

        // Расшифровываем префикс
        bool isError = text.startsWith("E:");

        // Выводим в терминал (убрав первые 2 символа префикса)
        m_terminal->appendOutput(text.mid(2), isError);
    }
}
} // namespace gs