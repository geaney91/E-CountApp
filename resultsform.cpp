#include "resultsform.h"
#include "ui_resultsform.h"

ResultsForm::ResultsForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResultsForm)
{
    ui->setupUi(this);
}

ResultsForm::~ResultsForm()
{
    delete ui;
}
