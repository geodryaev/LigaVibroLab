#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QStringList>

void setAmplitudeStress(QDoubleSpinBox* box);
void setAmplitudeDeform(QDoubleSpinBox* box);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(800,600);
    ui->frequency->setSuffix(" Гц.");
    ui->frequency->setValue(0.5);
    ui->frequency->setRange(0.01,10);
    ui->frequency->setSingleStep(0.05);
    ui->typeAmplitude->addItem("По контролю напряжения");
    ui->typeAmplitude->addItem("По контролю деформации");
    ui->typeAmplitude->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    setAmplitudeStress(ui->amplitude);
    connect(ui->typeAmplitude,QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::ChangeAmplutude);

}

void MainWindow::ChangeAmplutude(int index)
{
    if (index == 0 )
    {
        setAmplitudeStress(ui->amplitude);
    }
    else if (index == 1)
    {
        setAmplitudeDeform(ui->amplitude);
    }
}

void setAmplitudeStress(QDoubleSpinBox* box)
{
    box->setRange(0,100);
    box->setSuffix(" Н");
    box->setValue(10);
    box->setSingleStep(5);
}

void setAmplitudeDeform(QDoubleSpinBox* box)
{
    box->setRange(0,0.5);
    box->setSuffix(" %, ε");
    box->setValue(0.001);
    box->setSingleStep(0.005);
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_calculateVibro_clicked()
{
    bool hat = true;
    QString str;
    QStringList list;
    QString filePath = QFileDialog::getOpenFileName();
    if (!filePath.isEmpty())
    {
        QFile file(filePath);
        if(file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream stream(&file);
            while (!stream.atEnd())
            {
                str = stream.readLine();
                str = str.replace(',','.');
                list = str.split('\t');
                if (!hat)
                {
                    date.push(list[0].toDouble(),list[3].toDouble(),list[4].toDouble(),list[5].toDouble(),list[6].toDouble(),list[7].toDouble(),list[8].toDouble(),list[9].toDouble(),list[10].toDouble(),list[11].toDouble(),list[1].toDouble());
                }
                else
                {
                    hat = false;
                }
            }
            qDebug() << Q_FUNC_INFO;
        }
    }
    date.normalizeData();

    qDebug() << 1;
}
