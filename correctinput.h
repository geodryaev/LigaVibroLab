#ifndef CORRECTINPUT_H
#define CORRECTINPUT_H

#include <QDialog>
#include <QVector>
#include <QThread>
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
    ~autoAdjustment(){qDebug() << Q_FUNC_INFO;}
    unsigned long long errorMetrick();
    void transformSinToRealData(double a);

public slots:
    void process(QVector<QPointF> *d, QVector<QPointF> *sinTemplate, double min, double max,double freq);
    void getMainGraph(double * minX, double * maxX, double * minY, double * maxY);
    void getDataForStencil(QVector<QPointF> * curvData, double minPress, double maxPress, double frequency, double phi);
    void test();
signals:
    void complateSinTemplates();
    void complateMainGraph(QVector<QPointF> * points);
    void tick();

private:
    double min;
    double max;
    double freq;
    double m_minX;
    double m_maxX;
    vibroData * data;
    QVector<QPointF> pointsTemplatesGraph;
    QVector<QPointF> p_pointsTemplatesGraph;
    QVector<QPointF> pointsMainGraph;
};


class correctInput : public QDialog
{
    Q_OBJECT

public:
    explicit correctInput(vibroData * _data, const double max, const double min, const double freq, QWidget *parent = nullptr);
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
    QVector<QPointF> pointsMainGraph;
    QVector<QPointF> pointsTemplatesGraph;
    QVector<QwtPlotMarker*> selectedMarkers;
    QThread *thread;
    double minValX;
    double maxValX;
    double minValY;
    double maxValY;
    double minSinTemp;
    double maxSinTemp;
    double freqSinTemp;

signals:
    void getMainGraph(double *minX, double * maxX, double * minY, double * maxY);
    void getDataForStencil(QVector<QPointF> * curvData, double minPress, double maxPress, double frequency, double phi);
    void sendProcessSignals(QVector<QPointF> *d,QVector<QPointF> * sinTemplate,  double min, double max, double freq);
    void test();
private slots:
    void sendProcess(bool ev);
    void onPointClick(const QPointF &point);
    void paintMainGraph(QVector<QPointF> * points);
    void paintTemplateGraph();
    void addSineStencil(bool checked);
    void changeValueHorisontal(int value);
    void addProgrssBar();
    void graphReload(const QVector<QPointF> & curvData);
    void reloadSinTemplates();
    void getDataMainGraph(QVector<QPointF> points);
};


#endif // CORRECTINPUT_H
