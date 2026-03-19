#pragma once

#include "basewindow.h"
#include <QPushButton>

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
        QWidget* createLargeButton(const QString &iconPath, const QString &text, QWidget *parent);
    };

} // namespace gs