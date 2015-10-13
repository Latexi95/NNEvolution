#include "entity.h"
#include "map.h"
#include <random>

extern std::mt19937 randgen;

Entity::Entity(Map *map) :
    mNeuralNetwork(PROPERTY_COUNT, 4 + 4 + 4 + 4),
    mMap(map),
    mX(0),
    mY(0),
    mAge(1),
    mGeneration(0)

{
    setEnergy(std::uniform_int_distribution<>(50, 100)(randgen));
    setHealth(std::uniform_int_distribution<>(30, 60)(randgen));
    setHydration(std::uniform_int_distribution<>(1000, 2000)(randgen));
    setValueStore1(0);
    setValueStore2(0);
    setValueStore3(0);
    setValueStore4(0);
}

void Entity::makeDefaultNeuralNetwork()
{
    mNeuralNetwork.makeDefaultConnection();
}

void Entity::initClone(Entity *entity)
{
    mNeuralNetwork = entity->mNeuralNetwork.clone(true);
    setEnergy(std::uniform_int_distribution<>(50, 100)(randgen));
    setHealth(std::uniform_int_distribution<>(30, 60)(randgen));
    setHydration(std::uniform_int_distribution<>(1000, 2000)(randgen));
    setValueStore1(0);
    setValueStore2(0);
    setValueStore3(0);
    setValueStore4(0);
    mGeneration = entity->generation() + 1;
}


void Entity::run()
{
    if (age() == 1 && generation() > 50 && generation() % 50 == 0) {
        mNeuralNetwork.optimize();
    }

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

    setActionMoveLeft(moveLeft);
    setActionMoveRight(moveRight);
    setActionMoveUp(moveUp);
    setActionMoveDown(moveDown);
    setActionAttackLeft(attackLeft);
    setActionAttackRight(attackRight);
    setActionAttackUp(attackUp);
    setActionAttackDown(attackDown);
    setActionEatV(eatV);
    setActionEatM(eatM);
    setActionDrink(drink);
    setActionSplit(split);

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

    setHydration(getHydration() - (20 * getHeatLevel()));
    if (getHydration() <= 100) {
        takeEnergy(2.0 * (1.0 + 100.0 / age()));
    }
    else {
        takeEnergy(1.0 * (1.0 + 100.0 / age()));
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

int Entity::generation() const
{
    return mGeneration;
}

const NeuralNetwork &Entity::neuralNetwork() const
{
    return mNeuralNetwork;
}

void Entity::save(QDataStream &s, int version)
{
    s << mNeuralNetwork;
    s << mX;
    s << mY;
    s << mAge;
    s << mGeneration;
    s << getHealth();
    s << getEnergy();
    s << getHydration();
    s << getValueStore1();
    s << getValueStore2();
    s << getValueStore3();
    s << getValueStore4();

    s << getActionMoveLeft();
    s << getActionMoveRight();
    s << getActionMoveUp();
    s << getActionMoveDown();
    s << getActionAttackLeft();
    s << getActionAttackRight();
    s << getActionAttackUp();
    s << getActionAttackDown();
    s << getActionEatV();
    s << getActionEatM();
    s << getActionDrink();
    s << getActionSplit();
    s << getActionResult();
}

void Entity::load(QDataStream &s, int version)
{
    s >> mNeuralNetwork;
    s >> mX;
    s >> mY;
    s >> mAge;
    s >> mGeneration;

    NNType t;
    s >> t; setHealth(t);
    s >> t; setEnergy(t);
    s >> t; setHydration(t);
    s >> t; setValueStore1(t);
    s >> t; setValueStore2(t);
    s >> t; setValueStore3(t);
    s >> t; setValueStore4(t);

    s >> t; setActionMoveLeft(t);
    s >> t; setActionMoveRight(t);
    s >> t; setActionMoveUp(t);
    s >> t; setActionMoveDown(t);
    s >> t; setActionAttackLeft(t);
    s >> t; setActionAttackRight(t);
    s >> t; setActionAttackUp(t);
    s >> t; setActionAttackDown(t);
    s >> t; setActionEatV(t);
    s >> t; setActionEatM(t);
    s >> t; setActionDrink(t);
    s >> t; setActionSplit(t);
    s >> t; setActionResult(t);
}





