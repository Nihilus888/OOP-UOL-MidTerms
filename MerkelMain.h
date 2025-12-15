#pragma once

#include <vector>
#include "OrderBookEntry.h"
#include "OrderBook.h"
#include "Wallet.h"
#include "User.h"

class MerkelMain
{
public:
    MerkelMain();
    void init();
    void printMenu();
    void printHelp();
    void printMarketStats();
    void enterAsk();
    void enterBid();
    void printWallet();
    void gotoNextTimeframe();
    int getUserOption();
    void showCandlesticks();
    void processUserOption(int userOption);

private:
    OrderBook orderBook;
    Wallet wallet;
    std::string currentTime;

    // Add these for login/registration
    User currentUser;
    bool isLoggedIn = false;
};
