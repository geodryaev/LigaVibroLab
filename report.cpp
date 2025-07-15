#include "report.h"

Report::Report() {}




QImage Report::insertGraph(QString title, QString strX, QString strY, QVector<double> xData, QVector<double> yData)
{
    QwtPlot plot;
    double minValX = *std::min_element(xData.begin(), xData.end());
    double maxValX = *std::max_element(xData.begin(), xData.end());

    double minValY = *std::min_element(yData.begin(), yData.end());
    double maxValY = *std::max_element(yData.begin(), yData.end());
    plot.setTitle(title);
    plot.setCanvasBackground(Qt::white);
    plot.setAxisTitle(QwtPlot::xBottom, strX);
    plot.setAxisTitle(QwtPlot::yLeft, strY);
    plot.setAxisScale(QwtPlot::xBottom, minValX, maxValX);
    plot.setAxisScale(QwtPlot::yLeft, minValY, maxValY);
    // QVector<double> XData, YData;

    // for (const stepVibro &el : steps)
    // {
    //     if (!qIsNaN(el.epsilon_) && !qIsNaN(el.m_time) &&  !qIsInf(el.epsilon_) && !qIsInf(el.m_time))
    //     {
    //         YData.append(el.epsilon_);
    //         XData.append(el.m_time);
    //     }
    // }


    QwtPlotCurve *curv = new QwtPlotCurve("");
    curv->setSamples(xData, yData);
    curv->setPen(QPen(Qt::red,2));
    curv->attach(&plot);


    plot.resize(800,600);
    plot.replot();
    QImage img(plot.size(),QImage::Format_ARGB32);
    img.fill(Qt::white);
    QPainter paint(&img);
    plot.render(&paint);
    paint.end();
    return img;
}



void Report::reportToFileExcel(vibroData* data)
{
    QString pathToFile = QFileDialog::getSaveFileName(NULL, QObject::tr("Сохранить Excel файл"),QString(), QObject::tr("Excel Files (*.xlsx);;All Files (*)"));
    QXlsx::Document doc;
    doc.write("A1","Test");
    qDebug() << "QApplication instance pointer:" << QCoreApplication::instance();
    QVector<double> XData, YData;

    for (const stepVibro &el : data->steps)
    {
        if (!qIsNaN(el.epsilon_) && !qIsNaN(el.m_time) &&  !qIsInf(el.epsilon_) && !qIsInf(el.m_time))
        {
            YData.append(el.epsilon_ * 100); //Потому что в графике проценты
            XData.append(el.m_time);
        }
    }
    doc.insertImage(5,5,insertGraph("test","Число циклов нагружения N","Осевая деформация ε, %",XData, YData));


    doc.saveAs(pathToFile);

}

int Report::getY(int y)
{
    return height - y;
}
