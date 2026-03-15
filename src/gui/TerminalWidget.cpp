#include <QScrollBar>

#include "gui/terminalwidget.h"

namespace gs {

    TerminalWidget::TerminalWidget(QWidget *parent) : QTextEdit(parent) {
        setObjectName("terminalWidget");
        setPlaceholderText("Terminal >_");
        setStyleSheet("background-color: #1e1e1e; color: #cccccc; font-family: monospace;");
    }

    void TerminalWidget::clearTerminal() {
        clear();
        m_inputStartPos = 0;
    }

    void TerminalWidget::appendOutput(const QString &text, bool isError) {
        QTextCursor cursor = textCursor();
        cursor.movePosition(QTextCursor::End);
        setTextCursor(cursor);

        // Настраиваем цвет (красный для ошибок, обычный для stdout)
        QTextCharFormat format;
        format.setForeground(isError ? QColor("#f44747") : QColor("#cccccc"));
        cursor.mergeCharFormat(format);

        cursor.insertText(text);
        
        // Возвращаем стандартный цвет для ввода пользователя
        format.setForeground(QColor("#cccccc"));
        cursor.mergeCharFormat(format);

        m_inputStartPos = cursor.position();
        ensureCursorVisible();
    }

    void TerminalWidget::keyPressEvent(QKeyEvent *event) {
        // Защита от редактирования старого вывода
        if (textCursor().position() < m_inputStartPos) {
            QTextCursor cursor = textCursor();
            cursor.movePosition(QTextCursor::End);
            setTextCursor(cursor);
        }

        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
            QTextCursor cursor = textCursor();
            cursor.movePosition(QTextCursor::End);
            
            // Выделяем введенный пользователем текст
            cursor.setPosition(m_inputStartPos, QTextCursor::KeepAnchor);
            QString input = cursor.selectedText();
            
            // Снимаем выделение и переносим строку
            cursor.movePosition(QTextCursor::End);
            setTextCursor(cursor);
            QTextEdit::keyPressEvent(event); 
            
            m_inputStartPos = textCursor().position();
            
            // Отправляем данные в программу
            emit inputReady(input + "\n");
            return;
        }

        // Запрещаем удалять вывод программы (Backspace / Delete)
        if (event->key() == Qt::Key_Backspace && textCursor().position() <= m_inputStartPos) {
            return; 
        }

        QTextEdit::keyPressEvent(event);
    }

    void TerminalWidget::mousePressEvent(QMouseEvent *event) {
        QTextEdit::mousePressEvent(event);
        // Опционально: всегда держать курсор в конце для ввода
        if (textCursor().position() < m_inputStartPos) {
            QTextCursor cursor = textCursor();
            cursor.movePosition(QTextCursor::End);
            setTextCursor(cursor);
        }
    }

} // namespace gs