#ifndef ACTION_H_
#define ACTION_H_

struct Action {
    enum Type {
        None = 0,
        MoveUp,
        MoveDown,
        MoveLeft,
        MoveRight,
        AttackUp,
        AttackDown,
        AttackLeft,
        AttackRight,
        EatV,
        EatM,
        Drink,
        Split
    } mType;

    double mParam;
};


#endif // ACTION_H_

