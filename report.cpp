#include "report.h"

Report::Report() {}

void Report::reportToFileExcel()
{
    QString pathToFile = QFileDialog::getSaveFileName(NULL, QObject::tr("Сохранить Excel файл"),QString(), QObject::tr("Excel Files (*.xlsx);;All Files (*)"));
    QXlsx::Document doc;
    doc.write("A1","Test");
    doc.saveAs(pathToFile);

}


