#pragma once
#include <string>

struct Transaction {
    std::string username;
    std::string type;   // ASK / BID / DEPOSIT / WITHDRAW
    std::string product;
    double price;
    double amount;
    std::string timestamp;
};
