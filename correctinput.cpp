#include <ui_correctinput.h>
#include <QPen>
#include <QThread>
#include <QTimer>
#include "stepvibro.h"
#include "correctinput.h"

correctInput::correctInput(vibroData * _data, const double max, const double min, const double freq, QWidget *parent)
    : QDialog(parent),
    data(_data),
    ui(new Ui::correctInput)
{
    ui->setupUi(this);
    ui->min->setValue(min);
    ui->max->setValue(max);
    ui->freq->setValue(freq);
    ui->autoAdjustmen->hide();
    ui->progressBar->setMaximum(static_cast<int>(M_PI * 2 * 100));
    ui->progressBar->hide();
    ui->editMainGraph->hide();

    plot = new QwtPlot(this);
    sineCurv = new QwtPlotCurve("");
    sineCurv->setPen(QPen(Qt::blue,3));
    picker = new QwtPlotPicker(plot->canvas());
    grid = new QwtPlotGrid();
    zoom = new QwtPlotZoomer(plot->canvas());
    curv = new QwtPlotCurve("");
    worker = new autoAdjustment(data);
    thread = new QThread (this);
    worker->moveToThread(thread);

    connect(this,&correctInput::getMainGraph,worker,&autoAdjustment::getMainGraph, Qt::QueuedConnection );
    connect(picker, qOverload<const QPointF &>(&QwtPlotPicker::selected),this, &correctInput::onPointClick);
    connect(this,&correctInput::getDataForStencil,worker,&autoAdjustment::getDataForStencil);
    connect(ui->cheack,&QRadioButton::clicked,this,&correctInput::addSineStencil);
    connect(worker,&autoAdjustment::complateSinTemplates,this,&correctInput::reloadSinTemplates);
    connect(worker,&autoAdjustment::complateMainGraph,this,&correctInput::paintMainGraph);
    connect(ui->autoAdjustmen, &QPushButton::clicked,this, &correctInput::sendProcess);
    connect(this,&correctInput::sendProcessSignals, worker,&autoAdjustment::process);
    connect(worker, &autoAdjustment::tick, this, &correctInput::paintTemplateGraph);
    connect(ui->editMainGraph, &QPushButton::clicked, this, [this](bool ev){
        emit smoothMainGraph(&pointsMainGraph, &pointsTemplatesGraph);
    });
    connect(this, &correctInput::smoothMainGraph,worker, &autoAdjustment::smoothMainGraph);
    connect(worker,&autoAdjustment::reloadMainGraph, this, &correctInput::reloadMainGraph);
    thread->start();


    connect(thread, &QThread::started, [](){

        qDebug() << Q_FUNC_INFO;
    });



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

    QTimer::singleShot(1, this, [this]() {
        emit getMainGraph(&minValX,&maxValX,&minValY,&maxValY);
    });

}

correctInput::~correctInput()
{
    thread->requestInterruption();
    thread->quit();
    thread->wait();
    delete ui;
}

void correctInput::sendProcess(bool ev)
{
    emit sendProcessSignals(&pointsMainGraph, &pointsTemplatesGraph,minSinTemp,maxSinTemp,freqSinTemp);
    ui->autoAdjustmen->hide();
    ui->editMainGraph->show();
}

void correctInput::addSineStencil(bool checked)
{
    minSinTemp  = ui->min->value();
    maxSinTemp = ui->max->value();
    freqSinTemp = ui->freq->value();
    pointsTemplatesGraph = {};

    if (checked)
    {
        ui->autoAdjustmen->show();
        ui->max->hide();
        ui->min->hide();
        ui->freq->hide();
        ui->label_4->hide();
        ui->label_5->hide();
        ui->label_6->hide();
        sineCurv->attach(plot);
        emit getDataForStencil(&pointsTemplatesGraph,minSinTemp,maxSinTemp,freqSinTemp, phi);
    }
    else
    {
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

void correctInput::paintMainGraph(QVector<QPointF> * points)
{
    pointsMainGraph = *points;

    plot->setAxisScale(QwtPlot::xBottom, minValX, maxValX);
    plot->setAxisScale(QwtPlot::yLeft, minValY, maxValY);
    curv->setSamples(*points);
    curv->setPen(QPen(Qt::red,3));
    curv->attach(plot);

    grid->setMajorPen(QPen(Qt::lightGray, 1, Qt::DashLine));
    grid->setMinorPen(QPen(Qt::gray, 1, Qt::DotLine));
    grid->attach(plot);

    zoom->setRubberBand(QwtPicker::RectRubberBand);
    zoom->setRubberBandPen (QColor(Qt::blue));
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

void correctInput::paintTemplateGraph()
{
    sineCurv->setSamples(pointsTemplatesGraph);
    plot->replot();
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

void correctInput::reloadMainGraph()
{
    ui->editMainGraph->hide();
    sineCurv->detach();
    curv->setSamples(pointsMainGraph);
    plot->replot();
    for (int i = 0; i < pointsMainGraph.length(); i++)
    {
        data->steps[i].m_verticalPressure_kPa = pointsTemplatesGraph[i].y();
    }
}

void autoAdjustment::getMainGraph(double *minX, double * maxX, double * minY, double * maxY)
{
    QVector<double>XData;
    QVector<double>YData;
    QVector<QPointF> * points = new QVector<QPointF>();

    for (const stepVibro &el : std::as_const(data->steps))
    {
        points->append(QPointF(el.m_time, el.m_verticalPressure_kPa));
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

void autoAdjustment::smoothMainGraph(QVector<QPointF> *d, QVector<QPointF> *temp)
{
    double s = 1;
    QVector<QPointF> smootGraph = *d;
    for (int i = 0; i < temp->size(); i++)
    {
        smootGraph[i].setY(smootGraph[i].y() + s*((*temp)[i].y() -smootGraph[i].y()));
    }
    *d = std::move(smootGraph);
    emit reloadMainGraph();
}

void autoAdjustment::smoothTemplateGraph(QVector<QPointF> *p)
{
    QVector<QPointF>::Iterator it = p->begin();
    QVector<QPointF>::Iterator begin = p->begin();
    QVector<QPointF>::Iterator end = p->begin();

    double middle = (max + min)/2;
    bool state = upToFirstPoint(&it);
    while (it != p->end())
    {
        begin = it;
        end = nextPoint(begin,p->end(),state);
        it = end;

        double target = summOffset(begin,end,p,state);
        movePoints(begin,end,p,target,state);
        state = !state;
    }
}

//true навверх
//false вниз
bool autoAdjustment::upToFirstPoint(QVector<QPointF>::Iterator *it)
{
    double middle = (max + min)/2.0;
    if ((*it)->y() == middle)
    {
        if ((*(it)+1)->y() > middle)
            return true;
    }
    else
    {
        return false;
    }

    if ((*it)->y() < middle)
    {
        while((*it)->y() < middle)
        {
            ++(*it);
        }
        return true;
    }

    if ((*it)->y() > middle)
    {
        while((*it)->y() > middle)
        {
            ++(*it);
        }
        return false;
    }

    return false;
}

QVector<QPointF>::Iterator autoAdjustment::nextPoint(QVector<QPointF>::Iterator start, QVector<QPointF>::Iterator end, bool state)
{
    const double middle = (max + min)/2;

    if (state)
    {
        while (start != end && start->y() >= middle)
        {
            start++;
        }
        return start;
    }
    else
    {
        while (start != end && start->y() <= middle)
        {
            start++;
        }
        return start;
    }
}

// Тут по одной точек , можно попробовать по 3, хз
double autoAdjustment::summOffset(QVector<QPointF>::Iterator s1, QVector<QPointF>::Iterator s2, QVector<QPointF> *p, bool state)
{
    double valueEdge = (max+min)/2;
    if (state)
    {
        for(int i = std::distance(p->begin(), s1); i < std::distance(p->begin(),s2);i++)
        {
            if (valueEdge < data->steps[i].m_verticalPressure_kPa)
                valueEdge = data->steps[i].m_verticalPressure_kPa;
        }
    }
    else
    {
        for(int i = std::distance(p->begin(), s1); i < std::distance(p->begin(),s2);i++)
        {
            if (valueEdge > data->steps[i].m_verticalPressure_kPa)
                valueEdge = data->steps[i].m_verticalPressure_kPa;
        }
    }

    return std::abs(valueEdge - ((max-min)/2 + min));
}

void autoAdjustment::movePoints(QVector<QPointF>::Iterator s1, QVector<QPointF>::Iterator s2, QVector<QPointF> *p, double target, bool state)
{
    double shiftZerro = s1->x();
    if (s1!= p->begin())
    {
        s1--;
    }
    if (state)
    {
        for (auto it = s1; it != s2; it++)
        {
            it->setY(sin((it->x()-shiftZerro) * 2*M_PI * freq  * 60) * target + (max - min)/2 + min);
        }
    }
    else
    {
        for (auto it = s1; it != s2; it++)
        {
            it->setY(-1 * sin((it->x()-shiftZerro) * 2*M_PI * freq  * 60) * target + (max - min)/2 + min);
        }
    }
}

void autoAdjustment::process(QVector<QPointF> * d, QVector<QPointF> * sinTemplate, double min, double max,double freq)
{
    this->max = max;
    this->min = min;
    this->freq = freq;
    p_pointsTemplatesGraph = *sinTemplate;
    pointsMainGraph = *d;
    unsigned long long minDistance = LLONG_MAX;
    unsigned long long currentDistance;
    double phaseShift;

    for (int i = -M_PI*100; i <= M_PI*100; i++)
    {
        transformSinToRealData(i/100.0);
        currentDistance = errorMetrick();
        if (minDistance > currentDistance)
        {
            minDistance = currentDistance;
            phaseShift = i/100.0;
        }
        *sinTemplate = std::move(p_pointsTemplatesGraph);
        emit tick();
    }

    transformSinToRealData(phaseShift);
    smoothTemplateGraph(&p_pointsTemplatesGraph);
    *sinTemplate = std::move(p_pointsTemplatesGraph);
    emit tick();
}

unsigned long long autoAdjustment::errorMetrick()
{
    unsigned long long distance = 0;
    for (int i = 0; i < data->steps.size(); i++)
    {
        distance += std::abs(data->steps[i].m_verticalPressure_kPa - p_pointsTemplatesGraph[i].y());
    }
    return distance;
}

//Вот тут поменять надо на нормально и все, так же сделать тут проход
void autoAdjustment::transformSinToRealData(double a)
{
    double ampl = (max-min)/2;
    double yOffset = min + ampl;

    p_pointsTemplatesGraph.clear();
    for(auto  it : data->steps)
    {
        p_pointsTemplatesGraph.append(QPointF(it.m_time, ampl * std::sin(2 * M_PI * freq * it.m_time * 60 + a) + yOffset));
    }
}

void correctInput::on_pushButton_clicked()
{
    if (selectedMarkers.size() == 2)
    {
        QwtPlotMarker * o1;
        QwtPlotMarker * o2;
        if (selectedMarkers[1]->xValue() > selectedMarkers[0]->xValue())
        {
            o1 = selectedMarkers[0];
            o2 = selectedMarkers[1];
        }
        else
        {
            o1 = selectedMarkers[1];
            o2 = selectedMarkers[0];
        }

        // auto left = std::lower_bound(data->steps.begin(),data->steps.end(),o1->xValue(),[](const stepVibro &p, double x){
        //     return p.m_time < x;
        // });
        // auto right = std::lower_bound(data->steps.begin(),data->steps.end(),o2->xValue(),[](const stepVibro &p, double x){
        //     return p.m_time < x;
        // });

        QVector<stepVibro> v;
        for (int i = 0; i < data->steps.size(); i++)
        {
            if (data->steps[i].m_time >= o1->xValue() && data->steps[i].m_time <= o2->xValue())
            {
                v.push_back(data->steps[i]);
            }
            if (o2->xValue() < data->steps[i].m_time)
                break;
        }
        if (v.size() != 0)
        {
            data->steps = std::move(v);
        }
    }
    accept();
}

