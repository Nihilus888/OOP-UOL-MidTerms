#pragma once
#include <string>

class UserWallet {
public:
    std::string username;
    double balance = 0.0;

    UserWallet() = default;
    UserWallet(std::string user, double bal);

    void deposit(double amount);
    bool withdraw(double amount);
};
