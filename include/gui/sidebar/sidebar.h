#pragma once
#include <QWidget>
#include <QToolButton>
#include <QVBoxLayout>

namespace gs {

    class Sidebar : public QWidget {
        Q_OBJECT
    public:
        explicit Sidebar(QWidget *parent = nullptr);

        signals:
            void projectToggled(bool checked);
        void terminalToggled(bool checked);
        void runClicked();

    private:
        QToolButton* createButton(const QString& iconPath, const QString& tooltip);
    };

} // namespace gs