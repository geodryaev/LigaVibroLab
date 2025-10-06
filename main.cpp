#include "mainwindow.h"
#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile file(":/style/style.qss");
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString style = QLatin1String(file.readAll());
        a.setStyleSheet(style);
    }
    a.setWindowIcon(QIcon(":/files/icon.png"));

    MainWindow w;
    w.setWindowIcon(QIcon(":/files/icon.png"));
    w.show();

    return a.exec();
}
