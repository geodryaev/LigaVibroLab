#include "supportmodul.h"
#include "QVector"
#include <QPointer>
#include <qpushbutton.h>
#include "ui_supportmodul.h"
#include "qwt/qwt_text.h"

#include <QPen>

supportmodul::supportmodul(bool def, int countCicle, const vibroData* data, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::supportmodul)
{
    qDebug() << countCicle;
    ui->setupUi(this);
    d_plot = new QwtPlot (nullptr);
    QVBoxLayout *layout = new QVBoxLayout (ui->graph);
    layout->addWidget(d_plot);

    int count = 0;
    bool skipFirst = true;
    for (int i = 0; i < data->steps.size() && count < countCicle+1;i++)
    {
        if (data->steps[i].isDown)
            count++;
        if (!skipFirst)
        {
            vX.push_back(data->steps[i].epsilon_);
            vY.push_back(data->steps[i].m_verticalPressure_kPa);
        }
        else
        {
            skipFirst = false;
        }
    }

    QwtPlotCurve *curv = new QwtPlotCurve (QwtPlotCurve("e(t)"));
    curv->setSamples(vX,vY);
    curv->attach(d_plot);

    double scalePadding = 1.1;
    // d_plot->setAxisScale(QwtPlot::xBottom, *std::min_element(vX.begin(),vX.end())*(1-(1-scalePadding)),*std::max_element(vX.begin(),vX.end())*scalePadding);
    // d_plot->setAxisScale(QwtPlot::yLeft, *std::min_element(vY.begin(),vY.end())*(1-(1-scalePadding)),*std::max_element(vY.begin(),vY.end())*scalePadding);
    // d_plot->setAxisScale(QwtPlot::xBottom, -1,*std::max_element(vX.begin(),vX.end())*scalePadding);
    // d_plot->setAxisScale(QwtPlot::yLeft, -1,*std::max_element(vY.begin(),vY.end())*scalePadding);
    downDistance = *std::min_element(vY.begin(),vY.end()) - ((*std::max_element(vY.begin(),vY.end()) - *std::min_element(vY.begin(),vY.end())) * 0.1);
    upDistance = *std::max_element(vX.begin(),vX.end()) - *std::min_element(vX.begin(), vX.end());
    grid.setMajorPen(QPen(Qt::gray,1,Qt::DashLine));
    grid.setMinorPen(QPen(Qt::gray,1,Qt::DashLine));
    grid.attach(d_plot);

    QVector<QPointer<QPushButton>> vecButton;
    for (int i  = 0 ; i < countCicle; i++)
    {
        auto btn = new QPushButton();
        vecButton.append(btn);
        btn->setMinimumSize(25,25);
        btn->setText(QString::number(i+1));
        ui->dinamicPanel->addWidget(btn);
        connect(btn,&QPushButton::clicked,this,&supportmodul::processingModileDeform);
    }

    d_plot->replot();
    this->data = data;
}



supportmodul::~supportmodul()
{
    delete ui;
}

void supportmodul::processingModileDeform(bool checked)
{
    QPushButton * btn = qobject_cast<QPushButton*>(sender());
    QPair<double,double> point1, point2;

    int numberCicle = btn->text().toInt()-1;
    int count = 0;
    bool down = false;
    for (int i = 0; i < data->steps.size() && count < numberCicle + 3; i++)
    {
        if (!down && data->steps[i].isDown)
        {
            if (count == numberCicle + 1)
            {
                point1.first = data->steps[i].epsilon_;
                point1.second = data->steps[i].m_verticalPressure_kPa;
                down = true;
            }
             count++;
        }
        if (down && data->steps[i].isUp)
        {
                point2.first = data->steps[i].epsilon_;
                point2.second = data->steps[i].m_verticalPressure_kPa;
                break;
        }
    }


    double k = (point2.second - point1.second)/(point2.first - point1.first);
    double b = point1.second - k * point1.first;
    QVector<double> xs;
    QVector<double> ys;

    xs << point1.first << point2.first;
    ys << point1.second << point2.second;

    clear();

    supCurv.append(new QwtPlotCurve ("y = kx + b"));
    supCurv[supCurv.size()-1]->setSamples(xs,ys);
    supCurv[supCurv.size()-1]->setPen(Qt::red,2);
    supCurv[supCurv.size()-1]->attach(d_plot);

    xs.clear();
    ys.clear();

    //нижняя прямая

    double lenY = (*std::max_element(vY.begin(),vY.end()) - *std::min_element(vY.begin(),vY.end()))*0.01;
    vector(point1.first,downDistance,point2.first,downDistance);
    vector(point2.first,point2.second + lenY*0.05, point2.first, point1.first);



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
        double lenX = abs(y2-x1) * 0.05;

        setLine(x1,y1,x1+downDistance*0.1,y1+lenX * 0.01);
        setLine(x1,y1,x1+lenX,y1-lenX);
        setLine(x2,y2,x2-lenX,y2-lenX);
        setLine(x2,y2,x2-lenX,y2+lenX);
    }


    supCurv.append(new QwtPlotCurve ("sigma"));
    supCurv[supCurv.size()-1]->setSamples(xs,ys);
    supCurv[supCurv.size()-1]->setPen(Qt::red,2);
    supCurv[supCurv.size()-1]->attach(d_plot);


}

