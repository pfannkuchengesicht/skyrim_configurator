#include <QTreeWidgetItemIterator>
#include <QXmlStreamWriter>
#include <QFile>
#include <QMessageBox>
#include "exporter.h"
#include "ui_exporter.h"
#include "cini_parse.h"
#include "clog.h"

exporter::exporter(QWidget *parent, QString configpath, QString exportfile) :
    QDialog(parent),
    ui(new Ui::exporter)
{
    ui->setupUi(this);

    this->config = configpath;
    this->exportfile = exportfile;

    ui->txtExport->setText(this->exportfile);
    //ui->treKeys->setGeometry(0, 0, this->width(), 300);

    addTreeItems("Skyrim.ini", 0);
    addTreeItems("SkyrimPrefs.ini", 1);

}

exporter::~exporter()
{
    delete ui;
}

void exporter::addTreeItems(QString file, int id)
{
    CINI_Parse *ini = new CINI_Parse(config+file);

    QString node;

    QTreeWidgetItem *ini_tmp;
    QTreeWidgetItem *node_tmp;
    QTreeWidgetItem *kv_tmp;

    ini_tmp = new QTreeWidgetItem();
    ini_tmp->setText(0, file);
    ini_tmp->setFlags(ini_tmp->flags() | Qt::ItemIsTristate);
    ini_tmp->setCheckState(0, Qt::Checked);

    ini->setNode(-1);
    while(!(node = ini->setNextNode()).isEmpty())
    {
        node_tmp = new QTreeWidgetItem();
        node_tmp->setText(0, node);
        node_tmp->setFlags(node_tmp->flags() | Qt::ItemIsTristate);
        node_tmp->setCheckState(0, Qt::Checked);

        QStringList kv;
        while(!(kv = ini->getNext()).isEmpty())
        {
            kv_tmp = new QTreeWidgetItem();
            kv_tmp->setText(0, kv.at(0));
            kv_tmp->setText(1, kv.at(1));

            if(     (kv.at(0) == "sD3DDevice") ||
                    (kv.at(0) == "iScreenShotIndex") ||
                    (kv.at(0) == "sLanguage") ||
                    (kv.at(0) == "sLocalSavePath") ||
                    (kv.at(0) == "SGeneralMasterMismatchWarning") ||
                    (kv.at(0) == "sResourceArchiveList") ||
                    (kv.at(0) == "sResourceArchiveList2"))
            {
                kv_tmp->setCheckState(0, Qt::Unchecked);
            }
            else
            {
                kv_tmp->setCheckState(0, Qt::Checked);
            }

            node_tmp->addChild(kv_tmp);
        }

        ini_tmp->addChild(node_tmp);
    }

    files[id] = ini_tmp;
    ui->treKeys->addTopLevelItem(ini_tmp);

    delete ini;
}

bool exporter::isChecked(QTreeWidgetItem *parent, QString name, QTreeWidgetItem** item)
{
    int i;

    for( i = 0; i < parent->childCount(); i++)
    {
        if(parent->child(i)->text(0) == name)
        {
            if(parent->child(i)->checkState(0) == Qt::Checked || parent->child(i)->checkState(0) == Qt::PartiallyChecked)
            {
                *item = parent->child(i);
                return true;
            }
            else if(parent->child(i)->checkState(0) == Qt::Unchecked)
            {
                *item = 0;
                return false;
            }
        }
    }

    return false;
}

void exporter::closeEvent(QCloseEvent *)
{
    ui->treKeys->clear();
    delete this;
}

void exporter::on_btnCancel_clicked()
{
    this->close();
}

void exporter::on_btnSave_clicked()
{
    QMessageBox msg;
    QFile *pFile = new QFile(exportfile);
    if(!pFile->open(QIODevice::WriteOnly))
    {
        skrmclog->event(LOGLV_ERROR, "couldn't open \""+exportfile+"\" for writing!");
        msg.setText(tr("couldn't export file!"));
        msg.setWindowTitle(tr("Error!"));
        msg.setIcon(QMessageBox::Warning);
        msg.exec();
    }

    QXmlStreamWriter *pWriter = new QXmlStreamWriter(pFile);
    pWriter->setAutoFormatting(true);

    pWriter->writeStartDocument();
    pWriter->writeStartElement("cfg");
    pWriter->writeAttribute("ver", "1.0");

    CINI_Parse *pIni = new CINI_Parse(config+"SkyrimPrefs.ini");

    pWriter->writeStartElement("config");
    pWriter->writeAttribute("file", "main");

    QTreeWidgetItem *tmp, *tmp2;
    QStringList kv;
    QString node;
    pIni->setNode(-1);
    while(!(node = pIni->setNextNode()).isEmpty())
    {
        if(isChecked(files[1], node, &tmp))
        {
            pWriter->writeStartElement("section");
            pWriter->writeAttribute("name", node.remove('[').remove(']'));

            while(!(kv = pIni->getNext()).isEmpty())
            {
                if(isChecked(tmp, kv.at(0), &tmp2))
                {
                    pWriter->writeStartElement("value");
                    pWriter->writeAttribute("key", kv.at(0));
                    pWriter->writeCharacters(kv.at(1));
                    pWriter->writeEndElement();
                }
            }
            pWriter->writeEndElement();
        }
    }

    delete pIni;

    pWriter->writeEndElement();

    pIni = new CINI_Parse(config+"Skyrim.ini");

    pWriter->writeStartElement("config");
    pWriter->writeAttribute("file", "user");

    pIni->setNode(-1);
    while(!(node = pIni->setNextNode()).isEmpty())
    {
        if(isChecked(files[0], node, &tmp))
        {
            pWriter->writeStartElement("section");
            pWriter->writeAttribute("name", node.remove('[').remove(']'));

            while(!(kv = pIni->getNext()).isEmpty())
            {
                if(isChecked(tmp, kv.at(0), &tmp2))
                {
                    pWriter->writeStartElement("value");
                    pWriter->writeAttribute("key", kv.at(0));
                    pWriter->writeCharacters(kv.at(1));
                    pWriter->writeEndElement();
                }
            }
            pWriter->writeEndElement();
        }
    }

    delete pIni;

    pWriter->writeEndElement();
    pWriter->writeEndElement();
    pWriter->writeEndDocument();
    delete pWriter;

    msg.setText(tr("Successfully exported the config!"));
    msg.exec();

    skrmclog->event(LOGLV_NORMAL, "successfully writen export");

    pFile->close();
    delete pFile;

    this->close();
}
