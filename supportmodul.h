#ifndef SUPPORTMODUL_H
#define SUPPORTMODUL_H

#include <QDialog>
#include "vibrodata.h"
#include "qwt/qwt_plot.h"
#include "qwt/qwt_plot_curve.h"
#include "qwt/qwt_plot_grid.h"
#include "qwt/qwt_plot_marker.h"

namespace Ui {
class supportmodul;
}

class supportmodul : public QDialog
{
    Q_OBJECT

public:
    explicit supportmodul(bool def, int countCilce,const vibroData* data, QWidget *parent = nullptr);
    ~supportmodul();
    QwtPlot * getImage();
    double getModule();

private slots:
    void processingModileDeform(bool checked);
    void on_buttonBox_accepted();
    void changeRange(int index);

private:
    Ui::supportmodul *ui;
    QwtPlot *d_plot;
    QwtPlotGrid grid;
    QVector<double> vX;
    QVector<double> vY;
    QVector<QwtPlotCurve*> supCurv;
    QVector<QwtPlotMarker*> supMarker;
    QwtPlot * img = nullptr;
    QVector<QPointer<QPushButton>> vecButton;
    //choice == true -> динамический модуль упрогости
    //choice == false -> динамический модуль дуформации

    const vibroData *data;
    bool choice;
    double downDistance = 0;
    double upDistance = 0;
    double dinamicModElastic = 0; //модуль упругсоти
    double dinamicModDifform = 0; //модуль дифформации
    int countPoints = 0;

    void setLine(double x1, double y1, double x2, double y2);
    void clear();
    void vector(double x1, double y1, double x2, double y2);
    void setText(int maskPosition, QString text, double x1, double y1, double x2, double y2);
};

#endif // SUPPORTMODUL_H
