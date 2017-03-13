#ifndef EXPORT_SELECTION_H
#define EXPORT_SELECTION_H

#include <QWidget>

namespace Ui {
class export_selection;
}

class export_selection : public QWidget
{
    Q_OBJECT
    
public:
    explicit export_selection(QWidget *parent = 0);
    ~export_selection();
    
private:
    Ui::export_selection *ui;
};

#endif // EXPORT_SELECTION_H
