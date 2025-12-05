#include "GameEngine.h"
#include "ResourceManager.h"
#include <cmath>
#include <filesystem>
#include <iostream>
#include <sstream>

namespace {
const std::filesystem::path kAssetRoot("ProjectOOP");
const std::filesystem::path kFontCandidates[] = {
    std::filesystem::path("C:/Windows/Fonts/arial.ttf"),
    kAssetRoot / "assets" / "arial.ttf",
    std::filesystem::path("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")};

const char* obstacleTypeToString(ObstacleType type) {
    switch (type) {
    case ObstacleType::TRAIN:
        return "Train";
    case ObstacleType::BARRIER:
        return "Barrier";
    case ObstacleType::CONE:
        return "Cone";
    case ObstacleType::FENCE:
        return "Fence";
    default:
        return "Unknown";
    }
}
}

GameEngine::GameEngine()
    : mWindow(sf::VideoMode(800, 600), "Subway Surfers"),
    mFont(), mIsPaused(false),
    mIsGameOver(false), mShowMenu(true),
    mShowHighscorePanel(false), mMenuSelection(MenuItem::Play), mDayNightTimer(0.0f) {
    mWindow.setFramerateLimit(60);
    mWindow.setVerticalSyncEnabled(true);

    bool fontLoaded = false;
    for (const auto& candidate : kFontCandidates) {
        std::error_code ec;
        if (!std::filesystem::exists(candidate, ec)) {
            continue;
        }
        if (mFont.loadFromFile(candidate.string())) {
            fontLoaded = true;
            break;
        }
    }
    if (!fontLoaded) {
        std::cerr << "Failed to load any font candidate." << std::endl;
    }

    mScoreText.setFont(mFont);
    mScoreText.setCharacterSize(24);
    mScoreText.setFillColor(sf::Color::White);
    mScoreText.setPosition(sf::Vector2f(90.f, 40.f));

    mGameOverText.setFont(mFont);
    mGameOverText.setCharacterSize(48);
    mGameOverText.setFillColor(sf::Color::Red);
    mGameOverText.setString("GAME OVER\nPress R to Restart");
    mGameOverText.setPosition(sf::Vector2f(200.f, 250.f));

    mHudPanel.setSize(sf::Vector2f(220.f, 100.f));
    mHudPanel.setFillColor(sf::Color(0, 0, 0, 150));
    mHudPanel.setOutlineThickness(2.f);
    mHudPanel.setOutlineColor(sf::Color::Transparent);
    mHudPanel.setPosition(sf::Vector2f(20.f, 20.f));

    mCoinIcon.setPosition(sf::Vector2f(-100.f, -100.f));
    mPauseSprite.setPosition(sf::Vector2f(720.f, 20.f));

    mMenuPanel.setSize(sf::Vector2f(500.f, 300.f));
    mMenuPanel.setFillColor(sf::Color(0, 0, 0, 200));
    mMenuPanel.setOutlineThickness(3.f);
    mMenuPanel.setOutlineColor(sf::Color::White);
    mMenuPanel.setPosition(sf::Vector2f(
        (mWindow.getSize().x - mMenuPanel.getSize().x) / 2.f,
        (mWindow.getSize().y - mMenuPanel.getSize().y) / 2.f));

    const float menuCenterX = mMenuPanel.getPosition().x + mMenuPanel.getSize().x / 2.f;
    float optionY = mMenuPanel.getPosition().y + 60.f;
    const std::string labels[2] = { "Play", "High Score" };
    for (int i = 0; i < 2; ++i) {
        mMenuOptions[i].setFont(mFont);
        mMenuOptions[i].setCharacterSize(36);
        mMenuOptions[i].setString(labels[i]);
        auto bounds = mMenuOptions[i].getLocalBounds();
        mMenuOptions[i].setOrigin(bounds.left + bounds.width / 2.f,
            bounds.top + bounds.height / 2.f);
        mMenuOptions[i].setPosition(menuCenterX, optionY);
        optionY += 70.f;
    }

    mMenuHintText.setFont(mFont);
    mMenuHintText.setCharacterSize(20);
    mMenuHintText.setString("Use W/S or Arrow Keys to choose\nEnter to confirm");
    auto hintBounds = mMenuHintText.getLocalBounds();
    mMenuHintText.setOrigin(hintBounds.left + hintBounds.width / 2.f, 0.f);
    mMenuHintText.setPosition(menuCenterX, mMenuPanel.getPosition().y + mMenuPanel.getSize().y - 70.f);

    mHighscoreText.setFont(mFont);
    mHighscoreText.setCharacterSize(24);
    mHighscoreText.setFillColor(sf::Color::Yellow);
    mHighscoreText.setPosition(menuCenterX,
        mMenuPanel.getPosition().y + mMenuPanel.getSize().y - 120.f);

    updateMenuVisualState();

    loadResources();
    resetGame();
    mShowMenu = true;
    mMenuSelection = MenuItem::Play;
    mShowHighscorePanel = false;
    updateMenuVisualState();
}

void GameEngine::resetGame() {
    mPlayer = std::make_unique<Player>();
    mTrackManager = std::make_unique<TrackManager>();
    mScoreManager = std::make_unique<ScoreManager>();
    mIsGameOver = false;
    mIsPaused = false;
    mShowMenu = false;
    mShowHighscorePanel = false;
    mDayNightTimer = 0.0f;
    refreshHighscoreText();
}

void GameEngine::run() {
    sf::Clock clock;
    while (mWindow.isOpen()) {
        sf::Time deltaTime = clock.restart();
        processEvents();
        if (!mShowMenu && !mIsPaused && !mIsGameOver) {
            update(deltaTime);
        }
        render();
    }
}

void GameEngine::processEvents() {
    sf::Event event;
    while (mWindow.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            mWindow.close();
        }
        else if (event.type == sf::Event::KeyPressed) {
            if (mShowMenu) {
                if (event.key.code == sf::Keyboard::Up ||
                    event.key.code == sf::Keyboard::W) {
                    mMenuSelection = (mMenuSelection == MenuItem::Highscore)
                        ? MenuItem::Play
                        : MenuItem::Highscore;
                    updateMenuVisualState();
                }
                else if (event.key.code == sf::Keyboard::Down ||
                    event.key.code == sf::Keyboard::S) {
                    mMenuSelection = (mMenuSelection == MenuItem::Play)
                        ? MenuItem::Highscore
                        : MenuItem::Play;
                    updateMenuVisualState();
                }
                else if (event.key.code == sf::Keyboard::Enter ||
                    event.key.code == sf::Keyboard::Space) {
                    if (mMenuSelection == MenuItem::Play) {
                        mShowMenu = false;
                        mShowHighscorePanel = false;
                    }
                    else {
                        mShowHighscorePanel = !mShowHighscorePanel;
                        refreshHighscoreText();
                    }
                }
                continue;
            }
            if (mIsGameOver) {
                if (event.key.code == sf::Keyboard::R) {
                    resetGame();
                }
            }
            else {
                if (event.key.code == sf::Keyboard::A ||
                    event.key.code == sf::Keyboard::Left)
                    mPlayer->moveLeft();
                else if (event.key.code == sf::Keyboard::D ||
                    event.key.code == sf::Keyboard::Right)
                    mPlayer->moveRight();
                else if (event.key.code == sf::Keyboard::W ||
                    event.key.code == sf::Keyboard::Up)
                    mPlayer->jump();
                else if (event.key.code == sf::Keyboard::S ||
                    event.key.code == sf::Keyboard::Down)
                    mPlayer->slide();
                else if (event.key.code == sf::Keyboard::P) {
                    mIsPaused = !mIsPaused;
                    updatePauseSprite();
                }
            }
        }
    }
}

void GameEngine::update(sf::Time deltaTime) {
    mPlayer->update(deltaTime);
    mTrackManager->update(deltaTime);
    mScoreManager->addScore(10.0f *
        deltaTime.asSeconds()); // 10 points per second

    // Day/Night Cycle
    mDayNightTimer += deltaTime.asSeconds();
    float cycleDuration = 60.0f; // 60 seconds for a full cycle
    float brightness = 0.4f + 0.6f * (0.5f + 0.5f * std::cos(mDayNightTimer * (2.0f * 3.14159f / cycleDuration)));
    sf::Uint8 val = static_cast<sf::Uint8>(brightness * 255);
    mBackgroundSprite.setColor(sf::Color(val, val, val));

    if (mPlayer->isDoubleCoinActive()) {
        mScoreManager->setMultiplier(2);
    }
    else {
        mScoreManager->setMultiplier(1);
    }

    handleCollisions();

    // Magnet Logic
    if (mPlayer->isMagnetActive()) {
        auto& coins = mTrackManager->getCoins();
        sf::FloatRect playerBounds = mPlayer->getBounds();
        sf::Vector2f playerPos(playerBounds.left + playerBounds.width / 2.0f,
            playerBounds.top + playerBounds.height / 2.0f);
        for (auto* coin :
            coins) { // Changed to auto* because GameList returns pointers
            sf::Vector2f coinPos = coin->getPosition();
            float dx = playerPos.x - coinPos.x;
            float dy = playerPos.y - coinPos.y;
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist < 300.0f) { // Attraction range
                coin->setPosition(
                    sf::Vector2f(coinPos.x + dx * 5.0f * deltaTime.asSeconds(),
                        coinPos.y + dy * 5.0f * deltaTime.asSeconds()));
            }
        }
    }

    // Update HUD
    std::stringstream ss;
    ss << "Player: " << mScoreManager->getPlayerName()
        << "\nScore: " << (int)mScoreManager->getScore()
        << "\nCoins: " << mScoreManager->getCoins()
        << "\nHigh Score: " << (int)mScoreManager->getHighScore();
    mScoreText.setString(ss.str());
}

void GameEngine::handleCollisions() {
    sf::FloatRect playerBounds = mPlayer->getBounds();

    // Check Obstacles
    auto& obstacles = mTrackManager->getObstacles();
    for (auto* obs : obstacles) {
        if (obs->getBounds().intersects(playerBounds)) {
            ObstacleType type = obs->getType();
            bool fatal = obs->onCollision(*mPlayer);
            if (fatal) {
                std::string gameOverMsg = "GAME OVER\nHit: ";
                gameOverMsg += obstacleTypeToString(type);
                gameOverMsg += "\nPress R to Restart";
                mGameOverText.setString(gameOverMsg);
                mIsGameOver = true;
                mScoreManager->saveHighScore();
                refreshHighscoreText();
                return;
            }
        }
    }

    // Check Coins
    auto& coins = mTrackManager->getCoins();
    for (auto* coin : coins) {
        if (coin->getBounds().intersects(playerBounds)) {
            mScoreManager->addCoins(1);
            coin->setPosition(
                sf::Vector2f(-1000, -1000)); // Move off screen to be removed
        }
    }

    // Check PowerUps
    auto& powerUps = mTrackManager->getPowerUps();
    for (auto* pu : powerUps) {
        if (pu->getBounds().intersects(playerBounds)) {
            pu->applyEffect(*mPlayer);
            pu->setPosition(sf::Vector2f(-1000, -1000));
        }
    }
}

void GameEngine::render() {
    mWindow.clear(sf::Color::Black);
    mWindow.draw(mBackgroundSprite);

    // Get ambient brightness from background color
    sf::Color ambient = mBackgroundSprite.getColor();
    float brightness = ambient.r / 255.0f;

    // Draw Ground/Lanes
    sf::RectangleShape ground(sf::Vector2f(600.0f, 600.0f));
    ground.setPosition(sf::Vector2f(100.0f, 0.0f));
    sf::Uint8 groundVal = static_cast<sf::Uint8>(100.0f * brightness);
    ground.setFillColor(sf::Color(groundVal, groundVal, groundVal)); // Gray adjusted by brightness
    mWindow.draw(ground);

    // Draw Lane Lines
    sf::RectangleShape line(sf::Vector2f(5.0f, 600.0f));
    sf::Uint8 lineVal = static_cast<sf::Uint8>(255.0f * brightness);
    line.setFillColor(sf::Color(lineVal, lineVal, lineVal));
    line.setPosition(sf::Vector2f(300.0f, 0.0f));
    mWindow.draw(line);
    line.setPosition(sf::Vector2f(500.0f, 0.0f));
    mWindow.draw(line);

    if (!mShowMenu) {
        mTrackManager->draw(mWindow);
        mPlayer->draw(mWindow);

        mWindow.draw(mHudPanel);
        if (mHudBannerSprite.getTexture()) {
            mWindow.draw(mHudBannerSprite);
        }

        if (mCoinIcon.getTexture()) {
            mWindow.draw(mCoinIcon);
        }

        mWindow.draw(mScoreText);
        if (mPauseSprite.getTexture()) {
            mWindow.draw(mPauseSprite);
        }
    }

    if (mIsGameOver) {
        if (mGameOverSprite.getTexture()) {
            mWindow.draw(mGameOverSprite);
        }
        mWindow.draw(mGameOverText);
    }

    if (mShowMenu) {
        if (mMenuSprite.getTexture()) {
            mWindow.draw(mMenuSprite);
        }

        mWindow.draw(mMenuPanel);
        for (const auto& option : mMenuOptions) {
            mWindow.draw(option);
        }
        mWindow.draw(mMenuHintText);
        if (mShowHighscorePanel) {
            mWindow.draw(mHighscoreText);
        }
    }

    mWindow.display();
}

void GameEngine::loadResources() {
    auto& rm = ResourceManager::get();
    const auto textureDir = (kAssetRoot / "PNG").string();
    rm.loadTexturesFromDirectory(textureDir);

    const auto soundPath = (kAssetRoot / "sounds" / "Voicy_Subway-Surfers.wav").string();
    rm.loadSound("bg_music", soundPath);
    if (rm.hasTexture("Menu")) {
        mMenuSprite.setTexture(rm.getTexture("Menu"));
        const auto size = mMenuSprite.getTexture()->getSize();
        auto windowSize = mWindow.getSize();
        if (size.x > 0 && size.y > 0) {
            mMenuSprite.setScale(
                static_cast<float>(windowSize.x) / static_cast<float>(size.x),
                static_cast<float>(windowSize.y) / static_cast<float>(size.y));
        }
    }

    if (rm.hasTexture("background")) {
        mBackgroundSprite.setTexture(rm.getTexture("background"));
        const sf::Texture& bgTexture = rm.getTexture("background");
        auto windowSize = mWindow.getSize();
        auto texSize = bgTexture.getSize();
        if (texSize.x > 0 && texSize.y > 0) {
            mBackgroundSprite.setScale(
                static_cast<float>(windowSize.x) / static_cast<float>(texSize.x),
                static_cast<float>(windowSize.y) /
                static_cast<float>(texSize.y));
        }
    }

    mHudBannerSprite.setTexture(sf::Texture());
    mCoinIcon.setTexture(sf::Texture());

    if (rm.hasTexture("gameover")) {
        mGameOverSprite.setTexture(rm.getTexture("gameover"));
        const auto texSize = mGameOverSprite.getTexture()->getSize();
        if (texSize.x > 0 && texSize.y > 0) {
            float maxWidth = 500.f;
            float targetScale =
                maxWidth / static_cast<float>(texSize.x);
            mGameOverSprite.setScale(targetScale, targetScale);
            mGameOverSprite.setOrigin(texSize.x / 2.f, texSize.y / 2.f);
            mGameOverSprite.setPosition(
                static_cast<float>(mWindow.getSize().x) / 2.f,
                static_cast<float>(mWindow.getSize().y) / 2.f - 60.f);
        }
    }

    updatePauseSprite();

    mBgMusic.setBuffer(rm.getSoundBuffer("bg_music"));
    mBgMusic.setLoop(true);
    mBgMusic.setVolume(35.0f);
    mBgMusic.play();
}

void GameEngine::updatePauseSprite() {
    auto& rm = ResourceManager::get();
    const std::string textureKey = mIsPaused ? "play" : "pause";
    if (rm.hasTexture(textureKey)) {
        mPauseSprite.setTexture(rm.getTexture(textureKey));
        mPauseSprite.setScale(0.6f, 0.6f);
    }
}

void GameEngine::updateMenuVisualState() {
    for (int i = 0; i < 2; ++i) {
        bool selected = (static_cast<int>(mMenuSelection) == i);
        mMenuOptions[i].setFillColor(selected ? sf::Color::Yellow : sf::Color::White);
    }
}

void GameEngine::refreshHighscoreText() {
    if (!mScoreManager) {
        return;
    }
    std::stringstream ss;
    ss << "High Score: " << static_cast<int>(mScoreManager->getHighScore())
        << "\nBy: " << mScoreManager->getPlayerName();
    mHighscoreText.setString(ss.str());
    auto bounds = mHighscoreText.getLocalBounds();
    mHighscoreText.setOrigin(bounds.left + bounds.width / 2.f, 0.f);
}
