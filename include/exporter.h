/*
    Skyrim Configurator
    Copyright (C) 2017  Roy Freytag

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
