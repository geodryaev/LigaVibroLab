#include "report.h"
#include "qwt/qwt_symbol.h"
#include "supportmodul.h"
#include "xlsxrichstring.h"
#include "QFont"
#include <QDesktopServices>
#include <QUrl>
#include <QSvgGenerator>
#include <QFont>
#include <QSettings>
#include "qwt/qwt_text.h"


Report::Report() {

    if (QFile::exists("settings.ini"))
    {
        QSettings setting ("settings.ini");
        colorGraph1 = QColor(setting.value("color/graph1").toString()).isValid() ? QColor(setting.value("color/graph1").toString()) : Qt::white;
        colorGraph2 = QColor(setting.value("color/graph2").toString()).isValid() ? QColor(setting.value("color/graph2").toString()) : Qt::white;
        colorBackround= QColor(setting.value("color/backround").toString()).isValid() ? QColor(setting.value("color/backround").toString()) : Qt::black;
    }

}

double getA(QVector<QPair<double,double>>::Iterator it, QVector<QPair<double,double>>::Iterator end)
{
    double sigma1 = 0; //xy
    double sigma2 = 0; //x
    double sigma3 = 0; //y
    double sigma4 = 0; //x*x
    int count = 0;
    for (int i = 0; it+i != end; i++)
    {
        count++;
        sigma1 += (it+i)->first * (it+i)->second;
        sigma2 += (it+i)->first;
        sigma3 += (it+i)->second;
        sigma4 += (it+i)->first * (it+i)->first;
    }

    return (count * sigma1 - sigma2 * sigma3)/(count * sigma4 - sigma2 * sigma2);
}

double getB(QVector<QPair<double,double>>::Iterator it,QVector<QPair<double,double>>::Iterator end)
{
    double sigma1 = 0; //y
    double sigma2 = 0; //x
    int count = 0;
    for (int i = 0;it+i != end; i++)
    {
        count++;
        sigma1 += (it+i)->second;
        sigma2 += (it+i)->first;
    }

    return (sigma1 - getA(it,end)*sigma2)/count;
}

void getFunc(QVector<QPair<double,double>>* vec, double* a, double* b)
{

    QVector <QPair<double,double>>::Iterator  it = vec->begin();
    for (; it < vec->end() and  it->first < 6;  it++)
    {
        it++;
    }

    *a = getA(it, vec->end());
    *b = getB(it, vec->end());
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

double getW (QVector<double> *XData, QVector<double> *YData, const vibroData * data)
{
    XData->clear();
    YData->clear();
    double w = 0;

    for (QVector<stepVibro>::ConstIterator it = data->steps.begin(); it < data->steps.end()-1; it++)
    {
        w+= 0.5f * ( (it+1)->m_verticalPressure_kPa + it->m_verticalPressure_kPa) * ((it+1)->epsilon_ - it->epsilon_);
        XData->append(it->epsilon_);
        YData->append(it->m_verticalPressure_kPa);
    }

    return w;
}

void getXYDataEpsD (QVector<double> *XData, QVector<double> *YData, const vibroData * data)
{
    XData->clear();
    YData->clear();
    int count = 0;
    stepVibro last = data->steps[0];
    for (const stepVibro &el : data->steps)
    {
        if (el.isDown)
        {
             // if (last.epsilon_ * 0.8 < el.epsilon_)
            if (count > 9)
            {
                XData->append(log((el.m_time + 0.1f)*60));
                YData->append(el.epsilon_);
                // last = el;
                count = 0;
            }
            else
            {
                count++;
            }
        }
    }
}

void Report::savePlotAsSvg(QwtPlot * plot, QString fylePath)
{
    QSvgGenerator generator;
    generator.setFileName(fylePath);
    generator.setSize(QSize(800,600));
    generator.setViewBox(QRect(0,0,800,600));
    generator.setTitle("График");
    generator.setDescription("test2");

    QPainter paint;
    paint.begin(&generator);
    plot->render(&paint);
    paint.end();
}

QwtPlot * Report::getModulsDeforms(const vibroData* data, double *module, bool choice)
{

    //choice == true -> динамический модуль упрогости
    //choice == false -> динамический модуль дуформации
    QVector<double> vX;
    QVector<double> vY;
    int count = 0;
    int countCicle = 6;
    for (int i = 0; i < data->steps.size() && count < countCicle;i++)
    {
        if (data->steps[i].isDown)
            count++;
        vX.push_back(data->steps[i].epsilon_);
        vY.push_back(data->steps[i].m_verticalPressure_kPa);
    }
    qDebug() << countCicle << '\n';
    supportmodul *sup = new supportmodul(choice, countCicle, data);
    sup->setWindowState(Qt::WindowMaximized);
    sup->exec();

    *module = sup->getModule();



    if (sup->getImage() != nullptr)
        return sup->getImage();

    return nullptr;
}

QwtPlot * Report::insertGraph(QString title, QString strX, QString strY, QVector<double> xData, QVector<double> yData)
{
    QwtPlot * plot = new QwtPlot();
    double minValX = *std::min_element(xData.begin(), xData.end());
    double maxValX = *std::max_element(xData.begin(), xData.end());

    double minValY = *std::min_element(yData.begin(), yData.end());
    double maxValY = *std::max_element(yData.begin(), yData.end());


    QwtText tTitle (title);
    QFont font = tTitle.font();
    font.setPointSize(26);
    tTitle.setFont(font);
    plot->setTitle(tTitle);
    plot->setCanvasBackground(colorBackround);
    plot->setAxisTitle(QwtPlot::xBottom, strX);
    plot->setAxisTitle(QwtPlot::yLeft, strY);
    plot->setAxisScale(QwtPlot::xBottom, minValX, maxValX);
    plot->setAxisScale(QwtPlot::yLeft, minValY, maxValY);

    QwtPlotCurve *curv = new QwtPlotCurve("");
    curv->setSamples(xData, yData);
    curv->setPen(QPen(colorGraph1,1.1));
    curv->attach(plot);

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setMajorPen(QPen(Qt::lightGray, 1, Qt::DashLine));
    grid->setMinorPen(QPen(Qt::gray, 0.5, Qt::DotLine));
    grid->attach(plot);

    int width = fmax(800, xData.size() / 100);
    int height = fmax(600, yData.size() / 100);
    plot->resize(width, height);

    plot->replot();
    return plot;
}

QwtPlot * Report::insertGraph(QString title, QString strX, QString strY, QVector<double> xData, QVector<double> yData, double *a, double *b)
{
    QwtPlot * plot = new QwtPlot();
    QVector<QPair<double,double>> dataEpsilon;
    if (xData.isEmpty() || yData.isEmpty())
        return plot;

    double minValX = *std::min_element(xData.begin(), xData.end());
    double maxValX = *std::max_element(xData.begin(), xData.end());

    double minValY = *std::min_element(yData.begin(), yData.end());
    double maxValY = *std::max_element(yData.begin(), yData.end());

    QwtText tTitle (title);
    QFont font = tTitle.font();
    font.setPointSize(26);
    tTitle.setFont(font);
    plot->setTitle(tTitle);
    plot->setCanvasBackground(colorBackround);
    plot->setAxisTitle(QwtPlot::xBottom, strX);
    plot->setAxisTitle(QwtPlot::yLeft, strY);
    plot->setAxisScale(QwtPlot::xBottom, minValX, maxValX);
    plot->setAxisScale(QwtPlot::yLeft, minValY, maxValY);

    QwtPlotCurve *curv = new QwtPlotCurve();
    curv->setSamples(xData, yData);
    curv->setStyle(QwtPlotCurve::NoCurve);
    curv->setSymbol(new QwtSymbol(
        QwtSymbol::Ellipse,
        QBrush(Qt::black),
        QPen(colorGraph1),
        QSize(1, 1)
        ));

    curv->attach(plot);

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setMajorPen(QPen(Qt::lightGray, 1, Qt::DashLine));
    grid->setMinorPen(QPen(Qt::gray, 0.5, Qt::DotLine));
    grid->attach(plot);


    int width = fmax(800, xData.size() / 100);
    int height = fmax(600, yData.size() / 100);
    plot->resize(width, height);

    //ed
    for (int i = 0;  i < yData.size(); i++)
    {
        dataEpsilon.push_back(qMakePair(xData[i],yData[i]));
    }
    qDebug() << "Start test";
    getFunc(&dataEpsilon,a,b);

    if (a && b)
    {
        QVector<double> xLine, yLine;
        xLine << minValX << maxValX;
        yLine << (*a) * minValX + (*b) << (*a) * maxValX + (*b);

        QwtPlotCurve *line = new QwtPlotCurve("y = ax + b");
        line->setSamples(xLine, yLine);
        line->setPen(QPen(colorGraph2, 1.5, Qt::SolidLine)); // стиль линии
        line->attach(plot);
    }

    plot->replot();
    return plot;
}

QwtPlot * Report::insertGraph(QString title, QString strX, QString strY, QString strY2, QVector<double> xData, QVector<double> yData, QVector<double> yData2)
{
    QwtPlot * plot;
    double minValX = *std::min_element(xData.begin(), xData.end());
    double maxValX = *std::max_element(xData.begin(), xData.end());

    double minValY = *std::min_element(yData.begin(), yData.end());
    double maxValY = *std::max_element(yData.begin(), yData.end());
    double minValY2 = *std::min_element(yData2.begin(), yData2.end());
    double maxValY2 = *std::max_element(yData2.begin(), yData2.end());

    if (minValY2 < minValY)
    {
        minValY = minValY2;
    }

    if (maxValY2 > maxValY)
    {
        maxValY = maxValY2;
    }

    QwtText tTitle (title);
    QFont font = tTitle.font();
    font.setPointSize(26);
    tTitle.setFont(font);
    plot->setTitle(tTitle);
    plot->setCanvasBackground(colorBackround);
    plot->setAxisTitle(QwtPlot::xBottom, strX);
    plot->setAxisTitle(QwtPlot::yLeft, strY);
    plot->setAxisScale(QwtPlot::xBottom, minValX, maxValX);
    plot->setAxisScale(QwtPlot::yLeft, minValY, maxValY);

    QwtPlotCurve *curv = new QwtPlotCurve("");
    curv->setSamples(xData, yData);
    curv->setPen(QPen(colorGraph1,1.1));
    curv->attach(plot);

    QwtPlotCurve *curv2 = new QwtPlotCurve("");
    curv2->setSamples(xData, yData2);
    curv2->setPen(QPen(colorGraph2,1.1));
    curv2->attach(plot);

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setMajorPen(QPen(Qt::lightGray, 1, Qt::DashLine));
    grid->setMinorPen(QPen(Qt::gray, 0.5, Qt::DotLine));
    grid->attach(plot);

    int width = fmax(800, xData.size() / 100);
    int height = fmax(600, yData.size() / 100);

    plot->resize(width, height);

    plot->replot();
    return plot;
}

//Вибропазлучесть
void Report::reportToFileExcelVibrocell(const vibroData* data)
{
    if (data->minPoints.size() < 499)
    {
        QMessageBox::critical(nullptr, "Неверные данные","В вашей выгрузки колчиество циклов составляет " +
                                                              QString::number(data->minPoints.size()) + ", у вас должно быть минимум 500 циклов в соотвествии с ГОСТ Р 56353-2022 п. 6.4.3.4");
        return;
    }

    QString pathToFolder = QFileDialog::getSaveFileName(NULL, QObject::tr("Введите имя отчёта"),QString(), QObject::tr("Папка отчета"));

    if (!pathToFolder.isEmpty())
    {
        QFileInfo fi(pathToFolder);
        reportDir = fi.absolutePath() + "/" + fi.baseName();

        QDir dir;
        if (!dir.exists(reportDir))
            dir.mkdir(reportDir);

    QXlsx::Document doc;
    QXlsx::Format left;
    double a = 0;
    double b = 0;
    left.setHorizontalAlignment(QXlsx::Format::AlignLeft);
    doc.write("A1","Test");
    qDebug() << "QApplication instance pointer:" << QCoreApplication::instance();
    QVector<double> XData, YData, YData2;

    doc.setColumnWidth(1,25);
    doc.write(1,1,"Высота");
    doc.write(1,2,QString::number(data->steps[0].m_h0) + " мм.");
    doc.write(2,1,"Диаметр");
    doc.write(2,2, QString::number(data->steps[0].m_d0) + " мм.");
    doc.write(3,1,"Амплитуда");
    doc.write(3,2, QString::number(data->ampl/2) + " кПа.");
    doc.write(4,1,"Частота");
    doc.write(4,2, QString::number(data->frequency) + " Гц");
    doc.write(5,1,"Количество циклов");
    doc.write(5,2, data->minPoints.size(),left );

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
    savePlotAsSvg(insertGraph("График зависимости осевой деформации","Число циклов нагружения N","Осевая деформация ε, %",XData, YData),
                  reportDir + "/График зависимости осевой деформации.svg");
    doc.write(1,5,"=HYPERLINK(\"График зависимости осевой деформации.svg\", \"График зависимости осевой деформации\")");
    // doc.insertImage(1,5,insertGraph("График зависимости осевой деформации","Число циклов нагружения N","Осевая деформация ε, %",XData, YData));
    XData.clear();
    YData.clear();


    for (const stepVibro &el : data->steps)
    {
            YData.append(el.PPR * 100); //Потому что в графике проценты
    }
    XData = convertToN(data);
    savePlotAsSvg(insertGraph("График зависимости относительного порового давления от числа циклов динамического нагружения","Число циклов нагружения N","Относительное поровое давление PPR, %",XData, YData),reportDir + "/График зависимости относительного порового давления от числа циклов динамического нагружения.svg");
    doc.write(2,5,"=HYPERLINK(\"График зависимости относительного порового давления от числа циклов динамического нагружения.svg\", \"График зависимости относительного порового давления от числа циклов динамического нагружения\")");
    // doc.insertImage(1,16,insertGraph("График зависимости относительного порового давления от числа циклов динамического нагружения","Число циклов нагружения N","Относительное поровое давление PPR, %",XData, YData));
    XData.clear();
    YData.clear();

    for (const stepVibro &el : data->steps)
    {
            YData.append(el.q);
            XData.append(el.p_);
    }
    savePlotAsSvg(insertGraph("График зависимости максимальных касательных напряжений от средних эффективных напряжений","p`, кПа.","q, кПа.",XData, YData),reportDir + "/График зависимости максимальных касательных напряжений от средних эффективных напряжений.svg");
    doc.write(3,5,"=HYPERLINK(\"График зависимости максимальных касательных напряжений от средних эффективных напряжений.svg\", \"График зависимости максимальных касательных напряжений от средних эффективных напряжений\")");
    // doc.insertImage(22,5,insertGraph("График зависимости максимальных касательных напряжений от средних эффективных напряжений","p`, кПа.","q, кПа.",XData, YData));
    XData.clear();
    YData.clear();


    for (const stepVibro &el : data->steps)
    {
        YData.append(el.m_verticalPressure_kPa);
    }
    XData = convertToN(data);
    savePlotAsSvg(insertGraph("График зависимости напряжения от времени","Время, мин.","Вертикальное напряжение, кПа.",XData, YData),reportDir + "/График зависимости напряжения от времени.svg");
    doc.write(4,5,"=HYPERLINK(\"График зависимости напряжения от времени.svg\", \"График зависимости напряжения от времени\")");
    // doc.insertImage(22,16,insertGraph("График зависимости напряжения от времени","Время, мин.","Вертикальное напряжение, кПа.",XData, YData));
    XData.clear();
    YData.clear();

    double w = getW(&XData,&YData,data);
    doc.write(9,1,"ΔW");
    doc.write(9,2,QString::number(w,'f',6) + " кПа.");
    savePlotAsSvg(insertGraph("График Σ–Ε","Осевая деформация ε", "Девиатор напряжений σ, кПа.",XData,YData),reportDir + "/График Σ–Ε.svg");
    doc.write(5,5,"=HYPERLINK(\"График Σ–Ε.svg\", \"График Σ–Ε\")");
    // doc.insertImage(43,5, insertGraph("График Σ–Ε","Осевая деформация ε", "Девиатор напряжений σ, кПа.",XData,YData));

    getXYDataEpsD(&XData,&YData,data);
    savePlotAsSvg(insertGraph("ε = f(lnt)","Время, ln(мин.)","Осевая деформация ε, %",XData, YData,&a,&b),reportDir + "/ε = f(lnt).svg");
    doc.write(6,5,"=HYPERLINK(\"ε = f(lnt).svg\", \"ε = f(lnt)\")");
    // doc.insertImage(43,16,insertGraph("ε = f(lnt)","Время, ln(мин.)","Осевая деформация ε, %",XData, YData,&a,&b));
    doc.write(10,1,"a");
    doc.write(10,2, a);
    doc.write(11,1,"b");
    doc.write(11,2,b);
    doc.write(12,1,"Введите параметр t(сек)");
    doc.write(12,2,"1");
    doc.write(13,1,"ɛ(d)");
    doc.write(13,2,"=B10*ln(B12)+B11");
    XData.clear();
    YData.clear();

    QXlsx::RichString r;
    QXlsx::Format normal;
    QXlsx::Format sub;
        QXlsx::Format sup;
    sup.setFontScript(QXlsx::Format::FontScriptSuper);
    sub.setFontScript(QXlsx::Format::FontScriptSub);


    double moduleDeform;

    savePlotAsSvg(getModulsDeforms(data, &moduleDeform, true),reportDir + "/Eyd.svg");
    doc.write(7,5,"=HYPERLINK(\"Eyd.svg\", \"Eyd.svg\")");
    // doc.insertImage(64,5, getModulsDeforms(data, &moduleDeform, true));
    r.addFragment("E", normal);
    r.addFragment("y",sup);
    r.addFragment("d",sub);
    doc.write(7,1,r);
    doc.write(7,2,QString::number(moduleDeform) + " МПа.");


    savePlotAsSvg(getModulsDeforms(data, &moduleDeform, false),reportDir + "/Ed.svg");
    doc.write(8,5,"=HYPERLINK(\"Ed.svg\", \"Ed\")");
    // doc.insertImage(64,16, getModulsDeforms(data, &moduleDeform, false));
    r = QXlsx::RichString();
    r.addFragment("E", normal);
    r.addFragment("d",sub);
    doc.write(8,1,r);
    doc.write(8,2,QString::number(moduleDeform) + " МПа.");

    doc.saveAs(reportDir + "/report.xlsx");
    QDesktopServices::openUrl(QUrl::fromLocalFile(reportDir + "/report.xlsx"));
    }

}

//Cейсмо
void Report::reportToFileExcelSeismic(const vibroData *data)
{

    QString pathToFolder = QFileDialog::getSaveFileName(NULL, QObject::tr("Введите имя отчёта"),QString(), QObject::tr("Папка отчета"));

    if (!pathToFolder.isEmpty())
    {

        QFileInfo fi(pathToFolder);
        reportDir = fi.absolutePath() + "/" + fi.baseName();

        QDir dir;
        if (!dir.exists(reportDir))
            dir.mkdir(reportDir);

        QXlsx::Document doc;
        QXlsx::Format left;
        left.setHorizontalAlignment(QXlsx::Format::AlignLeft);
        QVector<double> XData, YData;

        doc.setColumnWidth(1,25);
        doc.write(1,1,"Высота");
        doc.write(1,2,QString::number(data->steps[0].m_h0) + " мм.");
        doc.write(2,1,"Диаметр");
        doc.write(2,2, QString::number(data->steps[0].m_d0) + " мм.");
        doc.write(3,1,"Амплитуда");
        doc.write(3,2, QString::number(data->ampl/2) + " кПа.");
        doc.write(4,1,"Частота");
        doc.write(4,2, QString::number(data->frequency) + " Гц");
        doc.write(5,1,"Количество циклов");
        doc.write(5,2, data->minPoints.size(),left );

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
        savePlotAsSvg(insertGraph("График зависимости осевой деформации","Число циклов нагружения N","Осевая деформация ε, %",XData, YData),reportDir + "/График зависимости осевой деформации.svg");
        doc.write(1,5,"=HYPERLINK(\"График зависимости осевой деформации.svg\", \"График зависимости осевой деформации\")");
        XData.clear();
        YData.clear();


        for (const stepVibro &el : data->steps)
        {
            YData.append(el.PPR * 100); //Потому что в графике проценты
        }
        XData = convertToN(data);
        savePlotAsSvg(insertGraph("График зависимости относительного порового давления от числа циклов динамического нагружения","Число циклов нагружения N","Относительное поровое давление PPR, %",XData, YData),reportDir + "/График зависимости относительного порового давления от числа циклов динамического нагружения.svg");
        doc.write(2,5,"=HYPERLINK(\"График зависимости относительного порового давления от числа циклов динамического нагружения.svg\", \"График зависимости относительного порового давления от числа циклов динамического нагружения\")");
        //doc.insertImage(1,16,insertGraph("График зависимости относительного порового давления от числа циклов динамического нагружения","Число циклов нагружения N","Относительное поровое давление PPR, %",XData, YData));
        XData.clear();
        YData.clear();


        for (const stepVibro &el : data->steps)
        {
            YData.append(el.q);
            XData.append(el.p_);
        }
        savePlotAsSvg(insertGraph("График зависимости максимальных касательных напряжений от средних эффективных напряжений","p`, кПа.","q, кПа.",XData, YData),reportDir + "/График зависимости максимальных касательных напряжений от средних эффективных напряжений.svg");
        doc.write(3,5,"=HYPERLINK(\"График зависимости максимальных касательных напряжений от средних эффективных напряжений.svg\", \"График зависимости максимальных касательных напряжений от средних эффективных напряжений\")");
        //doc.insertImage(22,5,insertGraph("График зависимости максимальных касательных напряжений от средних эффективных напряжений","p`, кПа.","q, кПа.",XData, YData));
        XData.clear();
        YData.clear();


        for (const stepVibro &el : data->steps)
        {
            YData.append(el.m_verticalPressure_kPa);
        }
        XData = convertToN(data);
        savePlotAsSvg(insertGraph("График зависимости напряжения от времени","Время, мин.","Вертикальное напряжение, кПа.",XData, YData),reportDir + "/График зависимости напряжения от времени.svg");
        doc.write(4,5,"=HYPERLINK(\"График зависимости напряжения от времени.svg\", \"График зависимости напряжения от времени\")");
        //doc.insertImage(22,16,insertGraph("График зависимости напряжения от времени","Время, мин.","Вертикальное напряжение, кПа.",XData, YData));
        XData.clear();
        YData.clear();

        double w = getW(&XData,&YData,data);
        doc.write(9,1,"ΔW");
        doc.write(9,2,QString::number(w,'f',6) + " кПа.");
        savePlotAsSvg(insertGraph("График Σ–Ε","Осевая деформация ε", "Девиатор напряжений σ, кПа.",XData,YData),reportDir + "/График Σ–Ε.svg");
        doc.write(5,5,"=HYPERLINK(\"График Σ–Ε.svg\", \"График Σ–Ε\")");
        //doc.insertImage(43,5, insertGraph("График Σ–Ε","Осевая деформация ε", "Девиатор напряжений σ, кПа.",XData,YData));


        QXlsx::RichString r;
        QXlsx::Format normal;
        QXlsx::Format sub;
        QXlsx::Format sup;
        sup.setFontScript(QXlsx::Format::FontScriptSuper);
        sub.setFontScript(QXlsx::Format::FontScriptSub);


        double moduleDeform;
        savePlotAsSvg(getModulsDeforms(data, &moduleDeform, true),reportDir + "/Eyd.svg");
        doc.write(7,5,"=HYPERLINK(\"Eyd.svg\", \"Eyd.svg\")");
        // doc.insertImage(64,5, getModulsDeforms(data, &moduleDeform, true));
        r.addFragment("E", normal);
        r.addFragment("y",sup);
        r.addFragment("d",sub);
        doc.write(7,1,r);
        doc.write(7,2,QString::number(moduleDeform) + " МПа.");

        savePlotAsSvg(getModulsDeforms(data, &moduleDeform, false),reportDir + "/Ed.svg");
        doc.write(8,5,"=HYPERLINK(\"Ed.svg\", \"Ed\")");
        // doc.insertImage(64,16, getModulsDeforms(data, &moduleDeform, false));
        r = QXlsx::RichString();
        r.addFragment("E", normal);
        r.addFragment("d",sub);
        doc.write(8,1,r);
        doc.write(8,2,QString::number(moduleDeform) + " МПа.");


        doc.saveAs(reportDir + "/report.xlsx");
        QDesktopServices::openUrl(QUrl::fromLocalFile(reportDir + "/report.xlsx"));
    }


}

int Report::getY(int y)
{
    return height - y;
}
