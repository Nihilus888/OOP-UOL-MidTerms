#pragma once
#include <string>

class Candlestick
{
public:
    std::string date;   // YYYY
    double open;
    double high;
    double low;
    double close;

    Candlestick(std::string date,
                double open,
                double high,
                double low,
                double close);
};