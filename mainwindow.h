#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QBasicTimer>
#include "entity.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void timerEvent(QTimerEvent *e);
    void tick();

    Ui::MainWindow *ui;
    QBasicTimer mUpdateTimer;
    std::list<Entity> mEntities;
    quint64 mIteration;
};

#endif // MAINWINDOW_H
