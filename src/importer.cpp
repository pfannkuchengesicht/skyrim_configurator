#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QFile>
#include <QXmlStreamReader>
#include <QMessageBox>
#include "clog.h"
#include "importer.h"
#include "ui_importer.h"

importer::importer(QWidget *parent, QString import) :
    QDialog(parent),
    ui(new Ui::importer)
{
    ui->setupUi(this);

    this->import = import;
    QMessageBox msg;
    QString key;

    QFile *pFile = new QFile(import);
    if(!pFile->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        skrmclog->event(LOGLV_ERROR, "couldn't open \""+import+"\" for reading!");
        msg.setText(tr("couldn't import file!"));
        msg.setWindowTitle(tr("Error!"));
        msg.setIcon(QMessageBox::Warning);
        msg.exec();
        this->close();
    }
    QXmlStreamReader *pReader = new QXmlStreamReader(pFile);

    QTreeWidgetItem *item, *node, *file;

    while(pReader->readNextStartElement())
    {
        if(pReader->name() == "cfg")
        {
            if(pReader->attributes().value("ver") == "1.0")
            {
                continue;
            }
            else
            {
                this->close();
            }
        }
        else if(pReader->name() == "config")
        {

            if(pReader->attributes().value("file") == "user")
            {
                file = new QTreeWidgetItem();
                file->setText(0, "Skyrim.ini");
            }
            else if(pReader->attributes().value("file") == "main")
            {
                file = new QTreeWidgetItem();
                file->setText(0, "SkyrimPrefs.ini");
            }

            while(pReader->readNextStartElement())
            {
                if(pReader->name() == "section")
                {
                    node = new QTreeWidgetItem();
                    node->setText(0, pReader->attributes().value("name").toString());
                    //conf->startSection(pReader->attributes().value("name").toString());

                    while(pReader->readNextStartElement())
                    {
                        if(pReader->name() == "value")
                        {
                            if((key = pReader->attributes().value("key").toString()).isEmpty())
                                continue;
                            item = new QTreeWidgetItem();
                            item->setText(0, key);
                            item->setText(1, pReader->readElementText());
                            node->addChild(item);
                        }
                        else pReader->skipCurrentElement();
                    }

                    file->addChild(node);
                }
                else pReader->skipCurrentElement();
            }

            ui->treChanges->addTopLevelItem(file);
        }
        else pReader->skipCurrentElement();
    }

    pReader->clear();
    pFile->close();
    delete pReader;
    delete pFile;
}

importer::~importer()
{
    delete ui;
}

void importer::on_btnYes_clicked()
{
    doImport(import);
    this->close();
}

void importer::on_btnNo_clicked()
{
    this->close();
}

void importer::closeEvent(QCloseEvent *)
{
    ui->treChanges->clear();
    delete this;
}
