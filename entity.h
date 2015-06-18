#ifndef ENTITY_H
#define ENTITY_H
#include "neuralnetwork.h"
#include <memory>
#include "action.h"
#include <QRunnable>

#define NNPROPERTY(_name_, _index_) void set##_name_ (NNType v) { mNeuralNetwork.setInputValue(_index_, v); } NNType get##_name_ () const { return mNeuralNetwork.inputValue(_index_); }

class Map;

#define V_FOOD_LEVEL_PROPERTY_START 9
#define M_FOOD_LEVEL_PROPERTY_START 18
#define WATER_LEVEL_PROPERTY_START 27
#define ENTITY_HEALTH_PROPERTY_START 26

#define PROPERTY_COUNT 36

class Entity
{
public:
    Entity(Map *map);

    void makeDefaultNeuralNetwork();

    void initClone(Entity *entity);

    NNPROPERTY(Health, 0)
    NNPROPERTY(Energy, 1)
    NNPROPERTY(HeatLevel, 2)
    NNPROPERTY(ActionResult, 3)
    NNPROPERTY(Hydration, 4)
    NNPROPERTY(ValueStore1, 5)
    NNPROPERTY(ValueStore2, 6)
    NNPROPERTY(ValueStore3, 7)
    NNPROPERTY(ValueStore4, 8)

#define V_FOOD_LEVEL_PROPERTY_START 9
#define M_FOOD_LEVEL_PROPERTY_START 18
#define WATER_LEVEL_PROPERTY_START 27
#define ENTITY_HEALTH_PROPERTY_START 26


    void run();
    void postUpdate();
    bool shouldBeDeleted();

    int x() const;
    void setX(int x);

    int y() const;
    void setY(int y);

    Action lastAction() const;

    NNType takeEnergy(NNType e);
    NNType takeHealth(NNType e);
    int age() const;

private:
    NeuralNetwork mNeuralNetwork;
    Action mLastAction;
    Map *mMap;
    int mX, mY;
    int mAge;
};

#endif // ENTITY_H
