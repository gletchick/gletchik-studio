#pragma once
#include <QTextEdit>
#include <QKeyEvent>

namespace gs {

    class TerminalWidget : public QTextEdit {
        Q_OBJECT
    public:
        explicit TerminalWidget(QWidget *parent = nullptr);

        // Добавление текста из процесса
        void appendOutput(const QString &text, bool isError = false);
        void clearTerminal();

        signals:
            // Сигнал, который будет испускаться при нажатии Enter
            void inputReady(const QString &text);

    protected:
        void keyPressEvent(QKeyEvent *event) override;
        void mousePressEvent(QMouseEvent *event) override;

    private:
        int m_inputStartPos = 0; // Позиция, с которой начинается пользовательский ввод
    };

} // namespace gs