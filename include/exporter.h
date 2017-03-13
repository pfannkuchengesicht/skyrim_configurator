#ifndef EXPORTER_H
#define EXPORTER_H

#include <QDialog>
#include <QString>
#include <QTreeWidgetItem>
#include <QList>
#include <QStringList>

namespace Ui {
class exporter;
}

class exporter : public QDialog
{
    Q_OBJECT
    
public:
    explicit exporter(QWidget *parent, QString configpath, QString exportfile);
    ~exporter();

private slots:
    void on_btnCancel_clicked();

    void on_btnSave_clicked();

private:
    Ui::exporter *ui;
    QString config;
    QString exportfile;
    QTreeWidgetItem* files[2];

    void addTreeItems(QString file, int id);

    bool isChecked(QTreeWidgetItem* parent, QString name, QTreeWidgetItem **item);

    void closeEvent(QCloseEvent *);
};

#endif // EXPORTER_H
