#ifndef NNVIEWWIDGET_H
#define NNVIEWWIDGET_H

#include <QWidget>
#include "neuralnetwork.h"
class NNViewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NNViewWidget(QWidget *parent = 0);

    NeuralNetwork neuralNetwork() const;
    void setNeuralNetwork(const NeuralNetwork &value);

    QSize sizeHint() const;
signals:

public slots:

private:
    void paintEvent(QPaintEvent *e);
    void updateSize();

    NeuralNetwork mNeuralNetwork;
};

#endif // NNVIEWWIDGET_H
