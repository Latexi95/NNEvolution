#ifndef ENTITYINFOFORM_H
#define ENTITYINFOFORM_H

#include <QWidget>
#include "entity.h"
namespace Ui {
class EntityInfoForm;
}

class EntityInfoForm : public QWidget
{
    Q_OBJECT

public:
    explicit EntityInfoForm(QWidget *parent = 0);
    ~EntityInfoForm();
    void updateInfo(const Entity &entity);
private:
    Ui::EntityInfoForm *ui;
};

#endif // ENTITYINFOFORM_H
