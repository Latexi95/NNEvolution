#ifndef MAP_H
#define MAP_H
#include "defs.h"
#include <vector>
#include <QImage>

class Entity;
struct Cell {
    Cell() : mFoodLevelV(0), mFoodLevelM(0), mFoodVGeneration(0), mHeatLevel(255), mWaterLevel(0), mWaterGeneration(0), mEntity(0) {}
    quint16 mFoodLevelV;
    quint16 mFoodLevelM;
    quint8 mFoodVGeneration;
    quint8 mHeatLevel;
    quint16 mWaterLevel;
    quint8 mWaterGeneration;
    Entity *mEntity;
};

class Map {
public:
    Map(const QImage &img);


    Cell &cell(int x, int y);
    const Cell &cell(int x, int y) const;
    Cell &cell(Entity *entity);
    const Cell &cell(Entity *entity) const;

    const Cell &safeCell(int x, int y) const;

    void generate();

    quint16 maxFoodLevel() const;

    const QImage &render();

    const Cell &outsideCell() const;
    void setOutsideCell(const Cell &outsideCell);

    bool moveEntity(Entity *entity, int x, int y);
    NNType attackEntity(Entity *entity, int x, int y);

    NNType eatV(Entity *entity);
    NNType eatM(Entity *entity);
    NNType drink(Entity *entity);
    int width() const;

    int height() const;

    bool pointOnMap(int x, int y) const;

private:
    Cell mOutsideCell;
    QImage mRenderBuffer;
    std::vector<Cell> mData;
    int mWidth, mHeight;
    quint16 mMaxFoodLevel;
    quint16 mMaxWaterLevel;
};

#endif // MAP_H
