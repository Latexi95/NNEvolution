#include "map.h"
#include <QColor>
#include <QDebug>
#include "entity.h"


Map::Map(const QImage &img)
{
    mMaxFoodLevel = 0;
    mMaxWaterLevel = 0;
    qDebug() << img;
    mWidth = img.width();
    mHeight = img.height();
    mData.resize(mWidth * mHeight);
    for (int y = 0; y != mHeight; ++y) {
        for (int x = 0; x != mWidth; ++x) {
            QColor color = QColor::fromRgb(img.pixel(x, y));
            Cell &c = cell(x, y);
            c.mFoodVGeneration = color.green();
            c.mHeatLevel = color.red();
            c.mWaterGeneration = color.blue();
            c.mFoodLevelM = 50;
            c.mFoodLevelV = c.mFoodVGeneration;
            c.mWaterLevel = pow2(c.mWaterGeneration);
        }
    }
    mRenderBuffer = QImage(mWidth, mHeight, QImage::Format_RGB32);
}

Cell &Map::cell(int x, int y)
{
    return mData[x + mWidth * y];
}

const Cell &Map::cell(int x, int y) const
{
    return mData[x + mWidth * y];
}

const Cell &Map::cell(Entity *entity) const
{
    return cell(entity->x(), entity->y());
}

Cell &Map::cell(Entity *entity)
{
    return cell(entity->x(), entity->y());
}

const Cell &Map::safeCell(int x, int y) const
{
    if (x < 0 || x >= mWidth || y < 0 || y >= mHeight) {
        return mOutsideCell;
    }
    return cell(x, y);
}



void Map::generate()
{

    for (Cell &c : mData) {
        c.mFoodLevelV = (quint16)std::min((c.mFoodLevelV + ((!c.mEntity ? 40 : 0) + c.mFoodVGeneration) / 40.0) * (0.95 + 0.05 * pow2(c.mFoodVGeneration / 255.0)), (double)0xFFFF);
        c.mFoodLevelM = c.mFoodLevelM * (0.95 - 0.3 * pow2(c.mHeatLevel / 255.0));
        c.mWaterLevel = (quint16)std::min(c.mWaterLevel * (0.95 - 0.3 * pow2(c.mHeatLevel / 255.0)) + c.mWaterGeneration, (double)0xFFFF);

        mMaxFoodLevel = std::max(mMaxFoodLevel, c.mFoodLevelV);
        mMaxWaterLevel = std::max(mMaxWaterLevel, c.mWaterLevel);
    }
}

quint16 Map::maxFoodLevel() const
{
    return mMaxFoodLevel;
}

const QImage &Map::render()
{
    for (int y = 0; y != mHeight; ++y) {
        for (int x = 0; x != mWidth; ++x) {
            const Cell &c = cell(x, y);

            mRenderBuffer.setPixel(x, y, qRgb(c.mEntity ? 255.0 : 0, 255.0 * (c.mFoodLevelV / (double)mMaxFoodLevel), 255.0 * (c.mWaterLevel / (double)mMaxWaterLevel)));
        }
    }
    //qDebug() << mMaxFoodLevel << mMaxWaterLevel;

    return mRenderBuffer;
}
const Cell &Map::outsideCell() const
{
    return mOutsideCell;
}

void Map::setOutsideCell(const Cell &outsideCell)
{
    mOutsideCell = outsideCell;
}

bool Map::moveEntity(Entity *entity, int x, int y)
{
    if (x < 0 || x >= mWidth || y < 0 || y >= mHeight) {
        return false;
    }
    Cell &c = cell(x, y);
    if (c.mEntity) return false;

    c.mEntity = entity;
    cell(entity->x(), entity->y()).mEntity = 0;
    entity->setX(x);
    entity->setY(y);
    entity->takeEnergy(1.0);
    return true;
}

NNType Map::attackEntity(Entity *entity, int x, int y)
{
    entity->takeEnergy(5);
    if (x < 0 || x >= mWidth || y < 0 || y >= mHeight) {
        return -1;
    }
    const Cell &c = cell(x, y);
    Entity *target = c.mEntity;
    if (!target) {
        return 0;
    }


    return target->takeHealth(std::max(entity->takeEnergy(20) - target->takeEnergy(10), 0.0));
}

NNType Map::eatV(Entity *entity)
{
    Cell &c = cell(entity->x(), entity->y());

    const int eatAmount = 50;
    int eaten = eatAmount;
    if (c.mFoodLevelV > eatAmount) {
        c.mFoodLevelV -= eatAmount;
    }
    else {
        eaten = c.mFoodLevelV;
        c.mFoodLevelV = 0;
    }

    entity->setEnergy(entity->getEnergy() - 1.0 + 0.5 * eaten);
    return eaten;
}

NNType Map::eatM(Entity *entity)
{
    Cell &c = cell(entity->x(), entity->y());

    const int eatAmount = 50;
    int eaten = eatAmount;
    if (c.mFoodLevelM > eatAmount) {
        c.mFoodLevelM -= eatAmount;
    }
    else {
        eaten = c.mFoodLevelM;
        c.mFoodLevelM = 0;
    }

    entity->setEnergy(entity->getEnergy() - 1.0 + 0.5 * eaten);
    return eaten;
}

NNType Map::drink(Entity *entity)
{
    Cell &c = cell(entity->x(), entity->y());

    const int drinkAmount = 200;
    int drank = drinkAmount;
    if (c.mWaterLevel > drinkAmount) {
        c.mWaterLevel -= drinkAmount;
    }
    else {
        drank = c.mWaterLevel;
        c.mWaterLevel = 0;
    }

    entity->takeEnergy(2.0);

    entity->setHydration(entity->getHydration() + drank);
    return drank;
}
int Map::width() const
{
    return mWidth;
}
int Map::height() const
{
    return mHeight;
}

bool Map::pointOnMap(int x, int y) const
{
    return !(x < 0 || x >= mWidth || y < 0 || y >= mHeight);
}






