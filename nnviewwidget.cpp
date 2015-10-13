#include "nnviewwidget.h"
#include <QPainter>
#include <QCoreApplication>
static QSize sTextBoxSpacing = QSize(80, 20);
static QSize sNeuronBoxSize = QSize(10, 10);
static QSize sSpacing = QSize(80, 10);

static QString sInputNeuronNames[] = {
    QCoreApplication::translate("NNViewWidget", "Health"),
    QCoreApplication::translate("NNViewWidget", "Energy"),
    QCoreApplication::translate("NNViewWidget", "HeatLevel"),
    QCoreApplication::translate("NNViewWidget", "ActionResult"),
    QCoreApplication::translate("NNViewWidget", "Hydration"),
    QCoreApplication::translate("NNViewWidget", "ValueStore1"),
    QCoreApplication::translate("NNViewWidget", "ValueStore2"),
    QCoreApplication::translate("NNViewWidget", "ValueStore3"),
    QCoreApplication::translate("NNViewWidget", "ValueStore4"),

    QCoreApplication::translate("NNViewWidget", "V Food Level UL"),
    QCoreApplication::translate("NNViewWidget", "V Food Level UC"),
    QCoreApplication::translate("NNViewWidget", "V Food Level UR"),
    QCoreApplication::translate("NNViewWidget", "V Food Level CL"),
    QCoreApplication::translate("NNViewWidget", "V Food Level CC"),
    QCoreApplication::translate("NNViewWidget", "V Food Level CR"),
    QCoreApplication::translate("NNViewWidget", "V Food Level DL"),
    QCoreApplication::translate("NNViewWidget", "V Food Level DC"),
    QCoreApplication::translate("NNViewWidget", "V Food Level DR"),

    QCoreApplication::translate("NNViewWidget", "M Food Level UL"),
    QCoreApplication::translate("NNViewWidget", "M Food Level UC"),
    QCoreApplication::translate("NNViewWidget", "M Food Level UR"),
    QCoreApplication::translate("NNViewWidget", "M Food Level CL"),
    QCoreApplication::translate("NNViewWidget", "M Food Level CC"),
    QCoreApplication::translate("NNViewWidget", "M Food Level CR"),
    QCoreApplication::translate("NNViewWidget", "M Food Level DL"),
    QCoreApplication::translate("NNViewWidget", "M Food Level DC"),
    QCoreApplication::translate("NNViewWidget", "M Food Level DR"),

    QCoreApplication::translate("NNViewWidget", "Water Level UL"),
    QCoreApplication::translate("NNViewWidget", "Water Level UC"),
    QCoreApplication::translate("NNViewWidget", "Water Level UR"),
    QCoreApplication::translate("NNViewWidget", "Water Level CL"),
    QCoreApplication::translate("NNViewWidget", "Water Level CC"),
    QCoreApplication::translate("NNViewWidget", "Water Level CR"),
    QCoreApplication::translate("NNViewWidget", "Water Level DL"),
    QCoreApplication::translate("NNViewWidget", "Water Level DC"),
    QCoreApplication::translate("NNViewWidget", "Water Level DR"),

    QCoreApplication::translate("NNViewWidget", "Health Level UL"),
    QCoreApplication::translate("NNViewWidget", "Health Level UC"),
    QCoreApplication::translate("NNViewWidget", "Health Level UR"),
    QCoreApplication::translate("NNViewWidget", "Health Level CL"),
    QCoreApplication::translate("NNViewWidget", "Health Level CC"),
    QCoreApplication::translate("NNViewWidget", "Health Level CR"),
    QCoreApplication::translate("NNViewWidget", "Health Level DL"),
    QCoreApplication::translate("NNViewWidget", "Health Level DC"),
    QCoreApplication::translate("NNViewWidget", "Health Level DR"),

    QCoreApplication::translate("NNViewWidget", "Health Level UL"),
    QCoreApplication::translate("NNViewWidget", "Health Level UC"),
    QCoreApplication::translate("NNViewWidget", "Health Level UR"),
    QCoreApplication::translate("NNViewWidget", "Health Level CL"),
    QCoreApplication::translate("NNViewWidget", "Health Level CC"),
    QCoreApplication::translate("NNViewWidget", "Health Level CR"),
    QCoreApplication::translate("NNViewWidget", "Health Level DL"),
    QCoreApplication::translate("NNViewWidget", "Health Level DC"),
    QCoreApplication::translate("NNViewWidget", "Health Level DR"),


    QCoreApplication::translate("NNViewWidget", "MoveLeft"),
    QCoreApplication::translate("NNViewWidget", "MoveRight"),
    QCoreApplication::translate("NNViewWidget", "MoveUp"),
    QCoreApplication::translate("NNViewWidget", "MoveDown"),
    QCoreApplication::translate("NNViewWidget", "AttackLeft"),
    QCoreApplication::translate("NNViewWidget", "AttackRight"),
    QCoreApplication::translate("NNViewWidget", "AttackUp"),
    QCoreApplication::translate("NNViewWidget", "AttackDown"),
    QCoreApplication::translate("NNViewWidget", "EatV"),
    QCoreApplication::translate("NNViewWidget", "EatM"),
    QCoreApplication::translate("NNViewWidget", "Drink"),
    QCoreApplication::translate("NNViewWidget", "Split")
};

static QString sOutputNeuronNames[] = {
    QCoreApplication::translate("NNViewWidget", "Move left"),
    QCoreApplication::translate("NNViewWidget", "Move right"),
    QCoreApplication::translate("NNViewWidget", "Move up"),
    QCoreApplication::translate("NNViewWidget", "Move down"),
    QCoreApplication::translate("NNViewWidget", "Attack left"),
    QCoreApplication::translate("NNViewWidget", "Attack right"),
    QCoreApplication::translate("NNViewWidget", "Attack up"),
    QCoreApplication::translate("NNViewWidget", "Attack down"),
    QCoreApplication::translate("NNViewWidget", "Eat V"),
    QCoreApplication::translate("NNViewWidget", "Eat M"),
    QCoreApplication::translate("NNViewWidget", "Drink"),
    QCoreApplication::translate("NNViewWidget", "Split"),

    QCoreApplication::translate("NNViewWidget", "ValueStore1"),
    QCoreApplication::translate("NNViewWidget", "ValueStore2"),
    QCoreApplication::translate("NNViewWidget", "ValueStore3"),
    QCoreApplication::translate("NNViewWidget", "ValueStore4"),
};

NNViewWidget::NNViewWidget(QWidget *parent) :
    QWidget(parent)
{
}
NeuralNetwork NNViewWidget::neuralNetwork() const
{
    return mNeuralNetwork;
}

void NNViewWidget::setNeuralNetwork(const NeuralNetwork &value)
{
    mNeuralNetwork = value;
    updateSize();
}

QSize NNViewWidget::sizeHint() const
{
    if (mNeuralNetwork.outputValues().empty()) {
        return QSize();
    }
    unsigned maxNeurons = 0;
    int levels = mNeuralNetwork.levels().size() - 1;

    for (const Level &lvl : mNeuralNetwork.levels()) {
        maxNeurons = std::max(lvl.mNeuronCount, maxNeurons);
    }

    QSize size = sNeuronBoxSize + sSpacing;
    size.rwidth() *= levels;
    size.rheight() *= maxNeurons;
    size.rwidth() += sTextBoxSpacing.width() * 2;

    size.setWidth(std::max(sTextBoxSpacing.height() * (int)mNeuralNetwork.levels().front().mNeuronCount, size.width()));
    return size;
}

void NNViewWidget::paintEvent(QPaintEvent *e)
{
    const Level &inputLvl = mNeuralNetwork.levels().front();
    QPainter p(this);

    for (unsigned i = 0; i < inputLvl.mNeuronCount; ++i) {
        p.drawText(QRect(QPoint(3,
                                3 + i * sTextBoxSpacing.height()),
                         sTextBoxSpacing),
                   Qt::AlignRight,
                   sInputNeuronNames[i]);
    }
    int x = 3 + sTextBoxSpacing.width();
    bool inputLvlCur = true;
    for (const Level &lvl : mNeuralNetwork.levels()) {
        for (const NNConnection &con : lvl.mConnections) {
            QPoint p1;
            if (inputLvlCur) {
                p1 = QPoint(x, con.mSrcNeuronIndex * sTextBoxSpacing.height() + sTextBoxSpacing.height() / 2);
            }
            else {
                p1 = QPoint(x, con.mSrcNeuronIndex * (sNeuronBoxSize.height() + sSpacing.height()) + sNeuronBoxSize.height() / 2);
            }
            p.drawLine(
                        p1,
                        QPoint(x + sSpacing.width(), con.mDestNeuronIndex * (sNeuronBoxSize.height() + sSpacing.height()) + sNeuronBoxSize.height() / 2)
                        );
        }

        for (unsigned neuronIndex = 0; neuronIndex < lvl.mNeuronCount; ++neuronIndex) {
            if (!inputLvlCur) {
                p.fillRect(QRect(QPoint(x - sNeuronBoxSize.width(), neuronIndex * (sNeuronBoxSize.height() + sSpacing.height())),
                                 sNeuronBoxSize), QColor(150, 0, 0));
            }
        }

        x += sNeuronBoxSize.width() + sSpacing.width();
        inputLvlCur = false;
    }
    for (unsigned i = 0; i < mNeuralNetwork.outputValueSize(); ++i) {
        p.drawText(QRect(QPoint(x - sNeuronBoxSize.width(),
                                3 + i * sTextBoxSpacing.height()),
                         sTextBoxSpacing),
                   Qt::AlignLeft,
                   sOutputNeuronNames[i]);
    }
}

void NNViewWidget::updateSize()
{
    QSize size = sizeHint();
    setGeometry(QRect(QPoint(0,0), size));
    setMinimumSize(size);
}

