#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>

class ScoreManager {
public:
    ScoreManager();

    // Game Loop Methods
    void addScore(float amount);
    void addCoins(int amount);

    // Getters
    float getScore() const { return mScore; }
    int getCoins() const { return mCoins; }
    int getMultiplier() const { return mMultiplier; }
    void setMultiplier(int mult) { mMultiplier = mult; }

    // --- High Score Specific Getters ---
    float getHighScore() const { return mHighScore; }
    // Returns the name of the person who holds the record
    const std::string& getHighScoreName() const { return mHighScoreName; }

    // --- Current Player Methods ---
    // Returns the name of the person currently playing
    // Renamed to 'getPlayerName' to match your GameEngine code
    const std::string& getPlayerName() const { return mCurrentPlayerName; }
    void setPlayerName(const std::string& name);

    // File Handling - High Score
    void saveHighScore();
    void loadHighScore();
    
    // File Handling - Game History (All Scores)
    void saveGameHistory(); // Saves current game session to scores.txt

private:
    // Session Data
    float mScore;
    int mCoins;
    int mMultiplier;
    std::string mCurrentPlayerName; // Name of the person playing right now

    // Persistent Data (Loaded from file)
    float mHighScore;
    std::string mHighScoreName;     // Name of the record holder stored in file

    std::filesystem::path mDataDirectory;
};