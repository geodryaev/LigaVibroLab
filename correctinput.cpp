#include <ui_correctinput.h>
#include <QPen>
#include <QThread>
#include "stepvibro.h"
#include "correctinput.h"

correctInput::correctInput(vibroData * data, const double max, const double min, const double freq, QWidget *parent)
    : QDialog(parent),
    data(data),
    ui(new Ui::correctInput)
{
    ui->setupUi(this);
    ui->min->setValue(min);
    ui->max->setValue(max);
    ui->freq->setValue(freq);
    ui->sinPosGorizont->setStyleSheet(R"(QSlider::sub-page:horizontal {background: transparent;})");
    ui->sinPosGorizont->hide();
    ui->autoAdjustmen->hide();
    ui->sinPosGorizont->setMinimum(-1000);
    ui->sinPosGorizont->setMaximum(1000);
    ui->sinPosGorizont->setValue(0);
    ui->progressBar->setMaximum(static_cast<int>(M_PI * 2 * 100));
    ui->progressBar->hide();

    worker = new autoAdjustment();
    plot = new QwtPlot(this);
    sineCurv = new QwtPlotCurve("");
    sineCurv->setPen(QPen(Qt::blue,3));

    picker = new QwtPlotPicker(plot->canvas());

    connect(this,&correctInput::getMainGraph,worker,&autoAdjustment::getMainGraph);
    connect(picker, qOverload<const QPointF &>(&QwtPlotPicker::selected),this, &correctInput::onPointClick);
    connect(this,&correctInput::getDataForStencil,worker,&autoAdjustment::getDataForStencil);
    connect(ui->cheack,&QRadioButton::clicked,this,&correctInput::addSineStencil);
    connect(worker,&autoAdjustment::complateSinTemplates,this,&correctInput::reloadSinTemplates);
    connect(worker,&autoAdjustment::complateMainGraph,this,&correctInput::paintMainGraph);
    connect(ui->autoAdjustmen,&QPushButton::clicked,&autoAdjustment::process);

    plot->setTitle("График нагрузки");
    plot->setCanvasBackground(Qt::white);
    plot->setAxisTitle(QwtPlot::xBottom, "Время, мин.");
    plot->setAxisTitle(QwtPlot::yLeft, "Усилие, кПа.");



    picker->setStateMachine(new QwtPickerClickPointMachine());
    picker->setMousePattern(QwtEventPattern::MouseSelect1, Qt::RightButton);
    picker->setTrackerMode(QwtPicker::AlwaysOn);
    picker->setRubberBand(QwtPicker::CrossRubberBand);
    picker->setEnabled(true);

    picker->setTrackerPen(QPen(Qt::black));

    emit getMainGraph(*data,&minValX,&maxValX,&minValY,&maxValY);



}

correctInput::~correctInput()
{
    delete ui;
}

void correctInput::addSineStencil(bool checked)
{
    double min = ui->min->value();
    double max = ui->max->value();
    double freq = ui->freq->value();
    pointsTemplatesGraph = {};

    if (checked)
    {
        ui->sinPosGorizont->show();
        ui->autoAdjustmen->show();
        ui->max->hide();
        ui->min->hide();
        ui->freq->hide();
        ui->label_4->hide();
        ui->label_5->hide();
        ui->label_6->hide();
        sineCurv->attach(plot);
        emit getDataForStencil(&pointsTemplatesGraph,min,max,freq, phi);
    }
    else
    {
        ui->sinPosGorizont->hide();
        ui->autoAdjustmen->hide();
        ui->max->show();
        ui->min->show();
        ui->freq->show();
        ui->label_4->show();
        ui->label_5->show();
        ui->label_6->show();

        sineCurv->detach();
        plot->replot();
    }
}

void correctInput::changeValueHorisontal(int value)
{
    phi = M_PI * value / 1000;
    sineCurv->detach();
    plot->replot();
    addSineStencil(true);
}

void correctInput::onPointClick(const QPointF &point)
{

    if (selectPoint.size() < 2)
    {
        selectPoint.append(point);
        qDebug() << "Точка выбрана:" << point;

        if (selectPoint.size() == 2) {
            double x1 = selectPoint[0].x();
            double x2 = selectPoint[1].x();
            qDebug() << "Выделенный интервал X: от" << x1 << "до" << x2;

        }
    }
    else
    {
        selectPoint.clear();
        selectPoint.append(point);
        qDebug() << "Сброс. Новая точка:" << point;
        for (QwtPlotMarker* &m : selectedMarkers) {
            m->detach();   // убрать с графика
            delete m;      // освободить память
        }
        selectedMarkers.clear();
        plot->replot();
    }
    QwtPlotMarker *marker = new QwtPlotMarker();
    marker->setValue(point);
    marker->setSymbol(new QwtSymbol(QwtSymbol::Ellipse,
                                    QBrush(Qt::black),
                                    QPen(Qt::black),
                                    QSize(8, 8)));
    marker->attach(plot);

    selectedMarkers.append(marker);  // сохраняем маркер
    plot->replot();
}

void correctInput::paintMainGraph(QVector<QPointF> points)
{
    QwtPlotCurve *curv = new QwtPlotCurve("");
    QwtPlotGrid *grid = new QwtPlotGrid();

    plot->setAxisScale(QwtPlot::xBottom, minValX, maxValX);
    plot->setAxisScale(QwtPlot::yLeft, minValY, maxValY);
    curv->setSamples(points);
    curv->setPen(QPen(Qt::red,3));
    curv->attach(plot);

    grid->setMajorPen(QPen(Qt::lightGray, 1, Qt::DashLine));
    grid->setMinorPen(QPen(Qt::gray, 1, Qt::DotLine));
    grid->attach(plot);
    QwtPlotZoomer * zoom = new QwtPlotZoomer(plot->canvas());
    zoom->setRubberBand(QwtPicker::RectRubberBand);
    zoom->setRubberBandPen(QColor(Qt::blue));
    zoom->setTrackerMode(QwtPicker::AlwaysOn);
    zoom->setTrackerPen(QColor(Qt::black));
    zoom->setMousePattern(QwtEventPattern::MouseSelect3,Qt::MiddleButton);
    zoom->setMousePattern(QwtEventPattern::MouseSelect2,
                          Qt::RightButton,
                          Qt::ControlModifier);

    plot->resize(800,600);
    plot->replot();
    if (!ui->widget->layout()) {
        QVBoxLayout *layout = new QVBoxLayout(ui->widget);
        layout->addWidget(plot);
    } else {
        ui->widget->layout()->addWidget(plot);
    }
    zoom->setZoomBase();
}

void correctInput::on_pushButton_clicked()
{
    if (selectedMarkers.size() != 2 and selectedMarkers.size() != 0)
    {
        this->close();
        return;
    }

    if (selectedMarkers.size() != 0)
    {
        qreal startTime = std::min(selectedMarkers[0]->value().x(), selectedMarkers[1]->value().x());
        qreal stopTime  = std::max(selectedMarkers[0]->value().x(), selectedMarkers[1]->value().x());

        int startIndex = -1;
        int endIndex = -1;

        for (int i = 0; i < data->steps.size(); ++i)
        {
            if (qAbs(data->steps[i].m_time - startTime) < 0.002f)
                startIndex = i;
            if (qAbs(data->steps[i].m_time - stopTime) < 0.002f)
                endIndex = i;
        }

        if (startIndex == -1 || endIndex == -1 || startIndex >= endIndex)
        {
            qWarning() << "Не удалось найти границы отрезка!";
            this->close();
            return;
        }


        data->steps.erase(data->steps.begin(), data->steps.begin() + startIndex);
        data->steps.erase(data->steps.begin() + (endIndex - startIndex + 1), data->steps.end());
    }
    accept();
}

void correctInput::on_autoAdjustmen_clicked()
{
    // QThread *thread = new QThread ();
    // autoAdjustment * worker = new autoAdjuystment(this);
    // worker->moveToThread(thread);


    ui->sinPosGorizont->hide();
    ui->autoAdjustmen->hide();
    ui->progressBar->show();
    ui->progressBar->setValue(0);

    // connect(thread, &QThread::started,worker, &autoAdjustment::process);
    // connect(worker, &autoAdjuystment::finish,thread,&QThread::quit);
    // connect(worker, &autoAdjuystment::tick,this,&correctInput::addProgrssBar);
    // connect(worker, &autoAdjuystment::reloadGraph,this,&correctInput::graphReload);

    // thread->start();

    ui->progressBar->hide();
}

void correctInput::addProgrssBar()
{
    ui->progressBar->setValue(ui->progressBar->value()+1);
}

void correctInput::graphReload(const QVector<QPointF> & curvData)
{

    sineCurv->setSamples(curvData);
    plot->replot();
}

void correctInput::reloadSinTemplates()
{
    sineCurv->setSamples(pointsTemplatesGraph);
    plot->replot();
}

void correctInput::getDataMainGraph(QVector<QPointF> points)
{
    double minValX;
    double maxValX;
    double minValY;
    double maxValY;
}

void autoAdjustment::getMainGraph(vibroData &data,double * minX, double * maxX, double * minY, double * maxY)
{
    QVector<double>XData;
    QVector<double>YData;
    QVector<QPointF> points;

    for (const stepVibro &el : std::as_const(data.steps))
    {
        points.append(QPointF(el.m_time, el.m_verticalPressure_kPa));
        YData.append(el.m_verticalPressure_kPa);
        XData.append(el.m_time);
    }
    *minX = *std::min_element(XData.begin(), XData.end());
    *maxX = *std::max_element(XData.begin(), XData.end());
    *minY = *std::min_element(YData.begin(), YData.end());
    *maxY = *std::max_element(YData.begin(), YData.end());
    m_minX= *minX;
    m_maxX= *maxX;

    emit complateMainGraph(points);

}

void autoAdjustment::getDataForStencil(QVector<QPointF> *curvData, double minPress, double maxPress, double frequency, double phi)
{
    curvData->clear();
    double ampl = (maxPress-minPress)/2;
    double yOffset = minPress + ampl;
    double dt = 0.0001;
    double y;

    for (double x = m_minX; x < m_maxX; x+=dt)
    {
        y = ampl * std::sin(2 * M_PI * frequency * x * 60 + phi) + yOffset;
        //pointsTemplatesGraph.append();
        curvData->append(QPointF(x,y));
    }
    emit complateSinTemplates();
}

void autoAdjustment::process(QVector<QPointF> * d,double min, double max,double freq, double phi)
{
    unsigned long long minDistance = LLONG_MAX;
    unsigned long long currentDistance;
    double phaseShift;

    for (double i = -M_PI; i <= M_PI; i+=0.01)
    {
        transformSinToRealData(i);
        currentDistance = errorMetrick();
        if (minDistance > currentDistance)
        {
            minDistance = currentDistance;
            phaseShift = i;
        }
        // emit tick();
    }

    transformSinToRealData(phaseShift);
    // emit reloadGraph();

}

unsigned long long autoAdjustment::errorMetrick()
{
    unsigned long long distance = 0;
    // qDebug() << "Real\t--\tVirtual";
    for (int i = 0; i < data->steps.size(); i++)
    {
        distance += std::abs(data->steps[i].m_verticalPressure_kPa - pointsTemplatesGraph[i].y());
        //qDebug() << data->steps[i].m_time << "\t--\t" << pointsTemplatesGraph[i].x();
    }
    return distance;
}

//Вот тут поменять надо на нормально и все, так же сделать тут проход
void autoAdjustment::transformSinToRealData(double a)
{
    double ampl = (max-min)/2;
    double yOffset = min + ampl;

    pointsTemplatesGraph.clear();
    for(auto  it : data->steps)
    {
        //pointsTemplatesGraph.append(QPointF(it.m_time, ampl * std::sin(2 * M_PI * ui->freq->value() * it.m_time * 60 + a) + yOffset));
    }
}
