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
