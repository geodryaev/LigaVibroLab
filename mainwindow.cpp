#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "calcamplitud.h"
#include "correctinput.h"


void setAmplitudeStress(QDoubleSpinBox* box);
void setAmplitudeDeform(QDoubleSpinBox* box);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(800,600);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//Вибропалзучесть
void MainWindow::on_calculateVibro_clicked()
{
    bool hat = true;
    bool u0read = true;
    double u0;
    QString str;
    QStringList list;
    QString filePath = QFileDialog::getOpenFileName(this, "Выберите CSV файл","","CSV файлы (*.csv)");

    data = new vibroData(ui->height->value(),ui->diametrs->value(), ui->minAmpl->value(),ui->maxAmpl->value());
    data->frequency = ui->frquency->value();

    if (!filePath.isEmpty())
    {
        QFile file(filePath);
        if(file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream stream(&file);
            while (!stream.atEnd())
            {
                str = stream.readLine().replace(',','.');
                list = str.split('\t');
                if (!hat)
                {
                    if (u0read)
                    {
                        u0read = false;
                        u0 = list[6].toDouble();
                    }
                    data->push(list[0].toDouble(),list[3].toDouble(),list[4].toDouble(),list[5].toDouble(),list[6].toDouble(),list[7].toDouble(),list[8].toDouble(),list[9].toDouble(),list[10].toDouble(),list[11].toDouble(),list[1].toDouble(),u0);
                }
                else
                {
                    hat = false;
                }
            }
            qDebug() << Q_FUNC_INFO;
        }
    }

    correctInput * cor = new correctInput(nullptr, data);
    cor->setWindowState(Qt::WindowMaximized);
    if (cor->exec() == QDialog::Accepted)
    {
        if (ui->checkBox->isChecked())
            data->normalizeData();

        report.reportToFileExcelVibrocell(data);
        qDebug() << 1;
        delete(data);
    }
    else
    {
        QMessageBox::critical(nullptr,"Ошибка", "Надо было нажимать ОК для дальнейшей обработки");
    }
}

//Cейсмо
void MainWindow::on_calculateVibro_2_clicked()
{
    bool hat = true;
    bool u0read = true;
    double u0;
    QString str;
    QStringList list;
    QString filePath = QFileDialog::getOpenFileName(this, "Выберите CSV файл","","CSV файлы (*.csv)");

    data = new vibroData(ui->height_2->value(),ui->diametrs_2->value(), ui->minAmpl_2->value(), ui->maxAmpl_2->value());
    data->frequency = ui->frquency_2->value();


    if (!filePath.isEmpty())
    {
        QFile file(filePath);
        if(file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream stream(&file);
            while (!stream.atEnd())
            {
                str = stream.readLine().replace(',','.');
                list = str.split('\t');
                if (!hat)
                {
                    if (u0read)
                    {
                        u0read = false;
                        u0 = list[6].toDouble();
                    }
                    data->push(list[0].toDouble(),list[3].toDouble(),list[4].toDouble(),list[5].toDouble(),list[6].toDouble(),list[7].toDouble(),list[8].toDouble(),list[9].toDouble(),list[10].toDouble(),list[11].toDouble(),list[1].toDouble(),u0);
                }
                else
                {
                    hat = false;
                }
            }
            qDebug() << Q_FUNC_INFO;
        }
    }

    correctInput * cor = new correctInput(nullptr, data);
    cor->setWindowState(Qt::WindowMaximized);
    if (cor->exec() == QDialog::Accepted)
    {
        if (ui->checkBox->isChecked())
            data->normalizeData();

        report.reportToFileExcelSeismic(data);
        qDebug() << 1;
        delete(data);
    }
    else
    {
        QMessageBox::critical(nullptr,"Ошибка", "Надо было нажимать ОК для дальнейшей обработки");
    }
}

void MainWindow::on_action_triggered()
{
    calcAmplitud * w = new calcAmplitud();
    w->exec();
}
