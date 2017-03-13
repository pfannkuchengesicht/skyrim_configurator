#include "about.h"
#include "ui_about.h"
#include "version.h"

about::about(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::about)
{
    ui->setupUi(this);

    parent->setEnabled(false);
    this->setEnabled(true);

    ui->lblCopyright->setText(tr("Copyright ")+"2011 - 2012 Roy Freytag."+tr(" All Rights reserved."));
    ui->lblVersion->setText(QString(VER_PRODUCTVERSION_STR)+QString("\n(")+QString(__DATE__)+QString(", ")+QString(__TIME__)+QString(")"));
    ui->lblLang->setText(QLocale::system().name());
}

about::~about()
{
    delete ui;
}

void about::on_btnBox_accepted()
{
    this->close();
}
