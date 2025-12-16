#ifndef CHARTSETTINGS_H
#define CHARTSETTINGS_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class ChartSettings;
}

class ChartSettings : public QDialog
{
    Q_OBJECT

public:
    explicit ChartSettings(QWidget *parent = nullptr);
    ~ChartSettings();

private slots:

    void on_buttonBox_accepted();

    void on_set_color_chart_1_clicked();

    void on_set_color_chart_2_clicked();

    void on_set_color_backround_clicked();

    void on_set_color_font_clicked();

private:
    Ui::ChartSettings *ui;
    QSettings * setting;

    QColor colorGraph1;
    QColor colorGraph2;
    QColor colorBackround;
    QColor colorText;

    int sizeText;
    double sizeChart;
    int widthChart;
    int heightChart;

};

#endif // CHARTSETTINGS_H
