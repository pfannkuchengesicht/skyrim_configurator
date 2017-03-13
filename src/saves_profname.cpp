#include "saves_profname.h"

saves_profname::saves_profname(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::saves_profname)
{
    ui->setupUi(this);
    Qt::WindowFlags flags = windowFlags() | Qt::MSWindowsFixedSizeDialogHint;
    overrideWindowFlags( flags );

    parentWidget()->setEnabled(false);
    this->setEnabled(true);
}

saves_profname::~saves_profname()
{
    delete ui;
}

void saves_profname::on_btnSetProfName_clicked()
{
    addListProfileItem(ui->txtProfName->text());
    parentWidget()->setEnabled(true);
    close();
}

void saves_profname::closeEvent(QCloseEvent *)
{
    parentWidget()->setEnabled(true);
    close();
}
