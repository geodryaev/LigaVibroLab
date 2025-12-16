#include "chartsettings.h"
#include "ui_chartsettings.h"

#include <QColorDialog>

ChartSettings::ChartSettings(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ChartSettings)
{
    ui->setupUi(this);
    setting = new QSettings("settings.ini",QSettings::IniFormat);
    QColor color;

    color = setting->value("color/graph1").isValid() ? QColor(setting->value("color/graph1").toString()) : QColor(Qt::green).name();
    ui->color_chart_1->setStyleSheet(QString("background-color: %1").arg(color.name()));
    colorGraph1 = color;

    color = setting->value("color/graph2").isValid() ? QColor(setting->value("color/graph2").toString()) : QColor(Qt::red).name();
    ui->color_chart_2->setStyleSheet(QString("background-color: %1").arg(color.name()));
    colorGraph2 = color;

    color = setting->value("color/backround").isValid() ? QColor(setting->value("color/backround").toString()) : QColor(Qt::white ).name();
    ui-> color_backround->setStyleSheet(QString("background-color: %1").arg(color.name()));
    colorBackround = color;

    color = setting->value("color/font").isValid() ? QColor(setting->value("color/font").toString()) : QColor(Qt::black).name();
    ui-> color_font->setStyleSheet(QString("background-color: %1").arg(color.name()));
    colorText = color;


    ui->char_width->setValue(setting->value("size/width").isValid() ? setting->value("size/width").toInt() : 400);
    ui->char_height->setValue(setting->value("size/height").isValid() ? setting->value("size/height").toInt() :600);
    ui->chart_text_size->setValue(setting->value("size/text").isValid() ? setting->value("size/text").toInt() :12);
    ui->chart_line_size->setValue(setting->value("size/chart").isValid() ? setting->value("size/chart").toDouble() : 1.0);

    widthChart = ui->char_width->value();
    heightChart = ui->char_height->value();
    sizeText = ui->chart_text_size->value();
    sizeChart = ui->chart_line_size->value();
}

ChartSettings::~ChartSettings()
{
    delete ui;
    delete setting;
}

void ChartSettings::on_buttonBox_accepted()
{

    widthChart = ui->char_width->value();
    heightChart = ui->char_height->value();
    sizeText = ui->chart_text_size->value();
    sizeChart = ui->chart_line_size->value();

    setting->setValue("color/graph1",colorGraph1.name());
    setting->setValue("color/graph2",colorGraph2.name());
    setting->setValue("color/backround",colorBackround.name());
    setting->setValue("color/font",colorText.name());
    setting->setValue("size/text",sizeText);
    setting->setValue("size/chart",sizeChart);
    setting->setValue("size/width",widthChart);
    setting->setValue("size/height",heightChart);

    setting->sync();
    delete(setting);
}

void ChartSettings::on_set_color_chart_1_clicked()
{
    QColorDialog dialog;

    dialog.exec();
    QColor selectColor = dialog.selectedColor();
    ui->color_chart_1->setStyleSheet(QString("background-color: %1").arg(selectColor.name()));
    colorGraph1 = selectColor;
}


void ChartSettings::on_set_color_chart_2_clicked()
{
    QColorDialog dialog;

    dialog.exec();
    QColor selectColor = dialog.selectedColor();
    ui->color_chart_2->setStyleSheet(QString("background-color: %1").arg(selectColor.name()));
    colorGraph2 = selectColor;
}


void ChartSettings::on_set_color_backround_clicked()
{
    QColorDialog dialog;

    dialog.exec();
    QColor selectColor = dialog.selectedColor();
    ui->color_backround->setStyleSheet(QString("background-color: %1").arg(selectColor.name()));
    colorBackround = selectColor;
}


void ChartSettings::on_set_color_font_clicked()
{
    QColorDialog dialog;

    dialog.exec();
    QColor selectColor = dialog.selectedColor();
    ui->color_font->setStyleSheet(QString("background-color: %1").arg(selectColor.name()));
    colorText = selectColor;
}
