#include "neuralnetwork.h"
#include <random>
#include <unordered_map>
#include <cassert>

std::mt19937 randgen(std::random_device{}());


NeuralNetwork::NeuralNetwork(quint32 inputValCount, quint32 outputValCount) :
    mOutputValues(outputValCount)
{
    mLevels.emplace_back(inputValCount);
}

NeuralNetwork::~NeuralNetwork()
{

}

void NeuralNetwork::makeDefaultConnection()
{
    mLevels[0].mConnections.emplace_back(13, 8, 10);
}

const std::vector<NNType> &NeuralNetwork::outputValues() const
{
    return mOutputValues;
}


void NeuralNetwork::setInputValues(const std::vector<NNType> &inputValues)
{
    assert(mLevels.front().mNeurons.size() == inputValues.size());
    auto inputIt = inputValues.begin();
    for (auto & n : mLevels.front().mNeurons) {
        n.mValue = *inputIt;
        ++inputIt;
    }
}

void NeuralNetwork::setInputValue(quint32 index, NNType value)
{
    mLevels.front().mNeurons[index].mValue = value;
}

NNType NeuralNetwork::inputValue(quint32 index) const
{
    return mLevels.front().mNeurons[index].mValue;
}

void NeuralNetwork::run()
{
    clearNeurons();

    auto lastLevelIt = --mLevels.end();
    for (auto levelIt = mLevels.begin(); levelIt != lastLevelIt;) {
        auto nextLevelIt = levelIt;
        ++nextLevelIt;

        Level &cur = *levelIt;
        Level &next = *nextLevelIt;

        for (const NNConnection &con : cur.mConnections) {
            const NNNeuron &src = cur.mNeurons[con.mSrcNeuronIndex];
            NNNeuron &dest = next.mNeurons[con.mDestNeuronIndex];
            dest.mValue += src.mValue * con.mWeight;
        }

        levelIt = nextLevelIt;
    }

    Level &lastLevel = *lastLevelIt;
    for (const NNConnection &con : lastLevel.mConnections) {
        const NNNeuron &src = lastLevel.mNeurons[con.mSrcNeuronIndex];
        mOutputValues[con.mDestNeuronIndex] += src.mValue * con.mWeight;
    }
}

NeuralNetwork NeuralNetwork::clone(bool mutate)
{
    NeuralNetwork c(*this);
    if (!mutate) return c;

    std::uniform_real_distribution<NNType> rdist(0, 1);

    while (true) {
        NNType mutType = rdist(randgen);
        if (mutType < 0.2) return c;

        if (mutType < 0.5) {
            Level &level = c.mLevels[std::uniform_int_distribution<>(0, c.mLevels.size() - 1)(randgen)];
            if (level.mConnections.empty()) continue;
            NNConnection &con = level.mConnections[std::uniform_int_distribution<>(0, level.mConnections.size() - 1)(randgen)];
            con.mWeight += 0.5 - rdist(randgen);
        }
        else if (mutType < 0.85) {
            int levelIndex = std::uniform_int_distribution<>(0, c.mLevels.size() - 1)(randgen);
            Level &level = c.mLevels[levelIndex];

            int sourceCount = level.mNeurons.size();
            int targetCount = (levelIndex == c.mLevels.size() - 1) ? c.mOutputValues.size() : c.mLevels[levelIndex + 1].mNeurons.size();

            if (targetCount == 0 || sourceCount == 0) continue;

            level.mConnections.emplace_back(std::uniform_int_distribution<>(0, sourceCount - 1)(randgen),
                                       std::uniform_int_distribution<>(0, targetCount - 1)(randgen),
                                       1.5 - 2.0 * rdist(randgen));
        }
        else if (mutType < 0.95) {
            int levelIndex = std::uniform_int_distribution<>(0, c.mLevels.size() - 1)(randgen);
            Level &level = c.mLevels[levelIndex];
            if (level.mConnections.empty()) continue;
            int removeIndex = std::uniform_int_distribution<>(0, level.mConnections.size() - 1)(randgen);
            level.mConnections.erase(level.mConnections.begin() + removeIndex);
        }
        else {
            c.mLevels.emplace_back();
            Level &newLevel = c.mLevels.back();
            Level &oldLevel = *(c.mLevels.end() - 2);
            std::unordered_map<int, int> endPoints;
            for (const NNConnection &con : oldLevel.mConnections) {
                endPoints[con.mDestNeuronIndex] = 0;
            }
            int index = 0;
            for (auto endPointIt = endPoints.begin();endPointIt != endPoints.end();++endPointIt) {
                endPointIt->second = index++;
            }

            newLevel.mNeurons.resize(index);
            newLevel.mConnections.resize(index);

            for (auto endPointIt = endPoints.begin();endPointIt != endPoints.end();++endPointIt) {
               NNConnection &con = newLevel.mConnections[endPointIt->second];
               con.mSrcNeuronIndex = endPointIt->second;
               con.mDestNeuronIndex = endPointIt->first;
            }

            for (NNConnection &con : oldLevel.mConnections) {
                int midNeuronIndex = endPoints[con.mDestNeuronIndex];
                con.mDestNeuronIndex = midNeuronIndex;
            }
        }
    }

    return c;
}

void NeuralNetwork::clearNeurons()
{
    for (auto levelIt = ++mLevels.begin(); levelIt != mLevels.end(); ++levelIt) {
        for (auto neuron : levelIt->mNeurons) {
            neuron.mValue = 0;
        }
    }
    for (auto &outputVal : mOutputValues) {
        outputVal = 0;
    }
}



Level::Level(quint32 neurons) :
    mNeurons(neurons)
{

}
