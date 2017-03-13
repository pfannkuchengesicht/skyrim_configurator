#ifndef SAVES_PROFNAME_H
#define SAVES_PROFNAME_H

#include <QDialog>

namespace Ui {
    class saves_profname;
}

class saves_profname : public QDialog
{
    Q_OBJECT

public:
    explicit saves_profname(QWidget *parent = 0);
    ~saves_profname();

signals:
    void addListProfileItem(QString item);

private:
    Ui::saves_profname *ui;
    void closeEvent(QCloseEvent *);

private slots:
    void on_btnSetProfName_clicked();
};

#endif // SAVES_PROFNAME_H
