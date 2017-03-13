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

#include <QFile>
#include <QDir>
#include <QTextStream>

#include <QMessageBox>

#include "clog.h"
#include "cini_parse.h"

extern bool bLogging;

CINI_Parse::CINI_Parse(QString file, bool mode)
{
    //node_count = 0;
    cur_key = 0;
    alt_mode = mode;

    addNode("[General]");
    setNode(0);

    bLogging?skrmclog->enable():skrmclog->disable();

    if(!open(file))
    {
        skrmclog->event(LOGLV_ERROR, "couldn't open "+file);
        delete this;
    }

    setNode(0);
}

CINI_Parse::CINI_Parse(QString file)
{
    //node_count = 0;
    cur_key = 0;
    alt_mode = false;

    addNode("[General]");
    setNode(0);
    bLogging?skrmclog->enable():skrmclog->disable();

    if(!open(file))
    {
        skrmclog->event(LOGLV_ERROR, "couldn't open "+file);
        delete this;
    }

    setNode(0);
}

CINI_Parse::CINI_Parse()
{
    //node_count = 0;
    cur_key = 0;
    alt_mode = false;

    addNode("[General]");
    setNode(0);
    bLogging?skrmclog->enable():skrmclog->disable();
}


bool CINI_Parse::open(QString file)
{
    skrmclog->event(LOGLV_NORMAL, "parsing "+file);

    ini_file = new QFile(file);

    if(!ini_file->open(QIODevice::ReadWrite | QIODevice::Text))
        return false;

    QTextStream in(ini_file);
    QString line;
    QStringList key_val;
    QStringList val_com;

    int i = 0;
    while(!in.atEnd())
    {
        line = in.readLine();
        if(line.contains("[") && line.contains("]"))
        {
            addNode(line);
            setNode(line);
        }
        /*else if(!line.isEmpty() && line.contains(";"))
        {
            setValue("COMMENT"+QVariant(i).toString(), line);
            i++;
        }*/
        else if(!line.isEmpty())
        {
            key_val = line.split("=");
            if(key_val.length() == 2)
            {
                val_com = key_val.at(1).split(";");
                if(val_com.length() == 2)
                    setValue(key_val.at(0), QVariant(val_com.at(0)), val_com.at(1));
                else
                    setValue(key_val.at(0), QVariant(val_com.at(0)));
            }
            else
                setValue(key_val.at(0));
        }
    }

    return true;
}

void CINI_Parse::addNode(QString name)
{
    /*if(node_count == MAX_NODES)
    {
        skrmclog->event(LOGLV_ERROR, "too many nodes in ini!");
        return;
    }*/

    for(int i = 0; i < nodes.count(); i++)
    {
        if(nodes[i].getNode() == name)
            return;
    }

    nodes.append(IniNode(name));
    setNode(name);
    skrmclog->event(LOGLV_NORMAL ,"added node \""+name+"\"");
}

void CINI_Parse::setNode(QString name)
{
    for(int i = 0; i < nodes.count(); i++)
    {
        if(nodes[i].getNode() == name)
        {
            cur_node = i;
            skrmclog->event(LOGLV_NORMAL, "selecting \""+name+"\" as current");
            return;
        }
    }

    addNode(name);
}

bool CINI_Parse::setNode(int id)
{
    //if(id > node_count)
    //{
    //    cur_node = 0;
    //    skrmclog->event(LOGLV_NORMAL, "selecting \"0\" as current");
    //    return false;
    //}
    //else
    //{
        cur_node = id;
        skrmclog->event(LOGLV_NORMAL, "selecting \""+QVariant(id).toString()+"\" as current");

        if(id > (nodes.count() - 1) || id < 0)
            return false;

        //if(!nodes[cur_node])
          //  return false;

        if(nodes[cur_node].getNode().isEmpty())
            return false;

        return true;
    //}
}

QString CINI_Parse::setNextNode()
{
    cur_key = 0;
    if(setNode(++cur_node))
    {
        return nodes[cur_node].getNode();
    }
    else
    {
        return QString("");
    }
}

QVariant CINI_Parse::getValue(QString key)
{
    if(alt_mode)
    {
        for(int i = 0; i < nodes.count(); i++)
        {
            if(nodes[i].isKey(key))
            {
                //skrmclog->event(LOGLV_NORMAL, "got value \""+nodes[i]->getValue(key).toString().leftJustified(10, ' ')+"\" for \""+key.leftJustified(10, ' ')+"\" in \""+QVariant(i).toString()+"\" in file \""+ini_file->fileName()+"\"");
                skrmclog->event(LOGLV_NORMAL, "\""+ini_file->fileName()+"\": \""+QVariant(i).toString()+"\": \""+key.leftJustified(30, ' ')+"\": got \""+nodes[i].getValue(key).toString()+"\"");
                return nodes[i].getValue(key);
            }
        }
        skrmclog->event(LOGLV_WARNING, "no value found for key \""+key+"\" in file \""+ini_file->fileName()+"\"");
        return QVariant("NULL");
    }

    skrmclog->event(LOGLV_NORMAL, "\""+ini_file->fileName()+"\": \""+QVariant(cur_node).toString()+"\": \""+key.leftJustified(30, ' ')+"\": got \""+nodes[cur_node].getValue(key).toString()+"\"");
    return nodes[cur_node].getValue(key);
}

QString CINI_Parse::getComment(QString key)
{
    if(alt_mode)
    {
        for(int i = 0; i < nodes.count(); i++)
        {
            if(nodes[i].isKey(key))
            {
                //skrmclog->event(LOGLV_NORMAL, "got value \""+nodes[i]->getValue(key).toString().leftJustified(10, ' ')+"\" for \""+key.leftJustified(10, ' ')+"\" in \""+QVariant(i).toString()+"\" in file \""+ini_file->fileName()+"\"");
                skrmclog->event(LOGLV_NORMAL, "\""+ini_file->fileName()+"\": \""+QVariant(i).toString()+"\": \""+key.leftJustified(30, ' ')+"\": got \""+nodes[i].getValue(key).toString()+"\"");
                return nodes[i].getComment(key);
            }
        }
        skrmclog->event(LOGLV_WARNING, "no value found for key \""+key+"\" in file \""+ini_file->fileName()+"\"");
        return QString("");
    }

    skrmclog->event(LOGLV_NORMAL, "\""+ini_file->fileName()+"\": \""+QVariant(cur_node).toString()+"\": \""+key.leftJustified(30, ' ')+"\": got \""+nodes[cur_node].getValue(key).toString()+"\"");
    return nodes[cur_node].getComment(key);
}

QString CINI_Parse::getComment(int id)
{
    if(alt_mode)
    {
        for(int i = 0; i < nodes.count(); i++)
        {
            //skrmclog->event(LOGLV_NORMAL, "got value \""+nodes[i]->getValue(key).toString().leftJustified(10, ' ')+"\" for \""+key.leftJustified(10, ' ')+"\" in \""+QVariant(i).toString()+"\" in file \""+ini_file->fileName()+"\"");
            skrmclog->event(LOGLV_NORMAL, "\""+ini_file->fileName()+"\": \""+QVariant(i).toString()+"\": \""+nodes[i].getKey(id).leftJustified(30, ' ')+"\": got \""+nodes[i].getComment(id)+"\"");
            return nodes[i].getComment(id);
        }
        skrmclog->event(LOGLV_WARNING, "no comment found for id \""+QString::number(id)+"\" in file \""+ini_file->fileName()+"\"");
        return QString("");
    }

    skrmclog->event(LOGLV_NORMAL, "\""+ini_file->fileName()+"\": \""+QVariant(cur_node).toString()+"\": \""+nodes[cur_node].getKey(id).leftJustified(30, ' ')+"\": got \""+nodes[cur_node].getComment(id)+"\"");
    return nodes[cur_node].getComment(id);
}

QStringList CINI_Parse::getNext()
{
    return nodes[cur_node].getKV(cur_key++);
}

void CINI_Parse::setValue(QString key, QVariant value)
{
    if(alt_mode)
    {
        for(int i = 0; i < nodes.count(); i++)
        {
            if(nodes[i].isKey(key))
            {
                nodes[i].setValue(key, value);
                skrmclog->event(LOGLV_NORMAL, "\""+ini_file->fileName()+"\": \""+QVariant(i).toString()+"\": \""+key.leftJustified(30, ' ')+"\": set \""+value.toString()+"\"");
                return;
            }
        }
    }

    skrmclog->event(LOGLV_NORMAL, "\""+ini_file->fileName()+"\": \""+QVariant(cur_node).toString()+"\": \""+key.leftJustified(30, ' ')+"\": set \""+value.toString()+"\"");
    nodes[cur_node].setValue(key, value);
}

void CINI_Parse::setValue(QString key, QVariant value, QString comment)
{
    if(alt_mode)
    {
        for(int i = 0; i < nodes.count(); i++)
        {
            if(nodes[i].isKey(key))
            {
                nodes[i].setValue(key, value, comment);
                skrmclog->event(LOGLV_NORMAL, "\""+ini_file->fileName()+"\": \""+QVariant(i).toString()+"\": \""+key.leftJustified(30, ' ')+"\": set \""+value.toString()+"\"");
                return;
            }
        }
    }

    skrmclog->event(LOGLV_NORMAL, "\""+ini_file->fileName()+"\": \""+QVariant(cur_node).toString()+"\": \""+key.leftJustified(30, ' ')+"\": set \""+value.toString()+"\"");
    nodes[cur_node].setValue(key, value, comment);
}

void CINI_Parse::setValue(QString key)
{
    if(alt_mode)
    {
        for(int i = 0; i < nodes.count(); i++)
        {
            if(nodes[i].isKey(key))
            {
                nodes[i].setValue(key);
                skrmclog->event(LOGLV_NORMAL, "\""+ini_file->fileName()+"\": \""+QVariant(i).toString()+"\": \""+key.leftJustified(30, ' ')+"\": set \"\"");
                return;
            }
        }
    }

    skrmclog->event(LOGLV_NORMAL, "\""+ini_file->fileName()+"\": \""+QVariant(cur_node).toString()+"\": \""+key.leftJustified(30, ' ')+"\": set \"\"");
    nodes[cur_node].setValue(key);
}

void CINI_Parse::setComment(QString key, QString comment)
{
    if(alt_mode)
    {
        for(int i = 0; i < nodes.count(); i++)
        {
            if(nodes[i].isKey(key))
            {
                nodes[i].setComment(key, comment);
                skrmclog->event(LOGLV_NORMAL, "\""+ini_file->fileName()+"\": \""+QVariant(i).toString()+"\": \""+key.leftJustified(30, ' ')+"\": set \"\"");
                return;
            }
        }
    }

    skrmclog->event(LOGLV_NORMAL, "\""+ini_file->fileName()+"\": \""+QVariant(cur_node).toString()+"\": \""+key.leftJustified(30, ' ')+"\": set \"\"");
    nodes[cur_node].setComment(key, comment);
}

void CINI_Parse::setComment(int id, QString comment)
{
    if(alt_mode)
    {
        for(int i = 0; i < nodes.count(); i++)
        {
            nodes[i].setComment(id, comment);
            skrmclog->event(LOGLV_NORMAL, "\""+ini_file->fileName()+"\": \""+QVariant(i).toString()+"\": \""+nodes[i].getKey(id).leftJustified(30, ' ')+"\": set \"\"");
            return;
        }
    }

    skrmclog->event(LOGLV_NORMAL, "\""+ini_file->fileName()+"\": \""+QVariant(cur_node).toString()+"\": \""+nodes[cur_node].getKey(id).leftJustified(30, ' ')+"\": set \"\"");
    nodes[cur_node].setComment(id, comment);
}

void CINI_Parse::save()
{
    ini_file->seek(0);

    QTextStream out(ini_file);
    for(int i = 0; i < nodes.count(); i++)
    {
        if(nodes[i].getKVCount())
        {
            out << getNode(i) + "\n";
        }

        for(int j = 0; j < nodes[i].getKVCount(); j++)
        {
            if(nodes[i].getKV(j).at(0).contains("COMMENT"))
            {
                out << nodes[i].getKV(j).at(1) << "\n";
                continue;
            }


            if(!nodes[i].getKey(j).isEmpty())
            {
                out << nodes[i].getKey(j) << "=" << nodes[i].getValue(j).toString();
                if(!nodes[i].getComment(j).isEmpty())
                    out << ";" << nodes[i].getComment(j);

                out  << "\n";
            }
        }
    }
    ini_file->resize(out.pos());

    skrmclog->event(LOGLV_NORMAL ,"saved config file to disk!");
}

QString CINI_Parse::getNode(int id)
{
    if(id > nodes.count() || id < 0)
        return QString("NULL");

    return nodes[id].getNode();
}

void CINI_Parse::setFile(QString file)
{
    if(!open(file))
    {
        skrmclog->event(LOGLV_ERROR, "couldn't open "+file);
        delete this;
    }
}
