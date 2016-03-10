#ifndef RESULTSFORM_H
#define RESULTSFORM_H

#include <QDialog>

namespace Ui {
class ResultsForm;
}

class ResultsForm : public QDialog
{
    Q_OBJECT

public:
    explicit ResultsForm(QWidget *parent = 0);
    ~ResultsForm();

private:
    Ui::ResultsForm *ui;
};

#endif // RESULTSFORM_H
