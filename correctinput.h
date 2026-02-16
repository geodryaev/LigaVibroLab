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
    void smoothMainGraph(QVector<QPointF> * d, QVector<QPointF> * temp);
signals:
    void complateSinTemplates();
    void complateMainGraph(QVector<QPointF> * points);
    void tick();
    void reloadMainGraph();


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

private:
    void smoothTemplateGraph(QVector<QPointF> *p);
    bool upToFirstPoint(QVector<QPointF>::Iterator *it);
    QVector<QPointF>::Iterator nextPoint(QVector<QPointF>::Iterator start, QVector<QPointF>::Iterator end, bool state);
    double summOffset(QVector<QPointF>::Iterator s1, QVector<QPointF>::Iterator s2, QVector<QPointF> *p, bool state);
    void movePoints(QVector<QPointF>::Iterator s1, QVector<QPointF>::Iterator s2, QVector<QPointF> *p, double target, bool state);
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
    QwtPlotGrid *grid;
    QwtPlotZoomer * zoom;
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
    void smoothMainGraph(QVector<QPointF> * d, QVector<QPointF> * temp);

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
    void reloadMainGraph();
    void on_pushButton_clicked();
};


#endif // CORRECTINPUT_H
