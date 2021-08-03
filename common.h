#pragma once

inline static constexpr int pointPenWidth = 3;
inline static constexpr int axesPenWidth = 2;
inline static constexpr int defaultX = 700;
inline static constexpr int defaultY = 700;

struct graphSizes
{
    graphSizes() = default;
    int point_pen_width = pointPenWidth;
    int axes_pen_width = axesPenWidth;
    int default_x = defaultX;
    int default_y = defaultY;
};

struct Point
{
    double x_ = 0.0;
    double y_ = 0.0;
};

struct PointInfo
{
    inline static constexpr int hasher_salt_ = 23;
    size_t size_ = 0;
    size_t pair_size_ = 0;
    size_t data_hash_ = 0; //need to think about crc8 or 16??
};
