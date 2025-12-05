#pragma once
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <memory>


template <typename T> class GameList {
public:
    GameList() : mData(nullptr), mCapacity(0), mCount(0) {}

    ~GameList() {
        clear();
        delete[] mData; // delete the array of pointers (raw memory management if we
        // used raw pointers, but we use unique_ptr logic manually)
// Actually, we can't easily use new[] for unique_ptr array without default
// constructors. Simplest way for "No vector" is an array of raw pointers
// and we manage deletion, OR an array of unique_ptr objects if we are
// careful. Let's use an array of raw pointers for simplicity in the
// container, but the container OWNS them.
    }

    // Disable copy
    GameList(const GameList&) = delete;
    GameList& operator=(const GameList&) = delete;

    void add(std::unique_ptr<T> item) {
        if (mCount >= mCapacity) {
            resize(mCapacity == 0 ? 4 : mCapacity * 2);
        }
        // We release the unique_ptr ownership to our raw pointer array
        // and we promise to delete it.
        mData[mCount++] = item.release();
    }

    void updateAll(sf::Time dt, float speed) {
        for (size_t i = 0; i < mCount; ++i) {
            mData[i]->update(dt, speed);
        }

        // Remove items
        // Custom remove_if logic
        size_t writeIdx = 0;
        for (size_t readIdx = 0; readIdx < mCount; ++readIdx) {
            if (!mData[readIdx]->isRemovable()) {
                if (writeIdx != readIdx) {
                    mData[writeIdx] = mData[readIdx];
                }
                writeIdx++;
            }
            else {
                delete mData[readIdx]; // Delete the object
            }
        }
        mCount = writeIdx;
    }

    void drawAll(sf::RenderWindow& window) {
        for (size_t i = 0; i < mCount; ++i) {
            mData[i]->draw(window);
        }
    }

    // Iterators for range-based for loops
    T** begin() { return mData; }
    T** end() { return mData + mCount; }

    void clear() {
        for (size_t i = 0; i < mCount; ++i) {
            delete mData[i];
        }
        mCount = 0;
    }

private:
    void resize(size_t newCapacity) {
        T** newData = new T * [newCapacity];
        for (size_t i = 0; i < mCount; ++i) {
            newData[i] = mData[i];
        }
        delete[] mData;
        mData = newData;
        mCapacity = newCapacity;
    }

    T** mData;
    size_t mCapacity;
    size_t mCount;
};
#pragma once
