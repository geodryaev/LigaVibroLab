#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qDebug>
#include <QComboBox>
#include <QFileDialog>
#include <QFile>
#include <QVector>
#include <QStringList>
#include <QMessageBox>

#include "vibrodata.h"
#include "report.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    //vibroData date;
private:
    Ui::MainWindow *ui;
    vibroData *data;
    Report report;
    double diametrs;
    double height;

private slots:
    void on_calculateVibro_clicked();

    void on_action_triggered();
    void on_calculateVibro_2_clicked();
};

#endif // MAINWINDOW_H
