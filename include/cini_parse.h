#ifndef CINI_PARSE_H
#define CINI_PARSE_H

#include <QList>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QFile>
#include <QDir>
#include <QTextStream>

#include <QMessageBox>

#define MAX_KEYS 512
#define MAX_NODES 64

class KeyValue
{
private:
    QString key;
    QVariant value;
    QString comment;
public:
    KeyValue(QString key, QVariant val, QString comment)
    {
        this->key = key;
        this->value = val;
        this->comment = comment;
    }

    KeyValue(QString key)
    {
        this->key = key;
        this->value = QVariant("NULL");
    }

    KeyValue()
    {
        this->key = "NULL";
        this->value = QVariant("NULL");
    }

    ~KeyValue()
    {
    }

    QVariant getValue() { return value; }
    void setValue(QVariant val) { this->value = val; }
    QString getKey() {  return key; }
    void setKey(QString key) { this->key = key; }
    QString getComment() { return comment; }
    void setComment(QString comment) { this->comment = comment; }
};

class IniNode
{
private:
    QString name;
    //KeyValue *kv_list[MAX_KEYS];
    QList<KeyValue> kv_list;
    //int kv_count;
public:
    IniNode(QString name)
    {
        //kv_count = 0;
        this->name = name;
    }

    IniNode()
    {
        //kv_count = 0;
        this->name = "NULL";
    }

    void addKV(QString key, QVariant value, QString comment)
    {
        /*
        if(kv_count == MAX_KEYS)
            return;
        */
        if(isKey(key))
            return;

        //kv_list[kv_count++] = new KeyValue(key, value);
        kv_list.append(KeyValue(key, value, comment));
    }

    bool isKey(QString name)
    {
        for(int i = 0; i < kv_list.count(); i++)
        {
            if(kv_list[i].getKey() == name)
            {
                return true;
            }
        }

        return false;
    }

    QString getNode()
    {
        return name;
    }

    void setNode(QString name)
    {
        this->name = name;
    }

    int getKVCount()
    {
        return kv_list.count();
    }

    QStringList getKV(int id)
    {
        if(id > kv_list.count() - 1 || id < 0)
            return QStringList();

        if(kv_list[id].getKey().isEmpty())
            return QStringList();

        QStringList tmp;
        tmp += kv_list[id].getKey();
        tmp += kv_list[id].getValue().toString();
        return tmp;
    }

    QString getKey(int id)
    {
        if(id < kv_list.count() && id > 0)
        {
            return kv_list[id].getKey();
        }

        return QString("");
    }

    QVariant getValue(QString key)
    {
        for(int i = 0; i < kv_list.count(); i++)
        {
            if(kv_list[i].getKey() == key)
            {
                return kv_list[i].getValue();
            }
        }

        return QVariant("NULL");
    }

    QVariant getValue(int id)
    {
        if(id < kv_list.count() && id > 0)
        {
            return kv_list[id].getValue();
        }

        return QVariant("NULL");
    }

    QString getComment(QString key)
    {
        for(int i = 0; i < kv_list.count(); i++)
        {
            if(kv_list[i].getKey() == key)
            {
                return kv_list[i].getComment();
            }
        }

        return QString("");
    }

    QString getComment(int id)
    {
        if(id < kv_list.count() && id > 0)
        {
            return kv_list[id].getComment();
        }

        return QString("");
    }

    void setValue(QString key, QVariant value)
    {
        for(int i = 0; i < kv_list.count(); i++)
        {
            if(kv_list[i].getKey() == key)
            {
                kv_list[i].setValue(value);
                return;
            }
        }

        addKV(key, value, QString(""));
    }

    void setValue(QString key, QVariant value, QString comment)
    {
        for(int i = 0; i < kv_list.count(); i++)
        {
            if(kv_list[i].getKey() == key)
            {
                kv_list[i].setValue(value);
                kv_list[i].setComment(comment);
                return;
            }
        }

        addKV(key, value, comment);
    }

    void setValue(QString key)
    {
        for(int i = 0; i < kv_list.count(); i++)
        {
            if(kv_list[i].getKey() == key)
            {
                return;
            }
        }

        addKV(key, QVariant("NULL"), QString(""));
    }

    bool setComment(QString key, QString comment)
    {
        for(int i = 0; i < kv_list.count(); i++)
        {
            if(kv_list[i].getKey() == key)
            {
                kv_list[i].setComment(comment);
                return true;
            }
        }

        return false;
    }

    bool setComment(int id, QString comment)
    {
        for(int i = 0; i < id; i++)
        {
            kv_list[i].setComment(comment);
            return true;
        }

        return false;
    }

    ~IniNode()
    {
        kv_list.clear();
        /*
        for(int i = 0; i < MAX_KEYS; i++)
        {
            delete kv_list[i];
        }*/
    }
};

class CINI_Parse
{
private:
    //IniNode *nodes[MAX_NODES];
    QList<IniNode> nodes;
    //int node_count;
    int cur_node;
    int cur_key;
    bool alt_mode;
    QFile *ini_file;
public:
    CINI_Parse(QString file, bool mode);
    CINI_Parse(QString file);
    CINI_Parse();
    QString getNode(int id);
    void save();
    void addNode(QString name);
    void setNode(QString name);
    bool setNode(int id);
    QString setNextNode();
    QVariant getValue(QString key);
    QString getComment(QString key);
    QString getComment(int id);
    QStringList getNext();
    void setValue(QString key, QVariant value);
    void setValue(QString key, QVariant value, QString comment);
    void setValue(QString key);
    void setComment(QString key, QString comment);
    void setComment(int id, QString comment);
    void setFile(QString file);

    ~CINI_Parse()
    {
        ini_file->close();
        delete ini_file;
        nodes.clear();
        /*
        for(int i = 0; i > MAX_NODES; i++)
        {
            delete nodes[i];
        }
        */
    }

private:
        bool open(QString file);
};

#endif // CINI_PARSE_H
