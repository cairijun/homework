#ifndef STATISTICSDIALOG_H
#define STATISTICSDIALOG_H

#include <QDialog>

namespace Ui {
class StatisticsDialog;
}

class StatisticsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit StatisticsDialog(QWidget *parent = 0);
    ~StatisticsDialog();
    void setContent(const QString & content);
    
private:
    Ui::StatisticsDialog *ui;
};

#endif // STATISTICSDIALOG_H
