#include "report.h"
#include <QDesktopServices>
#include <QUrl>
Report::Report() {}


void getXYData (QVector<double> *XData, QVector<double> *YData, const vibroData * data)
{
    XData->clear();
    YData->clear();
    int count = 0;
    for (const stepVibro &el : data->steps)
    {
        if (el.isUp )
        {
            if (count == 9)
            {
                XData->append(log((el.m_time + 0.1f)*60));
                YData->append(el.epsilon_*100);
                count = 0;
            }
            else
            {
                count++;
            }
            // XData->append(el.m_time);
            // YData->append(el.m_cellPressure_kPa);
        }

    }
}


QVector<double> convertToN(const vibroData * data)
{
    QVector<double> result;
    QVector<stepVibro>::const_iterator left = data->steps.begin();
    QVector<stepVibro>::const_iterator right;
    int count = 0;

    for (QVector<stepVibro>::const_iterator it = data->steps.begin() + 1; it != data->steps.end(); ++it)
    {
        if (it->isDown || it == data->steps.end() - 1)
        {
            qDebug() << it->isDown << "\tt:" << it->m_time;
            right = it;
            double distance = right->m_time - left->m_time;
            //if (distance == 0) continue;
            result.append(count);
            for (QVector<stepVibro>::const_iterator iter = left; iter != right; ++iter)
            {
                if (iter != left)
                {
                    result.append((iter->m_time - left->m_time)/ distance + count);
                }
            }
            left = it;

            count++;
            //result.append(count);
        }

    }

    return result;
}

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

    QwtPlotCurve *curv = new QwtPlotCurve("");
    curv->setSamples(xData, yData);
    curv->setPen(QPen(Qt::green,1.1));
    curv->attach(&plot);

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setMajorPen(QPen(Qt::lightGray, 1, Qt::DashLine));
    grid->setMinorPen(QPen(Qt::gray, 0.5, Qt::DotLine));
    grid->attach(&plot);


    int width = fmax(600, xData.size() / 100);
    int height = fmax(400, yData.size() / 100);
    plot.resize(width, height);

    plot.replot();
    QImage img(plot.size(),QImage::Format_ARGB32);
    img.fill(Qt::white);
    QPainter paint(&img);
    plot.render(&paint);
    paint.end();
    return img;
}

void Report::reportToFileExcelVibrocell(const vibroData* data)
{
    QString pathToFile = QFileDialog::getSaveFileName(NULL, QObject::tr("Сохранить Excel файл"),QString(), QObject::tr("Excel Files (*.xlsx);;All Files (*)"));
    QXlsx::Document doc;
    QXlsx::Format left;
    left.setHorizontalAlignment(QXlsx::Format::AlignLeft);
    doc.write("A1","Test");
    qDebug() << "QApplication instance pointer:" << QCoreApplication::instance();
    QVector<double> XData, YData;

    doc.setColumnWidth(1,25);
    doc.write(1,1,"Высота");
    doc.write(1,2,QString::number(data->steps[0].m_h0) + " мм.");
    doc.write(2,1,"Диаметр");
    doc.write(2,2, QString::number(data->steps[0].m_d0) + " мм.");
    doc.write(3,1,"Амплитуда");
    doc.write(3,2, QString::number(data->ampl) + " кПа.");
    doc.write(4,1,"Частота");
    doc.write(4,2, QString::number(data->frequency) + " Гц");
    doc.write(5,1,"Количество циклов");
    doc.write(5,2, data->minPoints.size()-1,left );

    for(const stepVibro &el:data->steps)
    {
        if (el.PPR >= 1.0f)
        {
            doc.write(6,1,"Факт разжижения зафиксирован на " + QString::number(el.numberTact));
            doc.write(7,1, "Критерий : PPR = " + QString::number(el.PPR));
            doc.write(8,1, "Разжижение наступило");
            break;
        }
        if (el.PPR >= 0.95 and el.epsilon_ > 0.05)
        {
            doc.write(6,1, "Факт разжижения зафиксирован на " + QString::number(el.numberTact));
            doc.write(7,1, "Критерий кобинированный : PPR = " + QString::number(el.PPR) + ", ε = " + QString::number(el.epsilon_) + " %");
            doc.write(8,1, "Разжижение наступило");
            break;
        }
        doc.write(6,1, "Разжижение не наступило");
    }


    //-----------------------Графики
    for (const stepVibro &el : data->steps)
    {
            YData.append(el.epsilon_ * 100); //Потому что в графике проценты
    }
    XData = convertToN(data);
    doc.insertImage(1,5,insertGraph("Грфик зависимости осевой деформации","Число циклов нагружения N","Осевая деформация ε, %",XData, YData));
    XData.clear();
    YData.clear();


    for (const stepVibro &el : data->steps)
    {
            YData.append(el.PPR * 100); //Потому что в графике проценты
    }
    XData = convertToN(data);
    doc.insertImage(1,16,insertGraph("График относительного порового давления от числа циклов динамического нагружения","Число циклов нагружения N","Относительное поровое давление PPR, %",XData, YData));
    XData.clear();
    YData.clear();


    for (const stepVibro &el : data->steps)
    {
            YData.append(el.q);
            XData.append(el.p_);
    }
    doc.insertImage(22,5,insertGraph("График зависимости максимальных касательных напряжений от средних эффективных","p`","q",XData, YData));
    XData.clear();
    YData.clear();


    for (const stepVibro &el : data->steps)
    {
        YData.append(el.m_verticalPressure_kPa);
    }
    XData = convertToN(data);
    doc.insertImage(22,16,insertGraph("График услия","Время, мин.","Усилие, кПа.",XData, YData));
    XData.clear();
    YData.clear();

    getXYData(&XData,&YData,data);

    doc.insertImage(43,5,insertGraph("ε = f(lnt)","Время, ln(мин.)","Осевая деформация ε, %",XData, YData));
    XData.clear();
    YData.clear();

    doc.saveAs(pathToFile);
    QDesktopServices::openUrl(QUrl::fromLocalFile(pathToFile));
}

void Report::reportToFileExcelSeismic(const vibroData *data){
    QString pathToFile = QFileDialog::getSaveFileName(NULL, QObject::tr("Сохранить Excel файл"),QString(), QObject::tr("Excel Files (*.xlsx);;All Files (*)"));
    QXlsx::Document doc;
    QXlsx::Format left;
    left.setHorizontalAlignment(QXlsx::Format::AlignLeft);
    doc.write("A1","Test");
    qDebug() << "QApplication instance pointer:" << QCoreApplication::instance();
    QVector<double> XData, YData;

    doc.setColumnWidth(1,25);
    doc.write(1,1,"Высота");
    doc.write(1,2,QString::number(data->steps[0].m_h0) + " мм.");
    doc.write(2,1,"Диаметр");
    doc.write(2,2, QString::number(data->steps[0].m_d0) + " мм.");
    doc.write(3,1,"Амплитуда");
    doc.write(3,2, QString::number(data->ampl) + " кПа.");
    doc.write(4,1,"Частота");
    doc.write(4,2, QString::number(data->frequency) + " Гц");
    doc.write(5,1,"Количество циклов");
    doc.write(5,2, data->minPoints.size()-1,left );

    for(const stepVibro &el:data->steps)
    {
        if (el.PPR >= 1.0f)
        {
            doc.write(6,1,"Факт разжижения зафиксирован на " + QString::number(el.numberTact));
            doc.write(7,1, "Критерий : PPR = " + QString::number(el.PPR));
            doc.write(8,1, "Разжижение наступило");
            break;
        }
        if (el.PPR >= 0.95 and el.epsilon_ > 0.05)
        {
            doc.write(6,1, "Факт разжижения зафиксирован на " + QString::number(el.numberTact));
            doc.write(7,1, "Критерий кобинированный : PPR = " + QString::number(el.PPR) + ", ε = " + QString::number(el.epsilon_) + " %");
            doc.write(8,1, "Разжижение наступило");
            break;
        }
        doc.write(6,1, "Разжижение не наступило");
    }


    //-----------------------Графики
    for (const stepVibro &el : data->steps)
    {
        YData.append(el.epsilon_ * 100); //Потому что в графике проценты
    }
    XData = convertToN(data);
    doc.insertImage(1,5,insertGraph("Грфик зависимости осевой деформации","Число циклов нагружения N","Осевая деформация ε, %",XData, YData));
    XData.clear();
    YData.clear();


    for (const stepVibro &el : data->steps)
    {
        YData.append(el.PPR * 100); //Потому что в графике проценты
    }
    XData = convertToN(data);
    doc.insertImage(1,16,insertGraph("График относительного порового давления от числа циклов динамического нагружения","Число циклов нагружения N","Относительное поровое давление PPR, %",XData, YData));
    XData.clear();
    YData.clear();


    for (const stepVibro &el : data->steps)
    {
        YData.append(el.q);
        XData.append(el.p_);
    }
    doc.insertImage(22,5,insertGraph("График зависимости максимальных касательных напряжений от средних эффективных","p`","q",XData, YData));
    XData.clear();
    YData.clear();


    for (const stepVibro &el : data->steps)
    {
        YData.append(el.m_verticalPressure_kPa);
    }
    XData = convertToN(data);
    doc.insertImage(22,16,insertGraph("График услия","Время, мин.","Усилие, кПа.",XData, YData));
    XData.clear();
    YData.clear();

    doc.saveAs(pathToFile);
    QDesktopServices::openUrl(QUrl::fromLocalFile(pathToFile));
}

int Report::getY(int y)
{
    return height - y;
}
