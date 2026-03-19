#include <QFile>
#include <QApplication>

#include "gui/windows/mainwindow.h"
#include "gui/windows/startwindow.h"

void loadStyleSheet(const QString& path) {
    QFile file(path);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString styleSheet = QLatin1String(file.readAll());
        qApp->setStyleSheet(styleSheet);
    }
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    loadStyleSheet(":/themes/dark.qss");

    gs::StartWindow w;
    w.show();
    return a.exec();
}