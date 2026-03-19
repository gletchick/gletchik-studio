#pragma once

#include "basewindow.h"
#include <QPushButton>
#include <QVBoxLayout>

namespace gs {

    class StartWindow : public BaseWindow {
        Q_OBJECT

    public:
        explicit StartWindow(QWidget *parent = nullptr);
        ~StartWindow() override = default;

    protected:
        void setupWorkspace(QWidget *contentWidget) override;

    private:
        void createActionButtons(QWidget *container, QLayout *layout);
        QPushButton* createLargeButton(const QString &text, const QString &subText, QWidget *parent);
    };

} // namespace gs