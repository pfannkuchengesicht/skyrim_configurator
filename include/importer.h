#ifndef IMPORTER_H
#define IMPORTER_H

#include <QDialog>

namespace Ui {
class importer;
}

class importer : public QDialog
{
    Q_OBJECT
    
public:
    explicit importer(QWidget *parent, QString import);
    ~importer();
    
private slots:
    void on_btnYes_clicked();

    void on_btnNo_clicked();

signals:
    void doImport(QString import);
private:
    Ui::importer *ui;
    QString import;

    void closeEvent(QCloseEvent *);
};

#endif // IMPORTER_H
