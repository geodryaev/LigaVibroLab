#ifndef SUPPORTMODUL_H
#define SUPPORTMODUL_H

#include <QDialog>
#include "vibrodata.h"
#include "qwt/qwt_plot.h"
#include "qwt/qwt_plot_curve.h"

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
    QVector<double> vX;
    QVector<double> vY;
    const vibroData *data;
    QwtPlot *d_plot;
    QwtPlotCurve * last = nullptr;
};

#endif // SUPPORTMODUL_H
