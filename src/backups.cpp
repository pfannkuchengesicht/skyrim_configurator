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

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QDateTime>
#include <QMessageBox>

#include "backups.h"
#include "ui_backups.h"
#include "clog.h"

#include "helpers.h"

backups::backups(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::backups)
{
    ui->setupUi(this);

    parentWidget()->setEnabled(false);
    this->setEnabled(true);

    Qt::WindowFlags flags = windowFlags() | Qt::MSWindowsFixedSizeDialogHint;
    overrideWindowFlags( flags );

    TCHAR szPath[MAX_PATH];
    SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, szPath);
    base_path = TCHAR2QString(szPath);
    base_path.append("\\My Games\\Skyrim\\");

    QStringList dates;
    QStringList fileFilter;
    fileFilter << "*.ini.backup_*";
    QDir save_dir(base_path);
    save_dir.setNameFilters(fileFilter);
    dates = save_dir.entryList(QDir::Files);
    dates.replaceInStrings("SkyrimPrefs.ini.backup_", "");
    dates.replaceInStrings("Skyrim.ini.backup_", "");
    dates.removeDuplicates();

    for(int i = 0; i < dates.length(); i++)
    {
        QDateTime time;
        time.setTime_t(QVariant(dates.at(i)).toUInt());
        ui->lstBackups->addItem(QVariant(time.date()).toString() + " " + QVariant(time.time()).toString());
    }

    ui->lstBackups->sortItems(Qt::DescendingOrder);
}

backups::~backups()
{
    delete ui;
}

void backups::on_btnRestore_clicked()
{
    QStringList dateTime;
    QDateTime time;

    bool worked = true;

    QMessageBox msg;

    if(!ui->lstBackups->count())
    {
        msg.setText("nothing to restore!");
        msg.exec();
        return;
    }

    dateTime = ui->lstBackups->currentItem()->text().split(" ");

    time.setDate(QVariant(dateTime.at(0)).toDate());
    time.setTime(QVariant(dateTime.at(1)).toTime());

    QFile file;

    file.setFileName(base_path + "SkyrimPrefs.ini");
    worked &= file.remove();
    skrmclog->event((worked?LOGLV_NORMAL:LOGLV_ERROR), file.errorString());

    file.setFileName(base_path + "SkyrimPrefs.ini.backup_" + QVariant(time.toTime_t()).toString());
    worked &= file.rename(base_path + "SkyrimPrefs.ini");
    skrmclog->event((worked?LOGLV_NORMAL:LOGLV_ERROR), file.errorString());

    file.setFileName(base_path + "Skyrim.ini");
    worked &= file.remove();
    skrmclog->event((worked?LOGLV_NORMAL:LOGLV_ERROR), file.errorString());

    file.setFileName(base_path + "Skyrim.ini.backup_" + QVariant(time.toTime_t()).toString());
    worked &= file.rename(base_path + "Skyrim.ini");
    skrmclog->event((worked?LOGLV_NORMAL:LOGLV_ERROR), file.errorString());

    if(worked)
    {
        msg.setText("restoring config from " + dateTime.at(0) + " " + dateTime.at(1) + " was successful!");
    }
    else
    {
        msg.setText("restoring config from " + dateTime.at(0) + " " + dateTime.at(1) + " failed!");
    }

    msg.exec();

    parentWidget()->setEnabled(true);
    reloadConfig();
    close();
}

void backups::on_btnCancelRest_clicked()
{
    parentWidget()->setEnabled(true);
    reloadConfig();
    close();
}

void backups::on_btnDeleteBackup_clicked()
{
    QStringList dateTime;
    QDateTime time;

    bool worked = true;

    QMessageBox msg;

    if(!ui->lstBackups->count())
    {
        msg.setText("nothing to delete!");
        msg.exec();
        return;
    }

    dateTime = ui->lstBackups->currentItem()->text().split(" ");

    time.setDate(QVariant(dateTime.at(0)).toDate());
    time.setTime(QVariant(dateTime.at(1)).toTime());

    QFile file;

    file.setFileName(base_path + "SkyrimPrefs.ini.backup_" + QVariant(time.toTime_t()).toString());
    worked &= file.remove();

    file.setFileName(base_path + "Skyrim.ini.backup_" + QVariant(time.toTime_t()).toString());
    worked &= file.remove();

    if(worked)
    {
        msg.setText("deleting backup from " + dateTime.at(0) + " " + dateTime.at(1) + " was successful!");
        delete ui->lstBackups->item(ui->lstBackups->currentIndex().row());
    }
    else
    {
        msg.setText("deleting backup from " + dateTime.at(0) + " " + dateTime.at(1) + " failed!");
    }

    msg.exec();
}

void backups::closeEvent(QCloseEvent *)
{
    parentWidget()->setEnabled(true);
    reloadConfig();
}
