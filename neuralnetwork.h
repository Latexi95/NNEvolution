#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H
#include <QtGlobal>
#include <vector>
#include <array>
#include "defs.h"
#include <QDataStream>

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

    quint32 mNeuronCount;
    std::vector<NNConnection> mConnections;
};


class NeuralNetwork
{
public:
    friend QDataStream &operator >> (QDataStream &s, NeuralNetwork &nn);
    friend QDataStream &operator << (QDataStream &s, const NeuralNetwork &nn);
    NeuralNetwork();
    NeuralNetwork(quint32 inputValCount, quint32 outputValCount);
    NeuralNetwork(const NeuralNetwork &n) = default;
    NeuralNetwork(NeuralNetwork &&n) : mLevels(std::move(n.mLevels)), mNeuronBuffers(std::move(n.mNeuronBuffers)), mOutputValueCount(n.mOutputValueCount), mOutputBufferIndex(n.mOutputBufferIndex) {}

    NeuralNetwork &operator=(NeuralNetwork &&n) { this->mNeuronBuffers = std::move(n.mNeuronBuffers); this->mLevels = std::move(n.mLevels); mOutputValueCount = n.mOutputValueCount; mOutputBufferIndex = n.mOutputBufferIndex; return *this;}
    NeuralNetwork &operator=(const NeuralNetwork &n) = default;
    ~NeuralNetwork();


    void makeDefaultConnection();

    const std::vector<NNType> &outputValues() const;
    quint32 outputValueSize() const { return mOutputValueCount; }

    void setInputValues(const std::vector<NNType> &inputValues);
    void setInputValue(quint32 index, NNType value);
    NNType inputValue(quint32 index) const;

    void run();

    NeuralNetwork clone(bool mutate);

    const std::vector<Level> &levels() const;

    void optimize();

private:
    std::vector<Level> mLevels;

    std::array<std::vector<NNType>, 2> mNeuronBuffers;
    quint32 mOutputValueCount;
    bool mOutputBufferIndex;
};

inline QDataStream &operator << (QDataStream &s, const NeuralNetwork &nn) {
    s << (int)nn.mOutputValueCount;
    s << (int)nn.levels().size();
    for (const Level &lvl : nn.levels()) {
        s << (int)lvl.mNeuronCount;
        s << (int)lvl.mConnections.size();
        for (const NNConnection &con : lvl.mConnections) {
            s << con.mSrcNeuronIndex;
            s << con.mDestNeuronIndex;
            s << con.mWeight;
        }
    }
    return s;
}

inline QDataStream &operator >> (QDataStream &s, NeuralNetwork &nn) {
    int outputSize, levelsSize;
    s >> outputSize;
    s >> levelsSize;
    nn.mOutputValueCount = outputSize;
    nn.mLevels.resize(levelsSize);

    int nsize = outputSize;
    for (Level &lvl : nn.mLevels) {
        int neurons, conSize;
        s >> neurons;
        s >> conSize;
        nsize = std::max(nsize, neurons);
        lvl.mConnections.resize(conSize);
        for (NNConnection &con : lvl.mConnections) {
            s >> con.mSrcNeuronIndex;
            s >> con.mDestNeuronIndex;
            s >> con.mWeight;
        }
    }
    for (auto &buf : nn.mNeuronBuffers) {
        buf.resize(nsize);
    }
    return s;
}

#endif // NEURALNETWORK_H
