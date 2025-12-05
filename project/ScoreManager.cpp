#include "ScoreManager.h"
#include <cstdlib> // For std::getenv
#include <ctime>   // For timestamp
#include <iomanip> // For formatting

namespace {
    const char* kDefaultName = "Player";
    const char* kDataFolder = "data"; 
    const char* kHighScoreFile = "highscore.txt";
    const char* kScoresHistoryFile = "scores.txt"; // New: All game scores
}

ScoreManager::ScoreManager()
    // FIX: Initialization order now matches the Header file exactly
    : mScore(0.0f), 
      mCoins(0), 
      mMultiplier(1), 
      mCurrentPlayerName(kDefaultName), 
      mHighScore(0.0f), 
      mHighScoreName(kDefaultName),
      mDataDirectory(std::filesystem::path(kDataFolder)) 
{
    // Attempt to auto-detect system username for the current player
    if (const char* envName = std::getenv("USER")) {
        mCurrentPlayerName = envName;
    }
    else if (const char* envWin = std::getenv("USERNAME")) {
        mCurrentPlayerName = envWin;
    }
    
    // Load existing high score immediately on startup
    loadHighScore();
}

void ScoreManager::addScore(float amount) {
    mScore += amount * mMultiplier;

    // AUTOMATIC UPDATE LOGIC:
    if (mScore > mHighScore) {
        mHighScore = mScore;
        mHighScoreName = mCurrentPlayerName; 
        
        // --- FILE HANDLING: AUTO-SAVE ---
        // Automatically save to file when new high score is achieved
        saveHighScore();
        // ---------------------------------
    }
}

void ScoreManager::addCoins(int amount) {
    mCoins += amount;
    mScore += amount * 50; 
    
    if (mScore > mHighScore) {
        mHighScore = mScore;
        mHighScoreName = mCurrentPlayerName;
        
        // --- FILE HANDLING: AUTO-SAVE ---
        // Automatically save to file when new high score is achieved
        saveHighScore();
        // ---------------------------------
    }
}

void ScoreManager::setPlayerName(const std::string& name) {
    if (!name.empty()) {
        mCurrentPlayerName = name;
    }
}

// ---------------------------------------------------------
// FILE HANDLING - HIGH SCORE
// ---------------------------------------------------------

void ScoreManager::saveHighScore() {
    std::error_code ec;
    std::filesystem::create_directories(mDataDirectory, ec);
    if (ec) {
        std::cerr << "Error creating directory: " << ec.message() << std::endl;
        return;
    }

    auto filePath = mDataDirectory / kHighScoreFile;
    std::ofstream file(filePath);
    
    if (!file.is_open()) {
        std::cerr << "Error opening file for writing." << std::endl;
        return;
    }

    file << mHighScoreName << "\n" << mHighScore << std::endl;
}

void ScoreManager::loadHighScore() {
    auto filePath = mDataDirectory / kHighScoreFile;
    std::ifstream file(filePath);
    
    if (!file.is_open()) {
        mHighScore = 0.0f;
        mHighScoreName = "None";
        return;
    }

    std::string nameLine;
    std::string scoreLine;

    if (!std::getline(file, nameLine)) return;
    if (!std::getline(file, scoreLine)) return;

    try {
        mHighScoreName = nameLine;
        mHighScore = std::stof(scoreLine);
    }
    catch (...) {
        mHighScore = 0.0f;
        mHighScoreName = "None";
    }
}

// ---------------------------------------------------------
// FILE HANDLING - GAME HISTORY (ALL SCORES)
// ---------------------------------------------------------

void ScoreManager::saveGameHistory() {
    std::error_code ec;
    std::filesystem::create_directories(mDataDirectory, ec);
    if (ec) {
        std::cerr << "Error creating directory: " << ec.message() << std::endl;
        return;
    }

    auto filePath = mDataDirectory / kScoresHistoryFile;
    
    // Open in APPEND mode to add to existing records
    std::ofstream file(filePath, std::ios::app);
    
    if (!file.is_open()) {
        std::cerr << "Error opening scores history file for writing." << std::endl;
        return;
    }

    // Get current timestamp
    std::time_t now = std::time(nullptr);
    char timestamp[100];
    std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

    // Format: Timestamp | Player Name | Score | Coins | High Score?
    bool isNewHighScore = (mScore >= mHighScore);
    
    file << timestamp << " | " 
         << mCurrentPlayerName << " | " 
         << static_cast<int>(mScore) << " | " 
         << mCoins << " | "
         << (isNewHighScore ? "HIGH SCORE!" : "")
         << std::endl;
    
    file.close();
}