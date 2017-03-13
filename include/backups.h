#ifndef BACKUPS_H
#define BACKUPS_H

#include <QDialog>

namespace Ui {
    class backups;
}

class backups : public QDialog
{
    Q_OBJECT

public:
    explicit backups(QWidget *parent = 0);
    ~backups();

signals:
    void reloadConfig();

private slots:
    void on_btnRestore_clicked();

    void on_btnCancelRest_clicked();

    void on_btnDeleteBackup_clicked();

private:
    Ui::backups *ui;
    QString base_path;

    void closeEvent(QCloseEvent *);
};

#endif // BACKUPS_H
