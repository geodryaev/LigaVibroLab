#include <ui_correctinput.h>
#include <QPen>

#include "stepvibro.h"
#include "correctinput.h"

correctInput::correctInput(vibroData * data, const double max, const double min, const double freq, QWidget *parent)
    : QDialog(parent),
    max(max),
    min(min),
    freq(freq),
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

    connect(ui->cheack,&QRadioButton::toggled,this,&correctInput::addSineStencil);
    connect(ui->sinPosGorizont,&QSlider::valueChanged, this, &correctInput::changeValueHorisontal);

    sineCurv = new QwtPlotCurve();
    sineCurv->setPen(QPen(Qt::blue, 4, Qt::DashLine));
    QVector<double>XData;
    QVector<double>YData;

    for (const stepVibro &el : std::as_const(data->steps))
    {
        YData.append(el.m_verticalPressure_kPa);
        XData.append(el.m_time);
    }

        plot = new QwtPlot(this);
        double minValX = *std::min_element(XData.begin(), XData.end());
        double maxValX = *std::max_element(XData.begin(), XData.end());
        minX= minValX;
        maxX= maxValX;

        double minValY = *std::min_element(YData.begin(), YData.end());
        double maxValY = *std::max_element(YData.begin(), YData.end());
        plot->setTitle("График нагрузки");
        plot->setCanvasBackground(Qt::white);
        plot->setAxisTitle(QwtPlot::xBottom, "Время, мин.");
        plot->setAxisTitle(QwtPlot::yLeft, "Усилие, кПа.");
        plot->setAxisScale(QwtPlot::xBottom, minValX, maxValX);
        plot->setAxisScale(QwtPlot::yLeft, minValY, maxValY);

        QwtPlotCurve *curv = new QwtPlotCurve("");
        curv->setSamples(XData, YData);
        curv->setPen(QPen(Qt::red,3));
        curv->attach(plot);

        QwtPlotGrid *grid = new QwtPlotGrid();
        grid->setMajorPen(QPen(Qt::lightGray, 1, Qt::DashLine));
        grid->setMinorPen(QPen(Qt::gray, 1, Qt::DotLine));
        grid->attach(plot);

        QwtPlotZoomer * zoom = new QwtPlotZoomer(plot->canvas());
        zoom->setRubberBand(QwtPicker::RectRubberBand);
        zoom->setRubberBandPen(QColor(Qt::blue));
        zoom->setTrackerMode(QwtPicker::AlwaysOn);
        zoom->setTrackerPen(QColor(Qt::black));
        zoom->setMousePattern(QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier);

        picker = new QwtPlotPicker(plot->canvas());
        picker->setStateMachine(new QwtPickerClickPointMachine());
        picker->setMousePattern(QwtEventPattern::MouseSelect1, Qt::RightButton);
        picker->setTrackerMode(QwtPicker::AlwaysOn);
        picker->setRubberBand(QwtPicker::CrossRubberBand);
        picker->setEnabled(true);

        picker->setTrackerPen(QPen(Qt::black));

        connect(picker, SIGNAL(selected(QPointF)), this, SLOT(onPointClick(QPointF)));

        plot->resize(800,600);
        plot->replot();
        if (!ui->widget->layout()) {
            QVBoxLayout *layout = new QVBoxLayout(ui->widget);
            layout->addWidget(plot);
        } else {
            ui->widget->layout()->addWidget(plot);
        }

}

correctInput::~correctInput()
{
    delete ui;
}

unsigned long long correctInput::errorMetrick()
{
    unsigned long long distance = 0;
    qDebug() << "Real\t--\tVirtual";
    for (int i = 0; i < data->steps.size(); i++)
    {
        distance += std::abs(data->steps[i].m_verticalPressure_kPa - pointsTemplatesGraph[i].y());
        //qDebug() << data->steps[i].m_time << "\t--\t" << pointsTemplatesGraph[i].x();
    }
    return distance;
}

void correctInput::transformSinToRealData(double a)
{
    double ampl = (max-min)/2;
    double yOffset = min + ampl;

    pointsTemplatesGraph.clear();
    for(auto  it : data->steps)
    {
        pointsTemplatesGraph.append(QPointF(it.m_time, ampl * std::sin(2 * M_PI * ui->freq->value() * it.m_time * 60 + a) + yOffset));
    }
}

void correctInput::addSineStencil(bool checked)
{
    min = ui->min->value();
    max = ui->max->value();
    freq = ui->freq->value();

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

        pointsTemplatesGraph.clear();
        double ampl = (max-min)/2;
        double yOffset = min + ampl;
        double dt = 0.0001;
        double y;


        for (double x = minX; x < maxX; x+=dt)
        {
            y = ampl * std::sin(2 * M_PI * ui->freq->value() * x * 60 + phi) + yOffset;
            pointsTemplatesGraph.append(QPointF(x,y));
        }
        sineCurv->setSamples(pointsTemplatesGraph);
        sineCurv->attach(plot);
        plot->replot();
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
        for (QwtPlotMarker* m : selectedMarkers) {
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

    ui->sinPosGorizont->hide();
    ui->autoAdjustmen->hide();
    ui->progressBar->show();
    ui->progressBar->setValue(0);


    ui->progressBar->hide();
}

void correctInput::addProgrssBar()
{
    ui->progressBar->setValue(ui->progressBar->value()+1);
}

void autoAdjuystment::process()
{
    unsigned long long minDistance = LLONG_MAX;
    unsigned long long currentDistance;
    double phaseShift;

    for (double i = -M_PI; i <= M_PI; i+=0.01)
    {
        m_owner->transformSinToRealData(i);
        currentDistance = m_owner->errorMetrick();
        if (minDistance > currentDistance)
        {
            minDistance = currentDistance;
            phaseShift = i;
        }
        emit tick();
    }

    m_owner->transformSinToRealData(phaseShift);
    emit reloadGraph();

}
