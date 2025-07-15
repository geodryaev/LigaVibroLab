#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qDebug>
#include <QComboBox>
#include <QFileDialog>
#include <QFile>
#include <QVector>

#include "vibrodata.h"
#include "report.h"
#include "calcamplitud.h"
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
    vibroData *date;
    Report report;
    double diametrs;
    double height;

private slots:
    void ChangeAmplutude(int index);
    void on_calculateVibro_clicked();

    void on_action_triggered();
};



#endif // MAINWINDOW_H
