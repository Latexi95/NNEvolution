#include "neuralnetwork.h"
#include <random>
#include <unordered_map>
#include <cassert>
#include <algorithm>

std::mt19937 randgen;


NeuralNetwork::NeuralNetwork(quint32 inputValCount, quint32 outputValCount) :
    mOutputValueCount(outputValCount),
    mOutputBufferIndex(0)
{
    mLevels.emplace_back(inputValCount);
    mLevels.emplace_back(10);

    for (auto &buf : mNeuronBuffers) {
        buf.resize(std::max(inputValCount, outputValCount));
    }
}

NeuralNetwork::NeuralNetwork() :
    mOutputValueCount(0),
    mOutputBufferIndex(0)
{

}

NeuralNetwork::~NeuralNetwork()
{

}

void NeuralNetwork::makeDefaultConnection()
{
    mLevels[0].mConnections.emplace_back(13, 0, 10);
    mLevels[1].mConnections.emplace_back(0, 8, 1);
}

const std::vector<NNType> &NeuralNetwork::outputValues() const
{
    return mNeuronBuffers[mOutputBufferIndex];
}


void NeuralNetwork::setInputValues(const std::vector<NNType> &inputValues)
{
    assert(mLevels.front().mNeuronCount == inputValues.size());
    int i = 0;
    for (auto input : inputValues) {
        mNeuronBuffers[!mOutputBufferIndex][i] = input;
        ++i;
    }
}

void NeuralNetwork::setInputValue(quint32 index, NNType value)
{
    mNeuronBuffers[!mOutputBufferIndex][index] = value;
}

NNType NeuralNetwork::inputValue(quint32 index) const
{
    return mNeuronBuffers[!mOutputBufferIndex][index];
}

void NeuralNetwork::run()
{
    auto lastLevelIt = mLevels.end();
    for (auto levelIt = mLevels.begin(); levelIt != lastLevelIt; ++levelIt) {

        Level &cur = *levelIt;

        std::fill(mNeuronBuffers[mOutputBufferIndex].begin(), mNeuronBuffers[mOutputBufferIndex].end(), 0);

        for (const NNConnection &con : cur.mConnections) {
            NNType src = mNeuronBuffers[!mOutputBufferIndex][con.mSrcNeuronIndex];
            auto &dest = mNeuronBuffers[mOutputBufferIndex][con.mDestNeuronIndex];
            dest += src * con.mWeight;
        }
        mOutputBufferIndex = !mOutputBufferIndex;
    }
    mOutputBufferIndex = !mOutputBufferIndex;
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
            con.mWeight += 1.5 - rdist(randgen) * 3;
        }
        else if (mutType < 0.80) {
            int levelIndex = std::uniform_int_distribution<>(0, c.mLevels.size() - 1)(randgen);
            Level &level = c.mLevels[levelIndex];

            int sourceCount = level.mNeuronCount;
            int targetCount = (levelIndex == c.mLevels.size() - 1) ? c.mOutputValueCount : c.mLevels[levelIndex + 1].mNeuronCount;

            if (targetCount == 0 || sourceCount == 0) continue;

            level.mConnections.emplace_back(std::uniform_int_distribution<>(0, sourceCount - 1)(randgen),
                                       std::uniform_int_distribution<>(0, targetCount - 1)(randgen),
                                       1.5 - 2.0 * rdist(randgen));
        }
        else if (mutType < 0.90) {
            if (c.mLevels.size() == 1) continue;
            int targetLevelIndex = std::uniform_int_distribution<>(1, c.mLevels.size() - 1)(randgen);
            int srcLevelIndex = targetLevelIndex - 1;
            Level &srcLevel = c.mLevels[srcLevelIndex];
            Level &targetLevel = c.mLevels[targetLevelIndex];
            int sourceCount = srcLevel.mNeuronCount;
            if (sourceCount == 0) continue;
            int targetIndex = targetLevel.mNeuronCount;
            targetLevel.mNeuronCount++;

            srcLevel.mConnections.emplace_back(std::uniform_int_distribution<>(0, sourceCount - 1)(randgen),
                               targetIndex,
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

            newLevel.mNeuronCount = index;
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

    quint32 oc = mOutputValueCount;

    for (const Level &l : mLevels) {
        oc = std::max(oc, l.mNeuronCount);
    }

    for (auto &buffer : mNeuronBuffers) {
        buffer.resize(oc);
    }

    return c;
}

const std::vector<Level> &NeuralNetwork::levels() const
{
    return mLevels;
}

void NeuralNetwork::optimize()
{
    for (Level &level : mLevels) {
        std::map<std::pair<int, int>, std::pair<int, NNType>> usageMap;

        bool multipleUsages = false;
        for (NNConnection &con : level.mConnections) {
            usageMap[std::pair<int, int>(con.mSrcNeuronIndex, con.mDestNeuronIndex)].second += con.mWeight;
            if (++usageMap[std::pair<int, int>(con.mSrcNeuronIndex, con.mDestNeuronIndex)].first > 1) {
                multipleUsages = true;
            }
        }
        if (multipleUsages) {
            level.mConnections.clear();
            level.mConnections.resize(usageMap.size());
            int index = 0;
            for (const std::pair<std::pair<int, int>, std::pair<int, NNType>> &p: usageMap) {
                const std::pair<int, int> &conPair = p.first;
                NNConnection &con = level.mConnections[index];
                con.mSrcNeuronIndex = conPair.first;
                con.mDestNeuronIndex = conPair.second;
                con.mWeight = p.second.second;
            }
        }
    }
}



Level::Level(quint32 neurons) :
    mNeuronCount(neurons)
{

}
