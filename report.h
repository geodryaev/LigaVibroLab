#ifndef REPORT_H
#define REPORT_H

#include <QFile>
#include <QFileDialog>
#include <xlsxdocument.h>
#include <QPixmap>
#include <QPainter>
#include <QPen>
#include <QFont>
#include <QVector>
#include <QApplication>
#include <xlsxdocument.h>
#include <QPainter>
#include <QPen>
#include <QBuffer>
#include <QImage>
#include <QVector>
#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_curve.h>
#include <qwt/qwt_plot_grid.h>
#include <cmath>

#include "vibrodata.h"

class Report
{
public:
    Report();
    void reportToFileExcelVibrocell(const vibroData* data);
    void reportToFileExcelSeismic(const vibroData* data);
private:
    int width;
    int height;

    QImage insertGraph(QString title, QString strX, QString strY, QVector<double> xData, QVector<double> yData);
    int getY(int y);
};

#endif // REPORT_H
