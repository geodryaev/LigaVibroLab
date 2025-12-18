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

class correctInput : public QDialog
{
    Q_OBJECT

public:
    explicit correctInput(vibroData * data, const double max, const double min, const double freq, QWidget *parent = nullptr);
    ~correctInput();

    unsigned long long errorMetrick();
    void transformSinToRealData(double a);

private:
    double min;
    double max;
    double freq;
    double minX;
    double maxX;
    double phi = 0;

    Ui::correctInput *ui;
    QwtPlot* plot;
    QVector<QPointF> selectPoint;
    QVector<QwtPlotMarker*> selectedMarkers;
    QwtPlotPicker * picker;
    vibroData * data;
    QwtPlotCurve * sineCurv;
    QVector<QPointF> pointsTemplatesGraph;




private slots:
    void addSineStencil(bool checked);
    void changeValueHorisontal(int value);
    void onPointClick(const QPointF &point);
    void on_pushButton_clicked();
    void on_autoAdjustmen_clicked();
    void addProgrssBar();
};


class autoAdjuystment : public QObject {
    Q_OBJECT
public:
    explicit autoAdjuystment(correctInput * owner):
        m_owner(owner){}

private:
    correctInput * m_owner;

public slots:
    void process();

signals:
    void tick();
    void reloadGraph();
};

#endif // CORRECTINPUT_H
