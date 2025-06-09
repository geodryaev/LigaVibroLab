#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qDebug>
#include <QComboBox>
#include <QFileDialog>
#include <QFile>
#include <QVector>
#include "vibrodata.h"


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
    vibroData date;

private slots:
    void ChangeAmplutude(int index);
    void on_calculateVibro_clicked();

};



#endif // MAINWINDOW_H
