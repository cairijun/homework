#include "StatisticsDialog.h"
#include "ui_StatisticsDialog.h"

StatisticsDialog::StatisticsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StatisticsDialog)
{
    ui->setupUi(this);
}

StatisticsDialog::~StatisticsDialog()
{
    delete ui;
}
