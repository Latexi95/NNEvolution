#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <chrono>
#include <QtConcurrent/QtConcurrent>
#include <QImage>
#include <random>

#define MIN_ENTITY_COUNT 10000
#define MAX_ENTITY_COUNT 500000
extern std::mt19937 randgen;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mIteration(0)
{
    ui->setupUi(this);

    Map *map = new Map(QImage("../NNEvolution/map.png"));
    ui->mapViewWidget->setMap(map);
    mUpdateTimer.start(0, this);

    std::uniform_int_distribution<> xDist(0, map->width() - 1);
    std::uniform_int_distribution<> yDist(0, map->height() - 1);

    mEntities.emplace_back(map);
    Entity &firstEntity = mEntities.back();
    firstEntity.makeDefaultNeuralNetwork();
    int x = xDist(randgen);
    int y = yDist(randgen);
    Cell &c = map->cell(x, y);
    firstEntity.setX(x);
    firstEntity.setY(y);
    c.mEntity = &firstEntity;
    for (int i = 0; i < MIN_ENTITY_COUNT / 2; ++i) {
        mEntities.emplace_back(map);
        Entity &entity = mEntities.back();
        entity.initClone(&firstEntity);
        while (true) {
            int x = xDist(randgen);
            int y = yDist(randgen);
            Cell &c = map->cell(x, y);
            if (!c.mEntity) {
                entity.setX(x);
                entity.setY(y);
                c.mEntity = &entity;
                break;
            }
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == mUpdateTimer.timerId()) {
        tick();
    }
}

void MainWindow::tick()
{
    auto startTime = std::chrono::high_resolution_clock::now();
    Map *map = ui->mapViewWidget->map();

    map->generate();
    QtConcurrent::map(mEntities, [](Entity &entity) {
        entity.run();
    }).waitForFinished();
    std::list<Entity> newEntities;
    int eating = 0;
    int attacking = 0;
    int moving = 0;
    int drinking = 0;
    int idling = 0;
    for (Entity &entity : mEntities) {
        Action action = entity.lastAction();
        switch (action.mType) {
            case Action::MoveUp:
                entity.setActionResult(map->moveEntity(&entity, entity.x(), entity.y() - 1) ? 10.0 : 0); ++moving; break;
            case Action::MoveDown:
                entity.setActionResult(map->moveEntity(&entity, entity.x(), entity.y() + 1) ? 10.0 : 0); ++moving; break;
            case Action::MoveLeft:
                entity.setActionResult(map->moveEntity(&entity, entity.x() - 1, entity.y()) ? 10.0 : 0); ++moving; break;
            case Action::MoveRight:
                entity.setActionResult(map->moveEntity(&entity, entity.x() + 1, entity.y()) ? 10.0 : 0); ++moving; break;
            case Action::AttackLeft:
                entity.setActionResult(map->attackEntity(&entity, entity.x() - 1, entity.y())); ++attacking; break;
            case Action::AttackRight:
                entity.setActionResult(map->attackEntity(&entity, entity.x() + 1, entity.y())); ++attacking; break;
            case Action::AttackUp:
                entity.setActionResult(map->attackEntity(&entity, entity.x(), entity.y() - 1)); ++attacking; break;
            case Action::AttackDown:
                entity.setActionResult(map->attackEntity(&entity, entity.x(), entity.y() + 1)); ++attacking; break;
            case Action::EatM:
                entity.setActionResult(map->eatM(&entity));++eating; break;
            case Action::EatV:
                entity.setActionResult(map->eatV(&entity));++eating; break;
            case Action::Drink:
                entity.setActionResult(map->drink(&entity));++drinking; break;
            case Action::Split: {
                NNType en = entity.takeEnergy(100);
                if (en > 50) {
                    entity.setActionResult(1);
                    newEntities.emplace_back(map);
                    Entity &newEntity = newEntities.back();
                    newEntity.initClone(&entity);
                    newEntity.setX(entity.x());
                    newEntity.setY(entity.y());
                    newEntity.setEnergy(en * 0.9);
                }
                else {
                    entity.setActionResult(0);
                }
                break;
            }
            case Action::None:
                ++idling;
                break;
        }
    }

    Entity *bestEntity = 0;
    Entity *cloneBaseEntity = 0;

    int oldest = 0;

    int deleted = 0;
    int entityCount = 0;

    int cloneBaseEntityIndex = std::uniform_int_distribution<>(0, MIN_ENTITY_COUNT / 2)(randgen);
    for (auto entityIt = mEntities.begin(); entityIt != mEntities.end();) {
        entityIt->postUpdate();
        if (entityIt->shouldBeDeleted()) {
            Cell &c = map->cell(&*entityIt);
            c.mFoodLevelM = std::min(c.mFoodLevelM + entityIt->getEnergy() + 60.0, (double)0xFFFF);
            c.mEntity = 0;
            entityIt = mEntities.erase(entityIt);
            ++deleted;
        }
        else {
            ++entityCount;
            if (entityIt->age() > oldest) {
                oldest = entityIt->age();
                bestEntity = &*entityIt;
            }
            if (entityCount == cloneBaseEntityIndex) {
                cloneBaseEntity = &*entityIt;
            }
            ++entityIt;
        }
    }

    if (entityCount == 0) {
        qDebug("all dead");
    }


    int splits = 0;
    if (!newEntities.empty() && entityCount < MAX_ENTITY_COUNT) {
        std::uniform_int_distribution<> xDist(-5, 5);
        std::uniform_int_distribution<> yDist(-5, 5);
        auto entityIt = newEntities.begin();
        mEntities.splice(mEntities.end(), newEntities);
        for (; entityIt != mEntities.end();) {
            int i = 0;
            for (; i < 10; ++i) {
                int x = entityIt->x() + xDist(randgen);
                int y = entityIt->y() + yDist(randgen);
                if (!map->pointOnMap(x, y)) continue;
                Cell &c = map->cell(x, y);
                if (!c.mEntity) {
                    entityIt->setX(x);
                    entityIt->setY(y);
                    c.mEntity = &*entityIt;
                    break;
                }
            }
            if (i == 10) {
                entityIt = mEntities.erase(entityIt);
            }
            else {
                ++entityIt;
            }
            ++splits;
        }
    }

    if (entityCount < MIN_ENTITY_COUNT && cloneBaseEntity) {
        std::uniform_int_distribution<> xDist(0, map->width() - 1);
        std::uniform_int_distribution<> yDist(0, map->height() - 1);
        for (int i = 0; i < 100; ++i) {
            mEntities.emplace_back(map);
            Entity &entity = mEntities.back();
            entity.initClone(bestEntity);
            while (true) {
                int x = xDist(randgen);
                int y = yDist(randgen);
                Cell &c = map->cell(x, y);
                if (!c.mEntity) {
                    entity.setX(x);
                    entity.setY(y);
                    c.mEntity = &entity;
                    break;
                }
            }
        }
    }


    auto endTime = std::chrono::high_resolution_clock::now();

    ++mIteration;
    if (ui->actionUpdate_statusbar->isChecked()) {
        ui->statusBar->showMessage(tr("%10 Update time: %1ms  Entity count: %2  Deleted: %3  Oldest: %4  Splits: %5  Eating: %6  Drinking: %7  Attacking: %8 Moving: %9")
                                   .arg(std::chrono::duration<double>(endTime - startTime).count() * 1000.0)
                                   .arg(entityCount)
                                   .arg(deleted)
                                   .arg(oldest)
                                   .arg(splits)
                                   .arg(eating)
                                   .arg(drinking)
                                   .arg(attacking)
                                   .arg(moving)
                                   .arg(mIteration)
                                   );
    }
    if (ui->actionUpdate_screen->isChecked()) ui->mapViewWidget->repaint();
}
