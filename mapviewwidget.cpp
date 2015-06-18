#include "mapviewwidget.h"
#include <QPainter>

MapViewWidget::MapViewWidget(QWidget *parent) : QWidget(parent)
{

}

void MapViewWidget::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    p.drawImage(0, 0, mMap->render());
}
Map *MapViewWidget::map() const
{
    return mMap;
}

void MapViewWidget::setMap(Map *map)
{
    mMap = map;
}


