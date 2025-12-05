#pragma once
#include "Player.h"
#include "ScoreManager.h"
#include "TrackManager.h"
#include "Obstacle.h" 
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <memory>


class GameEngine {
public:
    GameEngine();
    void run();

private:
    enum class MenuItem { Play = 0, Highscore = 1 };

    void processEvents();
    void update(sf::Time deltaTime);
    void render();
    void handleCollisions();
    void resetGame(const std::string& playerName = "Player"); 
    
    void loadMenuResources();       
    void loadAllGameResources();    
    
    void updatePauseSprite();
    void updateMenuVisualState();
    void refreshHighscoreText();

    sf::RenderWindow mWindow;
    std::unique_ptr<Player> mPlayer;
    std::unique_ptr<TrackManager> mTrackManager;
    std::unique_ptr<ScoreManager> mScoreManager;

    sf::Font mFont;
    sf::Text mScoreText;
    sf::Text mGameOverText;
    sf::Sound mBgMusic;
    sf::Sprite mBackgroundSprite;
    sf::Sprite mHudBannerSprite;
    sf::Sprite mCoinIcon;
    sf::Sprite mPauseSprite;
    sf::Sprite mGameOverSprite;
    sf::Sprite mMenuSprite;

    sf::RectangleShape mHudPanel;
    sf::RectangleShape mMenuPanel;
    sf::Text mMenuOptions[2];
    sf::Text mMenuHintText;
    sf::Text mHighscoreText;

    // --- Registration UI Elements ---
    sf::RectangleShape mRegPanel;
    sf::RectangleShape mRegInputBox;
    sf::Text mRegPromptText;
    sf::Text mRegInputText;
    sf::String mPlayerNameInput; // Stores the input text
    // --------------------------------

    bool mIsPaused;
    bool mIsGameOver;
    bool mShowMenu;
    bool mShowHighscorePanel;
    MenuItem mMenuSelection;
    
    // Reordered to match initialization in GameEngine.cpp
    bool mAreGameAssetsLoaded = false;
    bool mIsDebugMode = false; 
    bool mShowRegistration = false; 
    
    float mDayNightTimer;
};