#include "supportmodul.h"
#include "QVector"
#include <QPointer>
#include <qpainter.h>
#include <qpushbutton.h>
#include "ui_supportmodul.h"

#include "QSettings"
#include <QPen>
#include "qwt/qwt_plot_canvas.h"
#include <qwt/qwt_scale_widget.h>
#include "qwt/qwt_text.h"
#include "qwt/qwt_plot_curve.h"
#include <qwt/qwt_plot_canvas.h>



supportmodul::supportmodul(bool def, int countCicle, const vibroData* data_, QWidget *parent)
    : QDialog(parent),
    ui(new Ui::supportmodul),
    data(data_),
    choice(def), //true -> вычисляем упругость
    countPoints (countCicle)
{
    ui->setupUi(this);
    ui->selectCicle->setMaximum(data->minPoints.size());
    ui->selectCicle->setMinimum(2);

    d_plot = new QwtPlot (nullptr);
    QVBoxLayout *layout = new QVBoxLayout (ui->graph);
    layout->addWidget(d_plot);
    int count = 0;

        bool start = true;

    if(choice)
    {
        ui->label->setText("Вычисление динамического модуля упругости");
        ui->value->setText("E<sup>y</sup><sub>d</sub> = 0 МПа.");
        setWindowTitle("Вычисление динамического модуля упругости");
        ui->selectCicle->setValue(6);
        if (data->minPoints.size() > 10)
        {
            ui->selectCicle->setMaximum(10);
        }
        else
        {
            ui->selectCicle->setMaximum(data->minPoints.size());
        }

        d_plot->setTitle("Определение динамического модуля упругости");
        for (int i = 0; i < data->steps.size() && count < countCicle+1; i++)
        {
            if (data_->steps[i].isDown)
                count++;
            if (start)
            {
                vX.push_back(data_->steps[i].epsilon_ * 1000);
                vY.push_back(data_->steps[i].sigma1_);
            }
            else
            {
                if (count > 2)
                    start = true;
            }
        }
    }
    else
    {
        d_plot->setTitle("Определение динамического модуля деформации");
        ui->label->setText("Вычисление динамического модуля деформации");
        setWindowTitle("Вычисление динамического модуля деформации");
        ui->selectCicle->setMinimum(0);
        ui->selectCicle->setValue(0);
        for (int i = 0; i < data->steps.size(); i++)
        {
            if (data_->steps[i].isDown)
                count++;
            if (start)
            {
                vX.push_back(data_->steps[i].epsilon_ * 1000);
                vY.push_back(data_->steps[i].sigma1_);
            }
            else
            {
                if (count > 2)
                    start = true;
            }
        }
    }

    d_plot->setAxisTitle(QwtAxis::YLeft, QwtText("Осевое напряжение Δσ′<sub>1</sub>, кПа."));
    d_plot->setAxisTitle(QwtAxis::XBottom, QwtText("Осевая деформация Δe*10<sup>-3</sup>, д.е."));

    connect(ui->selectCicle,&QSpinBox::valueChanged,this,&supportmodul::changeRange);


    QwtPlotCurve *curv = new QwtPlotCurve (QwtPlotCurve("e(t)"));
    curv->setSamples(vX,vY);
    curv->attach(d_plot);

    downDistance = *std::min_element(vY.begin(),vY.end()) - ((*std::max_element(vY.begin(),vY.end()) - *std::min_element(vY.begin(),vY.end())) * 0.1);
    upDistance = *std::max_element(vX.begin(),vX.end()) - *std::min_element(vX.begin(), vX.end());

    grid.setMajorPen(QPen(Qt::gray,1,Qt::DashLine));
    grid.setMinorPen(QPen(Qt::gray,1,Qt::DashLine));
    grid.attach(d_plot);

    if (choice)
    {
        for (int i  = 0 ; i < ui->selectCicle->value(); i++)
        {
            auto btn = new QPushButton();
            vecButton.append(btn);
            btn->setMinimumSize(25,25);
            btn->setText(QString::number(i+1));
            ui->dinamicPanel->addWidget(btn);
            connect(btn,&QPushButton::clicked,this,&supportmodul::processingModileDeform);
        }
    }
    else
    {
        changeRange(0);
    }

    d_plot->replot();
}

supportmodul::~supportmodul()
{
    qDebug()<< "Я умер";
}

QImage * supportmodul::getImage()
{
    return img;
}

double supportmodul::getModule()
{
    if (choice)
    {
        return dinamicModElastic;
    }
    else
    {
        return dinamicModDifform;
    }
}

void supportmodul::processingModileDeform(bool checked)
{
    QPushButton * btn = qobject_cast<QPushButton*>(sender());
    QPointF point1;
    QPointF point2;
    QPointF point3;

    bool down = true;
    int numberCicle = btn->text().toInt()-1;
    int count = 0;

    if (choice)
    {
        for (int i = 0; i < data->steps.size() && count < numberCicle + 3; i++)
        {
            if (data->steps[i].isDown)
            {
                if (count == numberCicle + 1)
                {
                    point1.setX( data->steps[i].epsilon_ * 1000);
                    point1.setY( data->steps[i].sigma1_);
                    down = true;
                }
                if (count == numberCicle + 2)
                {
                    point3.setX( data->steps[i].epsilon_ * 1000);
                    point3.setY(data->steps[i].sigma1_);
                }
                count++;
            }
            if ( down && data->steps[i].isUp)
            {
                point2.setX(data->steps[i].epsilon_ * 1000);
                point2.setY(data->steps[i].sigma1_);
                down = false;
            }
        }

        QVector<double> xs;
        QVector<double> ys;

        xs << point1.x()<< point2.x();
        ys << point1.y()<< point2.y();

        clear();

        supCurv.append(new QwtPlotCurve ("y = kx + b"));
        supCurv[supCurv.size()-1]->setSamples(xs,ys);
        supCurv[supCurv.size()-1]->setPen(Qt::red,2);
        supCurv[supCurv.size()-1]->attach(d_plot);

        xs.clear();
        ys.clear();


        vector(point1.x(),downDistance,point2.x(),downDistance);
        setText(1,"Δε<sub>c</sub>",point1.x(),downDistance,point2.x(),downDistance);

        vector(point2.x(),point2.y(), point2.x(), point1.y());
        setText(3,"Δσ′",point2.x(),point2.y(), point2.x(), point1.x());

        vector(point3.x(),point3.y(),point2.x(),point3.y());
        setText(1,"Δε<sub>y</sub>",point3.x(),point3.y(),point2.x(),point3.y());

        dinamicModElastic = (point2.y()-point1.y()) /(point2.x()-point3.x());
        ui->value->setText("E<sup>y</sup><sub>d</sub> = " + QString::number(dinamicModElastic) + " МПа.");
    }

    d_plot->replot();
}

void supportmodul::setLine(double x1, double y1, double x2, double y2)
{
    QVector<double> xs;
    QVector<double> ys;

    supCurv.append(new QwtPlotCurve());

    xs << x1 << x2;
    ys <<y1 << y2 ;
    supCurv[supCurv.size()-1]->setSamples(xs,ys);
    supCurv[supCurv.size()-1]->setPen(Qt::red,2);
    supCurv[supCurv.size()-1]->attach(d_plot);
}

void supportmodul::clear()
{

    for(auto el : supCurv)
    {
        el->detach();
        delete(el);
    }

    for(auto el : supMarker)
    {
        if (el != nullptr)
        {
            el->detach();
            delete(el);
        }

    }
    supMarker.clear();
    supCurv.clear();
}

void supportmodul::vector(double x1, double y1, double x2, double y2)
{
    QVector<double> xs;
    QVector<double> ys;

    xs << x1 << x2;
    ys << y1 << y2;

    if (y1 == y2)
    {
        double lenX = abs(x2-x1) * 0.05;
        double lenY = (*std::max_element(vY.begin(),vY.end()) - *std::min_element(vY.begin(),vY.end()))*0.01;
        setLine(x1,y1,x1+lenX,y1+lenY);
        setLine(x1,y1,x1+lenX,y1-lenY);
        setLine(x2,y2,x2-lenX,y2-lenY);
        setLine(x2,y2,x2-lenX,y2+lenY);
    }

    if (x1 == x2)
    {
        double lenX = abs(y2-y1) * 0.05;

        setLine(x1,y1,x1+upDistance*0.01,y1-lenX);
        setLine(x1,y1,x1-upDistance*0.01,y1-lenX);
        setLine(x2,y2,x2+upDistance*0.01,y2+lenX);
        setLine(x2,y2,x2-upDistance*0.01,y2+lenX);
    }


    supCurv.append(new QwtPlotCurve ("sigma"));
    supCurv[supCurv.size()-1]->setSamples(xs,ys);
    supCurv[supCurv.size()-1]->setPen(Qt::red,2);
    supCurv[supCurv.size()-1]->attach(d_plot);


}

void supportmodul::setText(int maskPosition, QString text, double x1, double y1, double x2, double y2) // 0 up 1 down 2 left 3 right
{
    if (!(maskPosition > -1 && maskPosition < 4))
    {
        return;
    }
    QwtText qwtText(text);
    double x = (x2+x1)/2;
    double y = (y1+y2)/2;
    double paddingWidth =  *std::max_element(vX.begin(),vX.end()) - *std::min_element(vX.begin(), vX.end());
    double paddingHeight = *std::max_element(vY.begin(),vY.end()) - *std::min_element(vY.begin(), vY.end());

    paddingHeight *= 0.05;
    paddingWidth *= 0.05;

    supMarker.append(new QwtPlotMarker());


    switch (maskPosition) {
    case 0:
        supMarker[supMarker.size()-1]->setValue(x,y + paddingHeight);
        break;
    case 1:
        supMarker[supMarker.size()-1]->setValue(x,y - paddingHeight);
        break;
    case 2:
        supMarker[supMarker.size()-1]->setValue(x - paddingWidth, y);
        supMarker[supMarker.size()-1]->setLabelOrientation(Qt::Vertical);
        break;
    case 3:
        supMarker[supMarker.size()-1]->setValue(x + paddingWidth,y);
        supMarker[supMarker.size()-1]->setLabelOrientation(Qt::Vertical);
        break;
    default:
        return;
        break;
    }

    supMarker[supMarker.size()-1]->setLabel(qwtText);
    supMarker[supMarker.size()-1]->attach(d_plot);
}

void supportmodul::on_buttonBox_accepted()
{
    QSettings settings("settings.ini", QSettings::IniFormat);

    int width  = settings.value("size/width",400).toInt();
    int height = settings.value("size/height",600).toInt();
    int sizeFont = settings.value("size/text",12).toInt();
    double sizeLine = settings.value("size/chart").toDouble();

    QColor colorGraph1(settings.value("color/graph1").toString());
    QColor colorGraph2(settings.value("color/graph2").toString());
    QColor colorFont(settings.value("color/font").toString());
    QColor colorBackround(settings.value("color/backround").toString());
    qDebug() << colorGraph1.name() << "\n" << colorGraph2.name() << "\n" <<
        colorBackround.name() << "\n" << colorFont.name() << "\n" << width << "\n"
             << height << "\n" << sizeLine<< "\n" << sizeFont << "\n";

    qDebug() <<"";

    d_plot->setStyleSheet(QString("background-color: %1").arg(colorBackround.name()));

    QFont titleFont;
    titleFont.setPointSize(sizeFont);

    QwtText titleText = d_plot->title();
    titleText.setFont(titleFont);
    d_plot->setTitle(titleText);

    QFont axisFont;
    axisFont.setPointSize(sizeFont);

    QwtText xTitle = d_plot->axisTitle(QwtPlot::xBottom);
    QwtText yTitle = d_plot->axisTitle(QwtPlot::yLeft);
    xTitle.setFont(axisFont);
    yTitle.setFont(axisFont);
    d_plot->setAxisTitle(QwtPlot::yLeft,yTitle);
    d_plot->setAxisTitle(QwtPlot::xBottom,xTitle);

    d_plot->setAxisScaleDraw(QwtPlot::xBottom, new TickDrawX(axisFont));
    d_plot->setAxisScaleDraw(QwtPlot::yLeft,   new TickDrawY(axisFont));

    QwtScaleWidget *swX = d_plot->axisWidget(QwtPlot::xBottom);
    QwtScaleWidget *swY = d_plot->axisWidget(QwtPlot::yLeft);
    swX->setFont(axisFont);
    swY->setFont(axisFont);
    bool first = 1;
    for (QwtPlotItem *item : d_plot->itemList())
    {
        if (item->rtti() == QwtPlotItem::Rtti_PlotCurve)
        {

            QwtPlotCurve *curve = static_cast<QwtPlotCurve*>(item);

            QPen p = curve->pen();
            p.setWidthF(sizeLine);
            if (first)
            {
                p.setColor(colorGraph1);
                first = 0;
            }
            else
            {
                p.setColor(colorGraph2);
            }
            curve->setPen(p);
        }
    }

    d_plot->setFixedSize(width, height);
    d_plot->replot();

    QImage *img = new QImage(width, height, QImage::Format_ARGB32);
    img->fill(colorBackround);

    QPainter painter(img);
    d_plot->render(&painter);
    painter.end();

    this->img = img;
    accepted();
}

void supportmodul::changeRange(int index)
{
    clear();
    qDeleteAll(vecButton);
    d_plot->detachItems(QwtPlotItem::Rtti_PlotCurve, true);
    d_plot->replot();
    int count = 0;
    bool start = true;

    vX.clear();
    vY.clear();
    if (index == 0 && !choice)
    {
        index = data->steps.size();
    }
    if (index > 0 && index < 3)
    {
        index = 3;
    }
    for (int i = 0; i < data->steps.size() && count < index+1; i++)
    {
        if (data->steps[i].isDown)
            count++;
        if (start)
        {
            vX.push_back(data->steps[i].epsilon_ * 1000);
            vY.push_back(data->steps[i].sigma1_);
        }
        else
        {
            if (count > 2)
                start = true;
        }
    }
    QwtPlotCurve *curv = new QwtPlotCurve (QwtPlotCurve("e(t)"));
    curv->setSamples(vX,vY);
    curv->attach(d_plot);
    if (choice)
    {
        for (int i  = 0 ; i < ui->selectCicle->value(); i++)
        {
            auto btn = new QPushButton();
            vecButton.append(btn);
            btn->setMinimumSize(25,25);
            btn->setText(QString::number(i+1));
            ui->dinamicPanel->addWidget(btn);
            connect(btn,&QPushButton::clicked,this,&supportmodul::processingModileDeform);
        }
    }
    else
    {
        QPointF point1;
        QPointF point2;

        bool down = true;
        bool first = true;
        int count = 0;
        double avarageDown = 0;

        if (index == 0)
        {
            countPoints = data->minPoints.size();
        }
        else
        {
            countPoints = index;
        }
        point1.setX(vX[0]);

        for (int i = 0; i < data->steps.size() && count < countPoints; i++)
        {

            if (data->steps[i].isDown)
            {
                avarageDown += data->steps[i].sigma1_;
                count++;
            }
            if (data->steps[i].isUp && point2.x() < data->steps[i].epsilon_ * 1000)
            {
                point2.setX(data->steps[i].epsilon_ * 1000);
                point2.setY(data->steps[i].sigma1_);
                qDebug() << i << '\n';
            }
        }
        avarageDown/=count;
        point1.setY(avarageDown);

        QVector<double> xs;
        QVector<double> ys;

        xs << point1.x()<< point2.x();
        ys << point1.y() << point2.y();

        clear();

        supCurv.append(new QwtPlotCurve ("y = kx + b"));
        supCurv[supCurv.size()-1]->setSamples(xs,ys);
        supCurv[supCurv.size()-1]->setPen(Qt::red,2);
        supCurv[supCurv.size()-1]->attach(d_plot);

        xs.clear();
        ys.clear();

        vector(point1.x(),point1.y(), point2.x(),point1.y());
        setText(1,"Δε<sub>max</sub>",point1.x(),point1.y(), point2.x(),point1.y());
        vector(point2.x(),point2.y(),point2.x(),point1.y());
        setText(3,"Δσ′",point2.x(),point1.y(),point2.x(),point2.y());

        dinamicModDifform = std::abs(point2.y() - point1.y()) / std::abs(point2.x() - point1.x()) ;
        ui->value->setText("E<sub>d</sub> = " + QString::number(dinamicModDifform) + " МПа.");
    }
    d_plot->replot();
}
