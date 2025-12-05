#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <map>
#include <string>
#include <iostream>
#include <filesystem>

// ResourceManager should NOT define ScoreManager. 
// It only manages textures and sounds.

class ResourceManager {
public:
    static ResourceManager& get() {
        static ResourceManager instance;
        return instance;
    }

    // Delete copy constructor and assignment operator
    ResourceManager(const ResourceManager&) = delete;
    void operator=(const ResourceManager&) = delete;

    void loadTexturesFromDirectory(const std::string& directoryPath) {
        for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
            if (entry.path().extension() == ".png") {
                sf::Texture texture;
                if (texture.loadFromFile(entry.path().string())) {
                    std::string filename = entry.path().stem().string();
                    mTextures[filename] = texture;
                }
            }
        }
    }

    sf::Texture& getTexture(const std::string& name) {
        if (mTextures.find(name) == mTextures.end()) {
            // Return a default empty texture or handle error if not found
            static sf::Texture empty;
            return empty;
        }
        return mTextures[name];
    }

    bool hasTexture(const std::string& name) const {
        return mTextures.find(name) != mTextures.end();
    }

    bool loadSound(const std::string& name, const std::string& filename) {
        sf::SoundBuffer buffer;
        if (buffer.loadFromFile(filename)) {
            mSoundBuffers[name] = buffer;
            return true;
        }
        return false;
    }

    sf::SoundBuffer& getSoundBuffer(const std::string& name) {
        return mSoundBuffers[name];
    }

private:
    ResourceManager() {} // Private constructor
    std::map<std::string, sf::Texture> mTextures;
    std::map<std::string, sf::SoundBuffer> mSoundBuffers;
};