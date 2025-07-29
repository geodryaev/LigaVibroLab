#include <ui_correctinput.h>
#include <QPen>

#include "stepvibro.h"
#include "correctinput.h"

correctInput::correctInput(QWidget *parent, vibroData * data)
    : QDialog(parent)
    , ui(new Ui::correctInput)
{
    ui->setupUi(this);
    QVector<double>XData;
    QVector<double>YData;
    this->data = data;
    for (const stepVibro &el : std::as_const(data->steps))
    {
        YData.append(el.m_verticalPressure_kPa);
        XData.append(el.m_time);
    }

        plot = new QwtPlot(this);
        double minValX = *std::min_element(XData.begin(), XData.end());
        double maxValX = *std::max_element(XData.begin(), XData.end());

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
        curv->setPen(QPen(Qt::black,2));
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
        picker->setStateMachine(new QwtPickerClickPointMachine()); // режим: одиночные клики
        picker->setMousePattern(QwtEventPattern::MouseSelect1, Qt::RightButton);
        picker->setTrackerMode(QwtPicker::AlwaysOn);               // показывать координаты
        picker->setRubberBand(QwtPicker::CrossRubberBand);            // не рисуем прямоугольник
        picker->setEnabled(true);

        // Цвет "подсказки" координат:
        picker->setTrackerPen(QPen(Qt::black));

        // Подключаем сигнал
        connect(picker, SIGNAL(selected(QPointF)), this, SLOT(onPointClick(QPointF)));

        plot->resize(800,600);
        plot->replot();
        if (!ui->widget->layout()) {
            // Если layout нет — создаём вертикальный
            QVBoxLayout *layout = new QVBoxLayout(ui->widget);
            layout->addWidget(plot);
        } else {
            // Если layout уже есть — просто добавляем в него график
            ui->widget->layout()->addWidget(plot);
        }

}

correctInput::~correctInput()
{
    delete ui;
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

            // Здесь можешь сделать, что хочешь: обрезать график, выделить цветом и т.д.
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
                                    QBrush(Qt::red),
                                    QPen(Qt::red),
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

