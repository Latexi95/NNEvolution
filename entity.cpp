#include "entity.h"
#include "map.h"
#include <random>

extern std::mt19937 randgen;

Entity::Entity(Map *map) :
    mNeuralNetwork(PROPERTY_COUNT, 4 + 4 + 4 + 4),
    mMap(map),
    mX(0),
    mY(0),
    mAge(0)

{
    setEnergy(std::uniform_int_distribution<>(100, 150)(randgen));
    setHealth(std::uniform_int_distribution<>(30, 60)(randgen));
    setHydration(std::uniform_int_distribution<>(1000, 2000)(randgen));
}

void Entity::makeDefaultNeuralNetwork()
{
    mNeuralNetwork.makeDefaultConnection();
}

void Entity::initClone(Entity *entity)
{
    mNeuralNetwork = entity->mNeuralNetwork.clone(true);
    setEnergy(std::uniform_int_distribution<>(100, 150)(randgen));
    setHealth(std::uniform_int_distribution<>(30, 60)(randgen));
    setHydration(std::uniform_int_distribution<>(1000, 2000)(randgen));
}


void Entity::run()
{
    setHeatLevel(mMap->cell(mX, mY).mHeatLevel / 255.0);


    int index = 0;
    for (int y = -1; y <= 1; ++y) {
        for (int x = -1; x <= 1; ++x) {
            const Cell &cell = mMap->safeCell(mX + x, mY + y);
            mNeuralNetwork.setInputValue(M_FOOD_LEVEL_PROPERTY_START + index, cell.mFoodLevelM / 255.0);
            mNeuralNetwork.setInputValue(V_FOOD_LEVEL_PROPERTY_START + index, cell.mFoodLevelV / 255.0);
            mNeuralNetwork.setInputValue(ENTITY_HEALTH_PROPERTY_START + index, cell.mEntity ? cell.mEntity->getHealth() : 0);
            mNeuralNetwork.setInputValue(WATER_LEVEL_PROPERTY_START + index, cell.mWaterLevel / 255.0);
            ++index;
        }
    }

    mNeuralNetwork.run();


    const std::vector<NNType> &output = mNeuralNetwork.outputValues();
    NNType moveLeft = output[0];
    NNType moveRight = output[1];
    NNType moveUp = output[2];
    NNType moveDown = output[3];

    NNType attackLeft = output[4];
    NNType attackRight = output[5];
    NNType attackUp = output[6];
    NNType attackDown = output[7];

    NNType eatV = output[8];
    NNType eatM = output[9];
    NNType drink = output[10];
    NNType split = output[11];

    setValueStore1(output[12]);
    setValueStore2(output[13]);
    setValueStore3(output[14]);
    setValueStore4(output[15]);

    NNType bestVal = 0;
    Action::Type actionType = Action::None;
#define MAX_ACTION(var, actionEnum) if (var > bestVal) { bestVal = var; actionType = actionEnum; }

    MAX_ACTION(moveLeft, Action::MoveLeft);
    MAX_ACTION(moveRight, Action::MoveRight);
    MAX_ACTION(moveUp, Action::MoveUp);
    MAX_ACTION(moveDown, Action::MoveDown);

    MAX_ACTION(attackLeft, Action::AttackLeft);
    MAX_ACTION(attackRight, Action::AttackRight);
    MAX_ACTION(attackUp, Action::AttackUp);
    MAX_ACTION(attackDown, Action::AttackDown);

    MAX_ACTION(eatV, Action::EatV);
    MAX_ACTION(eatM, Action::EatM);
    MAX_ACTION(drink, Action::Drink);
    MAX_ACTION(split, Action::Split);

    ++mAge;

    mLastAction.mType = actionType;
}

void Entity::postUpdate()
{

    setHydration(getHydration() - (20 * pow2(getHeatLevel() / 255.0)));
    if (getHydration() <= 50) {
        takeEnergy(2.0);
    }
    else {
        takeEnergy(1.0);
    }
    if (getEnergy() <= 0.1) {
        takeHealth(2);
    }
    if (getHydration() <= 0.1) {
        takeHealth(2);
    }

}

bool Entity::shouldBeDeleted()
{
    return getHealth() <= 0;
}

int Entity::x() const
{
    return mX;
}

void Entity::setX(int x)
{
    mX = x;
}
int Entity::y() const
{
    return mY;
}

void Entity::setY(int y)
{
    mY = y;
}
Action Entity::lastAction() const
{
    return mLastAction;
}

NNType Entity::takeEnergy(NNType e)
{
    NNType t = getEnergy();
    if (t > e) {
        setEnergy(t - e);
        return e;
    }
    else {
        setEnergy(0);
        return t;
    }
}

NNType Entity::takeHealth(NNType e)
{
    NNType t = getHealth();
    if (t > e) {
        setHealth(t - e);
        return e;
    }
    else {
        setHealth(0);
        return t;
    }
}
int Entity::age() const
{
    return mAge;
}





