#pragma once

class LaneSystem {
public:
    static const int LANE_COUNT = 3;
    static const int LANE_WIDTH = 200;
    static const int CENTER_X = 400;

    static float getLaneCenter(int laneIndex) {
        if (laneIndex < 0)
            laneIndex = 0;
        if (laneIndex >= LANE_COUNT)
            laneIndex = LANE_COUNT - 1;

        // 0 -> Left (200), 1 -> Center (400), 2 -> Right (600)
        return CENTER_X + (laneIndex - 1) * LANE_WIDTH;
    }
};
#pragma once
