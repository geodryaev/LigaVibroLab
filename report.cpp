#include "report.h"

Report::Report() {}



void drawDetailedGraph(QPainter &painter, const QRect &area, const QVector<int> &data, const QStringList &labels) {
    const int left = area.left() + 40;
    const int bottom = area.bottom() - 30;
    const int top = area.top() + 20;
    const int right = area.right() - 20;

    // Оси
    QPen axisPen(Qt::black, 2);
    painter.setPen(axisPen);
    painter.drawLine(left, bottom, right, bottom); // X
    painter.drawLine(left, bottom, left, top);     // Y

    // Сетка
    QPen gridPen(Qt::lightGray, 1, Qt::DashLine);
    painter.setPen(gridPen);

    int maxY = *std::max_element(data.begin(), data.end());
    int yStep = 20;
    int yCount = maxY / yStep + 1;

    for (int i = 0; i < yCount; ++i) {
        int y = bottom - i * ((bottom - top) / yCount);
        painter.drawLine(left, y, right, y);

        painter.setPen(Qt::black);
        painter.drawText(left - 35, y + 5, QString::number(i * yStep));
        painter.setPen(gridPen);
    }

    // Подписи X
    painter.setPen(Qt::black);
    int xStep = (right - left) / (data.size() - 1);
    for (int i = 0; i < labels.size(); ++i) {
        int x = left + i * xStep;
        painter.drawText(x - 10, bottom + 20, labels[i]);
    }

    // Линия графика
    QPen linePen(Qt::blue, 2);
    painter.setPen(linePen);
    for (int i = 0; i < data.size() - 1; ++i) {
        int x1 = left + i * xStep;
        int y1 = bottom - (data[i] * (bottom - top)) / (yCount * yStep);
        int x2 = left + (i + 1) * xStep;
        int y2 = bottom - (data[i + 1] * (bottom - top)) / (yCount * yStep);
        painter.drawLine(QPoint(x1, y1), QPoint(x2, y2));
    }

    // Точки
    painter.setBrush(Qt::red);
    for (int i = 0; i < data.size(); ++i) {
        int x = left + i * xStep;
        int y = bottom - (data[i] * (bottom - top)) / (yCount * yStep);
        painter.drawEllipse(QPoint(x, y), 3, 3);
    }
}

void exportDetailedGraphToExcel(const QString &fileName) {
    // 1. Готовим данные
    QVector<int> values = {30, 50, 80, 60, 90, 110, 95};
    QStringList labels = {"Янв", "Фев", "Мар", "Апр", "Май", "Июн", "Июл"};

    // 2. Создаём картинку
    QPixmap pixmap(600, 400);
    pixmap.fill(Qt::white);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    QFont font = painter.font();
    font.setPointSize(9);
    painter.setFont(font);

    drawDetailedGraph(painter, QRect(50, 20, 520, 350), values, labels);
    painter.end();

    // 3. Вставляем в Excel
    QImage image = pixmap.toImage();

}


void Report::reportToFileExcel(vibroData date)
{
    QString pathToFile = QFileDialog::getSaveFileName(NULL, QObject::tr("Сохранить Excel файл"),QString(), QObject::tr("Excel Files (*.xlsx);;All Files (*)"));
    QXlsx::Document doc;
    doc.write("A1","Test");

    QVector<int> values = {30, 50, 80, 60, 90, 110, 95};
    QStringList labels = {"Янв", "Фев", "Мар", "Апр", "Май", "Июн", "Июл"};

    // 2. Создаём картинку
    QPixmap pixmap(600, 400);
    pixmap.fill(Qt::white);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    QFont font = painter.font();
    font.setPointSize(9);
    painter.setFont(font);

    drawDetailedGraph(painter, QRect(50, 20, 520, 350), values, labels);
    painter.end();

    // 3. Вставляем в Excel
    QImage image = pixmap.toImage();
    // Вставляем изображение в ячейку B2
    doc.insertImage(1,1,image);

    doc.saveAs(pathToFile);

}



