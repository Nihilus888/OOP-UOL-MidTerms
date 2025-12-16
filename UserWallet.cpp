#include "UserWallet.h"

UserWallet::UserWallet(std::string user, double bal)
    : username(user), balance(bal) {}

void UserWallet::deposit(double amount)
{
    balance += amount;
}

bool UserWallet::withdraw(double amount)
{
    if (balance >= amount)
    {
        balance -= amount;
        return true;
    }
    return false;
}
