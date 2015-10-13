#include "entityinfoform.h"
#include "ui_entityinfoform.h"

EntityInfoForm::EntityInfoForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EntityInfoForm)
{
    ui->setupUi(this);
}

EntityInfoForm::~EntityInfoForm()
{
    delete ui;
}

void EntityInfoForm::updateInfo(const Entity &entity)
{
    ui->healthLabel->setText(QString::number(entity.getHealth()));
    ui->ageLabel->setText(QString::number(entity.age()));
    ui->energyLabel->setText(QString::number(entity.getEnergy()));
    ui->hydrationLabel->setText(QString::number(entity.getHydration()));
    ui->heatLevelLabel->setText(QString::number(entity.getHeatLevel()));
    ui->posLabel->setText(tr("%1, %2").arg(entity.x()).arg(entity.y()));
    ui->generationLabel->setText(QString::number(entity.generation()));
}
