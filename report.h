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

#include "vibrodata.h"


class Report
{
public:
    Report();
    void reportToFileExcel(vibroData date);

};

#endif // REPORT_H
