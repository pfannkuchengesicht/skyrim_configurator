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
