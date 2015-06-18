#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H
#include <QtGlobal>
#include <vector>
#include <array>
#include "defs.h"




struct NNNeuron {
    NNNeuron() : mValue(0) {}
    NNNeuron(NNType v) : mValue(v) {}
    NNType mValue;
};

struct NNConnection {
    NNConnection() : mDestNeuronIndex(0), mSrcNeuronIndex(0), mWeight(1) {}
    NNConnection(quint32 src, quint32 dest, NNType w) : mDestNeuronIndex(dest), mSrcNeuronIndex(src), mWeight(w) {}
    quint32 mDestNeuronIndex;
    quint32 mSrcNeuronIndex;
    NNType mWeight;
};

struct Level {
    Level() = default;
    Level(quint32 neurons);

    std::vector<NNNeuron> mNeurons;
    std::vector<NNConnection> mConnections;
};


class NeuralNetwork
{
public:
    NeuralNetwork(quint32 inputValCount, quint32 outputValCount);
    NeuralNetwork(const NeuralNetwork &n) = default;
    NeuralNetwork(NeuralNetwork &&n) : mOutputValues(std::move(n.mOutputValues)), mLevels(std::move(n.mLevels)) {}

    NeuralNetwork &operator=(NeuralNetwork &&n) { this->mOutputValues = std::move(n.mOutputValues); this->mLevels = std::move(n.mLevels); return *this;}
    NeuralNetwork &operator=(const NeuralNetwork &n) = default;
    ~NeuralNetwork();


    void makeDefaultConnection();

    const std::vector<NNType> &outputValues() const;

    void setInputValues(const std::vector<NNType> &inputValues);
    void setInputValue(quint32 index, NNType value);
    NNType inputValue(quint32 index) const;

    void run();

    NeuralNetwork clone(bool mutate);

private:
    void clearNeurons();

    std::vector<NNType> mOutputValues;
    std::vector<Level> mLevels;







};

#endif // NEURALNETWORK_H
