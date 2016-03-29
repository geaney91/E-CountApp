#ifndef RESULTSFORM_H
#define RESULTSFORM_H

#include <QDialog>
#include "count.h"

namespace Ui {
class ResultsForm;
}

class ResultsForm : public QDialog
{
    Q_OBJECT

public:
    explicit ResultsForm(QWidget *parent = 0);
    ~ResultsForm();
    ResultsForm(QList<Count *> counts);
    void set_static_info(int total, int valid, int invalid, int quota, int seats);
    void set_up_form(Count *c);
    void add_info_to_form();
    void update_headers();
    void set_changes(QStringList changes);
    void set_elected();
    QStringList get_list();

private:
    Ui::ResultsForm *ui;
    QList<Count *> counts;
    Count *count;
    QStringList headers;
    QStringList changes;
    int keep_count;
};

#endif // RESULTSFORM_H
