#include "GameEngine.h"
#include "ResourceManager.h"
#include <cmath>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <limits> // Required for input clearing

// M_PI definition required for std::cos in update()
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// =========================================================================
// 💡 Game Constants and Local Helpers
// =========================================================================

namespace {
    // Game Tuning Constants 
    const float MAGNET_DISTANCE = 300.0f;
    const float MAGNET_PULL_SPEED = 5.0f;
    const float DAY_NIGHT_CYCLE_DURATION = 60.0f; // 60 seconds for a full cycle

    // Asset Paths 
    const std::filesystem::path kAssetRoot("ProjectOOP");
    const std::filesystem::path kFontCandidates[] = {
        kAssetRoot / "assets" / "arial.ttf", // 1. Try local asset first
        std::filesystem::path("C:/Windows/Fonts/arial.ttf"), // 2. Fallback to Windows
        std::filesystem::path("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf") // 3. Fallback to Linux
    };
}

// =========================================================================
// 🚀 GameEngine Constructor
// =========================================================================

GameEngine::GameEngine()
    : mWindow(sf::VideoMode(800, 600), "Subway Surfers"),
    mFont(), mIsPaused(false),
    mIsGameOver(false), mShowMenu(true),
    mShowHighscorePanel(false), mMenuSelection(MenuItem::Play), 
    mAreGameAssetsLoaded(false), mIsDebugMode(false), mShowRegistration(false),
    mDayNightTimer(0.0f) 
{
    mWindow.setFramerateLimit(60);
    mWindow.setVerticalSyncEnabled(true);

    // --- Font Loading ---
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
        std::cerr << "CRITICAL ERROR: Failed to load any font candidate for menu/UI." << std::endl;
    }

    // --- Text/Panel Setup ---
    mScoreText.setFont(mFont);
    mScoreText.setCharacterSize(24);
    mScoreText.setFillColor(sf::Color::White);
    mScoreText.setPosition(sf::Vector2f(90.f, 40.f));

    // GameOver text remains blank
    mGameOverText.setFont(mFont);
    mGameOverText.setCharacterSize(48);
    mGameOverText.setFillColor(sf::Color::Red);
    mGameOverText.setString("");
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
    mMenuHintText.setString("Use W/S or Arrow Keys to choose\nEnter to confirm\n(Press TAB for Debug Info | ESC to Exit)");
    auto hintBounds = mMenuHintText.getLocalBounds();
    mMenuHintText.setOrigin(hintBounds.left + hintBounds.width / 2.f, 0.f);
    mMenuHintText.setPosition(menuCenterX, mMenuPanel.getPosition().y + mMenuPanel.getSize().y - 70.f);

    mHighscoreText.setFont(mFont);
    mHighscoreText.setCharacterSize(24);
    mHighscoreText.setFillColor(sf::Color::Yellow);
    mHighscoreText.setPosition(menuCenterX,
        mMenuPanel.getPosition().y + mMenuPanel.getSize().y - 120.f);
        
    // --- Registration Panel Setup ---
    mRegPanel.setSize(sf::Vector2f(450.f, 250.f));
    mRegPanel.setFillColor(sf::Color(50, 50, 50, 240));
    mRegPanel.setOutlineThickness(3.f);
    mRegPanel.setOutlineColor(sf::Color::Cyan);
    mRegPanel.setPosition(sf::Vector2f(
        (mWindow.getSize().x - mRegPanel.getSize().x) / 2.f,
        (mWindow.getSize().y - mRegPanel.getSize().y) / 2.f));

    mRegPromptText.setFont(mFont);
    mRegPromptText.setCharacterSize(30);
    mRegPromptText.setFillColor(sf::Color::White);
    mRegPromptText.setString("Enter your name to play (Max 12 chars):\nPress ENTER to start");
    auto promptBounds = mRegPromptText.getLocalBounds();
    mRegPromptText.setOrigin(promptBounds.width / 2.f, 0.f);
    mRegPromptText.setPosition(mRegPanel.getPosition().x + mRegPanel.getSize().x / 2.f, mRegPanel.getPosition().y + 30.f);

    mRegInputBox.setSize(sf::Vector2f(300.f, 50.f));
    mRegInputBox.setFillColor(sf::Color(20, 20, 20));
    mRegInputBox.setOutlineThickness(2.f);
    mRegInputBox.setOutlineColor(sf::Color::Yellow);
    mRegInputBox.setPosition(mRegPanel.getPosition().x + 75.f, mRegPanel.getPosition().y + 130.f);

    mRegInputText.setFont(mFont);
    mRegInputText.setCharacterSize(30);
    mRegInputText.setFillColor(sf::Color::Yellow);
    mRegInputText.setPosition(mRegInputBox.getPosition().x + 10.f, mRegInputBox.getPosition().y + 5.f);
    mPlayerNameInput = "";
    // --------------------------------

    updateMenuVisualState();

    // --- Menu Resource Load & Immediate Draw ---
    loadMenuResources();
    
    mWindow.clear(sf::Color::Black);
    if (mMenuSprite.getTexture()) {
        mWindow.draw(mMenuSprite);
    }
    mWindow.draw(mMenuPanel);
    for (const auto& option : mMenuOptions) {
        mWindow.draw(option);
    }
    mWindow.draw(mMenuHintText);
    mWindow.display(); 
    // -------------------------------------------

    resetGame(); 
    mShowMenu = true;
    mMenuSelection = MenuItem::Play;
    mShowHighscorePanel = false;
    updateMenuVisualState();
}

// =========================================================================
// 🔄 RESOURCE LOADING FUNCTIONS
// =========================================================================

void GameEngine::loadMenuResources() {
    auto& rm = ResourceManager::get();
    const auto textureDir = (kAssetRoot / "PNG").string();
    
    rm.loadTexturesFromDirectory(textureDir); 

    if (rm.hasTexture("Menu")) {
        mMenuSprite.setTexture(rm.getTexture("Menu"));
        const auto size = mMenuSprite.getTexture()->getSize();
        auto windowSize = mWindow.getSize();
        if (size.x > 0 && size.y > 0) {
            mMenuSprite.setScale(
                static_cast<float>(windowSize.x) / static_cast<float>(size.x),
                static_cast<float>(windowSize.y) / static_cast<float>(size.y));
        }
    } else {
         std::cerr << "Warning: 'Menu' texture not found." << std::endl;
    }
}

void GameEngine::loadAllGameResources() {
    auto& rm = ResourceManager::get();
    
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
    } else {
        std::cerr << "Warning: 'background' texture not found." << std::endl;
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
    } else {
        std::cerr << "Warning: 'gameover' texture not found." << std::endl;
    }

    updatePauseSprite(); 

    const auto soundPath = (kAssetRoot / "sounds" / "Voicy_Subway-Surfers.wav").string();
    if (rm.loadSound("bg_music", soundPath)) {
        mBgMusic.setBuffer(rm.getSoundBuffer("bg_music"));
        mBgMusic.setLoop(true);
        mBgMusic.setVolume(35.0f);
        mBgMusic.play();
    } else {
        std::cerr << "Warning: Background music sound file not loaded." << std::endl;
    }
    
    mAreGameAssetsLoaded = true;
}


// =========================================================================
// 🎮 GAME LOOP & EVENTS
// =========================================================================

void GameEngine::resetGame(const std::string& playerName) {
    mPlayer = std::make_unique<Player>();
    mTrackManager = std::make_unique<TrackManager>();
    mScoreManager = std::make_unique<ScoreManager>(); 
    
    // --- FILE HANDLING: LOAD ---
    // Load the stored high score from file (Name and Score)
    mScoreManager->loadHighScore(); 
    // ---------------------------
    
    // Set the name of the CURRENT player
    mScoreManager->setPlayerName(playerName); 
    
    mIsGameOver = false;
    mIsPaused = false;
    mShowMenu = false;
    mShowHighscorePanel = false;
    mShowRegistration = false;
    mDayNightTimer = 0.0f;
    refreshHighscoreText();
}

void GameEngine::run() {
    sf::Clock clock;
    while (mWindow.isOpen()) {
        sf::Time deltaTime = clock.restart();
        processEvents();
        // Check mShowRegistration state
        if (!mShowMenu && !mIsPaused && !mIsGameOver && !mShowRegistration) {
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
            if (event.key.code == sf::Keyboard::Escape) {
                if (mShowRegistration) {
                    mShowRegistration = false;
                    mShowMenu = true;
                } else if (!mShowMenu) {
                    mShowMenu = true;
                    // If game was running (not over), we pause it so we can resume.
                    // If game was over, we reset to allow new game.
                    if (!mIsGameOver) {
                        mIsPaused = true;
                    } else {
                        mIsPaused = false;
                        mIsGameOver = false;
                    }
                } else {
                    mWindow.close();
                }
                updateMenuVisualState(); // Update text (Play vs Resume)
            }

            // Debug Mode Toggle
            if (event.key.code == sf::Keyboard::Tab) {
                mIsDebugMode = !mIsDebugMode;
            }

            // --- Registration Screen Input ---
            if (mShowRegistration) {
                if (event.key.code == sf::Keyboard::Enter) {
                    std::string finalName = mPlayerNameInput.toAnsiString();
                    // Ensure name is not empty
                    if (finalName.empty()) {
                        finalName = "Player";
                    }
                    // Load assets if needed, then start game with the name
                    if (!mAreGameAssetsLoaded) {
                        loadAllGameResources();
                    }
                    resetGame(finalName); 
                    mPlayerNameInput = ""; // Clear input for next time
                }
                // Stop processing other keys
                continue; 
            }
            // --- End Registration Screen Input ---

            if (mShowMenu) {
                // Menu Navigation
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
                        if (mIsPaused) {
                            // Resume Game
                            mShowMenu = false;
                            mIsPaused = false;
                            updatePauseSprite();
                        } else {
                            // Go to registration screen instead of starting directly
                            mShowMenu = false;
                            mShowRegistration = true;
                            mPlayerNameInput = "";
                            mRegInputText.setString("");
                            mShowHighscorePanel = false;
                        }
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
                    // Go back to the menu to register a new name
                    mShowMenu = true;
                    mIsGameOver = false;
                }
            }
            else {
                // In-game controls
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
        
        // --- Text Entered Event Handling (must be outside KeyPressed) ---
        else if (event.type == sf::Event::TextEntered && mShowRegistration) {
            // Check for backspace (ASCII 8)
            if (event.text.unicode == 8) {
                if (mPlayerNameInput.getSize() > 0) {
                    mPlayerNameInput.erase(mPlayerNameInput.getSize() - 1);
                }
            }
            // Check for valid character (alphanumeric or space) and max length
            else if (event.text.unicode >= 32 && event.text.unicode <= 126 && mPlayerNameInput.getSize() < 12) {
                mPlayerNameInput += static_cast<char>(event.text.unicode);
            }
            mRegInputText.setString(mPlayerNameInput);
        }
        // --- End Text Entered Event Handling ---
    }
}

void GameEngine::update(sf::Time deltaTime) {
    mPlayer->update(deltaTime);
    mTrackManager->update(deltaTime);
    mScoreManager->addScore(10.0f *
        deltaTime.asSeconds()); // 10 points per second

    // Day/Night Cycle (using constant)
    mDayNightTimer += deltaTime.asSeconds();
    float brightness = 0.4f + 0.6f * (0.5f + 0.5f * std::cos(mDayNightTimer * (2.0f * M_PI / DAY_NIGHT_CYCLE_DURATION)));
    sf::Uint8 val = static_cast<sf::Uint8>(brightness * 255);
    mBackgroundSprite.setColor(sf::Color(val, val, val));

    if (mPlayer->isDoubleCoinActive()) {
        mScoreManager->setMultiplier(2);
    }
    else {
        mScoreManager->setMultiplier(1);
    }

    handleCollisions();

    // Magnet Logic (using constants)
    if (mPlayer->isMagnetActive()) {
        auto& coins = mTrackManager->getCoins();
        sf::FloatRect playerBounds = mPlayer->getBounds();
        sf::Vector2f playerPos(playerBounds.left + playerBounds.width / 2.0f,
            playerBounds.top + playerBounds.height / 2.0f);
        for (auto* coin : coins) { 
            sf::Vector2f coinPos = coin->getPosition();
            float dx = playerPos.x - coinPos.x;
            float dy = playerPos.y - coinPos.y;
            float dist = std::sqrt(dx * dx + dy * dy);
            
            // Only pull coins within the magnet range
            if (dist < MAGNET_DISTANCE) { 
                // Pull coin towards the player center
                coin->setPosition(
                    sf::Vector2f(coinPos.x + dx * MAGNET_PULL_SPEED * deltaTime.asSeconds(),
                        coinPos.y + dy * MAGNET_PULL_SPEED * deltaTime.asSeconds()));
            }
        }
    }

    // Update HUD
    std::stringstream ss;
    // UPDATED: Changed to getPlayerName() to match compiler suggestion and standard naming
    ss << "Player: " << mScoreManager->getPlayerName()
        << "\nScore: " << (int)mScoreManager->getScore()
        << "\nCoins: " << mScoreManager->getCoins()
        << "\nHigh Score: " << (int)mScoreManager->getHighScore();
        
    // Append Debug Mode status
    if (mIsDebugMode) {
        ss << "\n*** DEBUG MODE ON ***";
    }

    mScoreText.setString(ss.str());
}

void GameEngine::handleCollisions() {
    sf::FloatRect playerBounds = mPlayer->getBounds();

    // Check Obstacles
    auto& obstacles = mTrackManager->getObstacles();
    for (auto* obs : obstacles) {
        if (obs->getBounds().intersects(playerBounds)) {
            bool fatal = obs->onCollision(*mPlayer);
            if (fatal) {
                // Game Over - Text remains blank
                mGameOverText.setString(""); 
                
                mIsGameOver = true;
                
                // --- FILE HANDLING: SAVE ---
                // Save the current score if it's a new high score.
                mScoreManager->saveHighScore();
                
                // Save this game session to history (ALL scores are logged)
                mScoreManager->saveGameHistory();
                // ---------------------------
                
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
                sf::Vector2f(-1000, -1000)); 
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
    ground.setFillColor(sf::Color(groundVal, groundVal, groundVal));
    mWindow.draw(ground);

    // Draw Lane Lines
    sf::RectangleShape line(sf::Vector2f(5.0f, 600.0f));
    sf::Uint8 lineVal = static_cast<sf::Uint8>(255.0f * brightness);
    line.setFillColor(sf::Color(lineVal, lineVal, lineVal));
    line.setPosition(sf::Vector2f(300.0f, 0.0f));
    mWindow.draw(line);
    line.setPosition(sf::Vector2f(500.0f, 0.0f));
    mWindow.draw(line);

    if (!mShowMenu && !mShowRegistration) { // Game is running
        mTrackManager->draw(mWindow);
        mPlayer->draw(mWindow);

        // --- COLLISION DEBUGGING DRAWING (Activated by TAB) ---
        if (mIsDebugMode) {
            sf::FloatRect playerBounds = mPlayer->getBounds();
            sf::RectangleShape playerDebugBox;
            playerDebugBox.setPosition(playerBounds.left, playerBounds.top);
            playerDebugBox.setSize(sf::Vector2f(playerBounds.width, playerBounds.height));
            playerDebugBox.setFillColor(sf::Color(0, 255, 0, 150)); 
            mWindow.draw(playerDebugBox);

            auto& obstacles = mTrackManager->getObstacles();
            for (auto* obs : obstacles) {
                sf::FloatRect obsBounds = obs->getBounds();
                sf::RectangleShape obsDebugBox;
                obsDebugBox.setPosition(obsBounds.left, obsBounds.top);
                obsDebugBox.setSize(sf::Vector2f(obsBounds.width, obsBounds.height));
                obsDebugBox.setFillColor(sf::Color(255, 0, 0, 150)); 
                mWindow.draw(obsDebugBox);
            }
        }
        // ------------------------------------

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
    
    // --- Draw Registration Screen ---
    if (mShowRegistration) {
        // Draw dimmed background (using menu background or a simple semi-transparent rect)
        if (mMenuSprite.getTexture()) {
            mWindow.draw(mMenuSprite);
        } else {
             sf::RectangleShape overlay(sf::Vector2f(mWindow.getSize().x, mWindow.getSize().y));
             overlay.setFillColor(sf::Color(0, 0, 0, 180));
             mWindow.draw(overlay);
        }

        mWindow.draw(mRegPanel);
        mWindow.draw(mRegPromptText);
        mWindow.draw(mRegInputBox);
        mWindow.draw(mRegInputText);
    }

    mWindow.display();
}

void GameEngine::updatePauseSprite() {
    auto& rm = ResourceManager::get();
    const std::string textureKey = mIsPaused ? "play" : "pause";
    if (rm.hasTexture(textureKey)) {
        mPauseSprite.setTexture(rm.getTexture(textureKey));
        mPauseSprite.setScale(0.6f, 0.6f);
    } else {
        std::cerr << "Warning: Pause/Play texture '" << textureKey << "' not found." << std::endl;
    }
}

void GameEngine::updateMenuVisualState() {
    // Update Play/Resume text based on pause state
    if (mIsPaused) {
        mMenuOptions[0].setString("Resume");
    } else {
        mMenuOptions[0].setString("Play");
    }
    // Re-center the text
    auto bounds = mMenuOptions[0].getLocalBounds();
    mMenuOptions[0].setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);

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
    // UPDATED: Using getHighScoreName() to show the Record Holder
    ss << "High Score: " << static_cast<int>(mScoreManager->getHighScore())
        << "\nBy: " << mScoreManager->getHighScoreName();
    mHighscoreText.setString(ss.str());
    auto bounds = mHighscoreText.getLocalBounds();
    mHighscoreText.setOrigin(bounds.left + bounds.width / 2.f, 0.f);
}
