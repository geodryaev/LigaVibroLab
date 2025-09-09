#include "supportmodul.h"
#include "QVector"
#include <QPointer>
#include <qpainter.h>
#include <qpushbutton.h>
#include "ui_supportmodul.h"
#include "qwt/qwt_text.h"

#include <QPen>

supportmodul::supportmodul(bool def, int countCicle, const vibroData* data_, QWidget *parent)
    : QDialog(parent),
    ui(new Ui::supportmodul),
    data(data_),
    choice(def)
{
    ui->setupUi(this);

    d_plot = new QwtPlot (nullptr);
    QVBoxLayout *layout = new QVBoxLayout (ui->graph);
    layout->addWidget(d_plot);
    int count = 0;
    bool start = false;

    d_plot->setTitle("Определение динамического модуля упругости");
    d_plot->setAxisTitle(QwtAxis::YLeft, QwtText("Осевое напряжение Δσ′<sub>1</sub>, кПа."));
    d_plot->setAxisTitle(QwtAxis::XBottom, QwtText("Осевое напряжение Δe*10<sup>-3</sup>, кПа."));

    for (int i = 0; i < data->steps.size() && count < countCicle+1; i++)
    {
        if (data_->steps[i].isDown)
            count++;
        if (start)
        {
            vX.push_back(data_->steps[i].epsilon_);
            vY.push_back(data_->steps[i].m_verticalPressure_kPa);
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
}

supportmodul::~supportmodul()
{
    qDebug()<< "Я умер";
}

QImage* supportmodul::getImage()
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

    for (int i = 0; i < data->steps.size() && count < numberCicle + 3; i++)
    {
        if (data->steps[i].isDown)
        {
            if (count == numberCicle + 1)
            {
                point1.setX( data->steps[i].epsilon_);
                point1.setY( data->steps[i].m_verticalPressure_kPa);
                down = true;
            }
            if (count == numberCicle + 2)
            {
                point3.setX( data->steps[i].epsilon_);
                point3.setY(data->steps[i].m_verticalPressure_kPa);
            }
            count++;
        }
        if ( down && data->steps[i].isUp)
        {
            point2.setX(data->steps[i].epsilon_);
            point2.setY(data->steps[i].m_verticalPressure_kPa);
            down = false;
        }
    }

    double k = (point2.y()- point1.y())/(point2.x() - point1.x());
    double b = point1.y()- k * point1.x();
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


    vector(point1.x(),downDistance,point2.x(),downDistance);
    setText(1,"Δε<sub>c</sub>",point1.x(),downDistance,point2.x(),downDistance);

    vector(point2.x(),point2.y(), point2.x(), point1.x());
    setText(3,"Δσ′",point2.x(),point2.y(), point2.x(), point1.x());

    vector(point3.x(),point3.y(),point2.x(),point3.y());
    setText(1,"Δε<sub>y</sub>",point3.x(),point3.y(),point2.x(),point3.y());

    dinamicModElastic = (point2.y()-point1.y())/(point2.x()-point3.x());



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
    img = new QImage(d_plot->size(),QImage::Format_ARGB32);
    img->fill(Qt::white);
    QPainter paint(img);
    d_plot->render(&paint);
    paint.end();
    accepted();
}
