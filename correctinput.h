#ifndef CORRECTINPUT_H
#define CORRECTINPUT_H

#include <QDialog>
#include <QVector>
#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_curve.h>
#include <qwt/qwt_plot_grid.h>
#include <qwt/qwt_plot_zoomer.h>
#include <qwt/qwt_plot_canvas.h>
#include <qwt/qwt_plot_picker.h>
#include <qwt/qwt_picker_machine.h>
#include <qwt/qwt_plot_marker.h>
#include <qwt/qwt_symbol.h>
#include <QDebug>

#include "vibrodata.h"


namespace Ui {
class correctInput;
}


class autoAdjustment : public QObject {
    Q_OBJECT
public:
    explicit autoAdjustment(vibroData *data)
        :data(data)
    {

    }

    unsigned long long errorMetrick();
    void transformSinToRealData(double a);

public slots:
    void process(QVector<QPointF> * d,double min, double max,double freq, double phi);
    void getMainGraph(vibroData &data, double * minX, double * maxX, double * minY, double * maxY);
    void getDataForStencil(QVector<QPointF> * curvData, double minPress, double maxPress, double frequency, double phi);

signals:
    void complateSinTemplates();
    void complateMainGraph(QVector<QPointF> points);
private:
    double min;
    double max;
    double freq;
    double m_minX;
    double m_maxX;
    vibroData * data;
    QVector<QPointF> pointsTemplatesGraph;
};


class correctInput : public QDialog
{
    Q_OBJECT

public:
    explicit correctInput(vibroData * data, const double max, const double min, const double freq, QWidget *parent = nullptr);
    ~correctInput();

private:
    double phi = 1;
    vibroData * data;
    QwtPlot* plot;
    Ui::correctInput *ui;
    QwtPlotPicker * picker;
    autoAdjustment * worker;
    QwtPlotCurve * sineCurv;
    QwtPlotCurve * curv;
    QVector<QPointF> selectPoint;
    QVector<QPointF> pointsTemplatesGraph;
    QVector<QwtPlotMarker*> selectedMarkers;
    double  minValX;
    double  maxValX;
    double  minValY;
    double  maxValY;

signals:
    void getMainGraph(vibroData &data, double * minX, double * maxX, double * minY, double * maxY);
    void getDataForStencil(QVector<QPointF> * curvData, double minPress, double maxPress, double frequency, double phi);
private slots:
    void onPointClick(const QPointF &point);
    void paintMainGraph(QVector<QPointF> points);
    void addSineStencil(bool checked);
    void changeValueHorisontal(int value);
    void on_pushButton_clicked();
    void on_autoAdjustmen_clicked();
    void addProgrssBar();
    void graphReload(const QVector<QPointF> & curvData);
    void reloadSinTemplates();
    void getDataMainGraph(QVector<QPointF> points);
};

#endif // CORRECTINPUT_H
