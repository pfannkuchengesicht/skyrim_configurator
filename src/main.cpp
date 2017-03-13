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

#include <QApplication>
#include <QTranslator>
#include <string.h>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w;
    QString locale = QLocale::system().name();
    QTranslator translator;
    if(!translator.load(QString("skrmcnf.lng"), QString("./locals/")+locale))
    {
        if(!translator.load(QString("skrmcnf.lng"), QString("./locals/")+locale.split("_").at(0)))
        {
            translator.load(QString("skrmcnf.lng"), QString("./locals/en"));
        }
    }

    a.installTranslator(&translator);

    MainWindow *w;
    if(argc > 1)
    {
        int i;
        for( i = 1; i < argc; i++ )
        {
            if(strcmp(argv[i],"-logging") == 0)
            {
                w = new MainWindow(NULL, true);
            }
            else
            {
                w = new MainWindow(NULL, false);
            }
        }
    }
    else
    {
        w = new MainWindow(NULL, false);
    }




    w->show();

    return a.exec();
}
