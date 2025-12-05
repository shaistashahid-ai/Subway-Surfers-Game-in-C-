#include "TrackManager.h"
#include "LaneSystem.h"
#include <algorithm>
#include <array>
#include <vector>

TrackManager::TrackManager()
    : mGameSpeed(300.0f), mSpawnTimer(0.0f), mCoinTimer(0.0f),
    mPowerUpTimer(0.0f), mDifficultyTimer(0.0f),
    mRng(static_cast<unsigned int>(std::random_device{}())) {
}

void TrackManager::update(sf::Time dt) {
    float dtSeconds = dt.asSeconds();

    mObstacles.updateAll(dt, mGameSpeed);
    mPowerUps.updateAll(dt, mGameSpeed);
    mCoins.updateAll(dt, mGameSpeed);

    // Spawning logic
    mSpawnTimer += dtSeconds;
    if (mSpawnTimer >= obstacleInterval()) {
        spawnObstacle();
        mSpawnTimer = 0.0f;
    }

    mCoinTimer += dtSeconds;
    if (mCoinTimer >= coinInterval()) {
        spawnCoin();
        mCoinTimer = 0.0f;
    }

    mPowerUpTimer += dtSeconds;
    if (mPowerUpTimer >= powerUpInterval()) {
        spawnPowerUp();
        mPowerUpTimer = 0.0f;
    }

    // Speed scaling
    mDifficultyTimer += dtSeconds;
    if (mDifficultyTimer > 5.0f) {
        mGameSpeed = std::min(1100.0f, mGameSpeed + 25.0f);
        mDifficultyTimer = 0.0f;
    }
}

void TrackManager::draw(sf::RenderWindow& window) {
    mCoins.drawAll(window);
    mObstacles.drawAll(window);
    mPowerUps.drawAll(window);
}

void TrackManager::spawnObstacle() {
    std::uniform_int_distribution<int> patternDist(0, 2);
    int pattern = patternDist(mRng);

    if (pattern == 0) {
        int lane = randomLane();
        float x = LaneSystem::getLaneCenter(lane);
        float y = -200.0f;
        ObstacleType type = randomObstacleType();
        switch (type) {
        case ObstacleType::TRAIN:
            mObstacles.add(std::make_unique<Train>(x, y));
            break;
        case ObstacleType::BARRIER:
            mObstacles.add(std::make_unique<Barrier>(x, y));
            break;
        case ObstacleType::CONE:
            mObstacles.add(std::make_unique<Cone>(x, y));
            break;
        case ObstacleType::FENCE:
            mObstacles.add(std::make_unique<Fence>(x, y));
            break;
        }
    }
    else if (pattern == 1) {
        // spawn two barriers leaving one safe lane
        int blockedLane = randomLane();
        int secondLane = (blockedLane + 2) % 3; // opposite lane to guarantee gap
        float y = -150.0f;
        mObstacles.add(std::make_unique<Barrier>(LaneSystem::getLaneCenter(blockedLane), y));
        mObstacles.add(std::make_unique<Barrier>(LaneSystem::getLaneCenter(secondLane), y - 80.0f));
    }
    else {
        // train plus coin trail on other lane
        int trainLane = randomLane();
        mObstacles.add(std::make_unique<Train>(LaneSystem::getLaneCenter(trainLane), -220.0f));
        int safeLane = randomLane(trainLane);
        spawnCoinRow(safeLane, 4, 110.0f);
    }
}

void TrackManager::spawnCoin() {
    int lane = randomLane();
    spawnCoinRow(lane, 3, 80.0f);
}

void TrackManager::spawnPowerUp() {
    int lane = randomLane();
    float x = LaneSystem::getLaneCenter(lane);
    float y = -50.0f;

    std::uniform_int_distribution<int> typeDist(0, 3);
    int type = typeDist(mRng);
    std::unique_ptr<PowerUp> pu;

    switch (type) {
    case 0:
        pu = std::make_unique<MagnetPower>(x, y);
        break;
    case 1:
        pu = std::make_unique<JetpackPower>(x, y);
        break;
    case 2:
        pu = std::make_unique<ShieldPower>(x, y);
        break;
    case 3:
        pu = std::make_unique<DoubleCoinPower>(x, y);
        break;
    }

    mPowerUps.add(std::move(pu));
}

float TrackManager::obstacleInterval() const {
    float interval = 1.6f - (mGameSpeed - 300.0f) / 1400.0f;
    return std::max(0.55f, interval);
}

float TrackManager::coinInterval() const {
    float interval = 0.7f - (mGameSpeed - 300.0f) / 2000.0f;
    return std::max(0.25f, interval);
}

float TrackManager::powerUpInterval() const {
    float interval = 12.0f - (mGameSpeed - 300.0f) / 500.0f;
    return std::max(5.0f, interval);
}

void TrackManager::spawnCoinRow(int lane, int count, float spacing) {
    float baseX = LaneSystem::getLaneCenter(lane);
    float y = -60.0f;
    for (int i = 0; i < count; ++i) {
        auto coin = std::make_unique<Coin>(baseX, y - (i * spacing));
        mCoins.add(std::move(coin));
    }
}

int TrackManager::randomLane(int excludeLane) {
    std::vector<int> lanes = { 0, 1, 2 };
    if (excludeLane >= 0 && excludeLane < 3) {
        lanes.erase(std::remove(lanes.begin(), lanes.end(), excludeLane), lanes.end());
    }
    std::uniform_int_distribution<std::size_t> dist(0, lanes.size() - 1);
    return lanes[dist(mRng)];
}

ObstacleType TrackManager::randomObstacleType() {
    static const std::array<ObstacleType, 4> types = { ObstacleType::TRAIN, ObstacleType::BARRIER,
        ObstacleType::CONE, ObstacleType::FENCE };
    std::uniform_int_distribution<int> dist(0, static_cast<int>(types.size()) - 1);
    return types[dist(mRng)];
}
