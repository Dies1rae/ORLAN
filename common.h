#pragma once

#include <string>

inline static constexpr int pointPenWidth = 4;
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
    std::string data_hash_ = ""; //here i can do a crc, but i think about some simple hsh
};
