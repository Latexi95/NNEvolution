#ifndef MAPVIEWWIDGET_H
#define MAPVIEWWIDGET_H
#include <QWidget>
#include "map.h"

class MapViewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MapViewWidget(QWidget *parent = 0);

    Map *map() const;
    void setMap(Map *map);

signals:

public slots:

private:
    void paintEvent(QPaintEvent *e);

    Map *mMap;
};

#endif // MAPVIEWWIDGET_H
