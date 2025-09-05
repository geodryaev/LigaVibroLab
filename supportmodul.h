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

private slots:
    void processingModileDeform(bool checked);
private:
    Ui::supportmodul *ui;
    QwtPlot *d_plot;
    QwtPlotGrid grid;
    QVector<double> vX;
    QVector<double> vY;
    QVector<QwtPlotCurve*> supCurv;
    QVector<QwtPlotMarker*> supMarker;
    const vibroData *data;
    double downDistance = 0;
    double upDistance = 0;


    void setLine(double x1, double y1, double x2, double y2);
    void clear();
    void vector(double x1, double y1, double x2, double y2);
};

#endif // SUPPORTMODUL_H
