#include "export_selection.h"
#include "ui_export_selection.h"

export_selection::export_selection(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::export_selection)
{
    ui->setupUi(this);
}

export_selection::~export_selection()
{
    delete ui;
}
