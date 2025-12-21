#include "MerkelMain.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <limits>

#include "User.h"
#include "UserService.h"
#include "OrderBookEntry.h"
#include "CSVReader.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <string>

/* ================= CONSTRUCTOR ================= */

MerkelMain::MerkelMain()
    : orderBook("20200317.csv")
{
}

/* ================= INIT ================= */

void MerkelMain::init()
{
    int input;

    // Login / Registration loop
    while (!isLoggedIn)
    {
        std::cout << "1: Register\n2: Login\nChoose: ";
        std::string choice;
        std::getline(std::cin, choice);

        if (choice == "1")
        {
            currentUser = UserService::registerUser();
            if (!currentUser.username.empty())
                isLoggedIn = true;
        }
        else if (choice == "2")
        {
            currentUser = UserService::loginUser();
            if (!currentUser.username.empty())
                isLoggedIn = true;
        }
        else
        {
            std::cout << "Invalid choice.\n";
        }
    }

    std::cout << "\nWelcome, " << currentUser.fullName << "!\n";

    currentTime = orderBook.getEarliestTime();
    wallet.insertCurrency("BTC", 10);

    while (true)
    {
        printMenu();
        input = getUserOption();
        processUserOption(input);
    }
}

/* ================= MENU ================= */

void MerkelMain::printMenu()
{
    std::cout << "\n1: Print help\n";
    std::cout << "2: Print exchange stats\n";
    std::cout << "3: Make an offer\n";
    std::cout << "4: Make a bid\n";
    std::cout << "5: Print wallet\n";
    std::cout << "6: Continue\n";
    std::cout << "7: View candlestick data\n";
    std::cout << "8: Deposit money\n";
    std::cout << "9: Withdraw money\n";
    std::cout << "10: View recent transactions\n";
    std::cout << "11: View user summary\n";
    std::cout << "12: Simulate trading\n";
    std::cout << "==============\n";
    std::cout << "Current time: " << currentTime << "\n";
}

/* ================= CORE FEATURES ================= */

void MerkelMain::printHelp()
{
    std::cout << "Analyse the market and place bids and asks wisely.\n";
}

void MerkelMain::printMarketStats()
{
    for (const std::string& product : orderBook.getKnownProducts())
    {
        auto asks = orderBook.getOrders(OrderBookType::ask, product, currentTime);
        if (asks.empty()) continue;

        std::cout << "Product: " << product << "\n";
        std::cout << "Asks: " << asks.size() << "\n";
        std::cout << "High: " << OrderBook::getHighPrice(asks) << "\n";
        std::cout << "Low: " << OrderBook::getLowPrice(asks) << "\n";
    }
}

/* ================= ASK / BID ================= */

void MerkelMain::enterAsk()
{
    std::cout << "Enter ask: product,price,amount\n";
    std::string input;
    std::getline(std::cin, input);

    auto tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "Bad input format.\n";
        return;
    }

    try
    {
        OrderBookEntry obe = CSVReader::stringsToOBE(
            tokens[1], tokens[2], currentTime, tokens[0], OrderBookType::ask);

        obe.username = currentUser.username;

        if (wallet.canFulfillOrder(obe))
        {
            orderBook.insertOrder(obe);
            logTransaction("ASK", obe.product, obe.price, obe.amount, currentTime);
        }
        else
        {
            std::cout << "Insufficient funds.\n";
        }
    }
    catch (...)
    {
        std::cout << "Invalid numeric values.\n";
    }
}

void MerkelMain::enterBid()
{
    std::cout << "Enter bid: product,price,amount\n";
    std::string input;
    std::getline(std::cin, input);

    auto tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "Bad input format.\n";
        return;
    }

    try
    {
        OrderBookEntry obe = CSVReader::stringsToOBE(
            tokens[1], tokens[2], currentTime, tokens[0], OrderBookType::bid);

        obe.username = currentUser.username;

        if (wallet.canFulfillOrder(obe))
        {
            orderBook.insertOrder(obe);
            logTransaction("BID", obe.product, obe.price, obe.amount, currentTime);
        }
        else
        {
            std::cout << "Insufficient funds.\n";
        }
    }
    catch (...)
    {
        std::cout << "Invalid numeric values.\n";
    }
}

/* ================= WALLET ================= */

void MerkelMain::printWallet()
{
    std::cout << wallet.toString() << "\n";
}

void MerkelMain::depositMoney()
{
    std::string currency;
    double amount;

    std::cout << "Currency: ";
    std::cin >> currency;
    std::cout << "Amount: ";
    std::cin >> amount;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    wallet.insertCurrency(currency, amount);
    logTransaction("DEPOSIT", "", 0, amount, currentTime);

    std::cout << "Deposit successful.\n";
}

void MerkelMain::withdrawMoney()
{
    std::string currency;
    double amount;

    std::cout << "Currency: ";
    std::cin >> currency;
    std::cout << "Amount: ";
    std::cin >> amount;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (wallet.removeCurrency(currency, amount))
    {
        logTransaction("WITHDRAW", "", 0, amount, currentTime);
        std::cout << "Withdrawal successful.\n";
    }
    else
    {
        std::cout << "Insufficient balance.\n";
    }
}

/* ================= TIME ================= */

void MerkelMain::gotoNextTimeframe()
{
    currentTime = orderBook.getNextTime(currentTime);
}

/* ================= CANDLESTICKS ================= */

void MerkelMain::showCandlesticks()
{
    std::string product;
    int choice;

    std::cout << "Product: ";
    std::cin >> product;
    std::cout << "1=Daily 2=Monthly 3=Yearly: ";
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    OrderBook::TimeFrame tf = OrderBook::TimeFrame::yearly;
    if (choice == 1) tf = OrderBook::TimeFrame::daily;
    if (choice == 2) tf = OrderBook::TimeFrame::monthly;

    auto candles = orderBook.getCandlesticks(product, OrderBookType::ask, tf);
    for (const auto& c : candles)
        std::cout << c.date << " O:" << c.open
                  << " H:" << c.high
                  << " L:" << c.low
                  << " C:" << c.close << "\n";
}

/* ================= TRANSACTIONS ================= */

void MerkelMain::logTransaction(const std::string& type,
                                const std::string& product,
                                double price,
                                double amount,
                                const std::string& timestamp)
{
    std::ofstream file("transactions.csv", std::ios::app);
    file << currentUser.username << ","
         << type << ","
         << product << ","
         << price << ","
         << amount << ","
         << timestamp << "\n";
}

void MerkelMain::showRecentTransactions(const std::string& username)
{
    std::ifstream file("transactions.csv");
    std::vector<std::string> lines;
    std::string line;

    while (std::getline(file, line))
        if (line.find(username) == 0)
            lines.push_back(line);

    int start = std::max(0, (int)lines.size() - 5);
    for (int i = start; i < lines.size(); ++i)
        std::cout << lines[i] << "\n";
}

void MerkelMain::printUserSummary()
{
    std::ifstream file("transactions.csv");
    std::string line;

    int asks = 0, bids = 0;
    double spent = 0;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string user, type, product, price, amount, time;

        std::getline(ss, user, ',');
        std::getline(ss, type, ',');
        std::getline(ss, product, ',');
        std::getline(ss, price, ',');
        std::getline(ss, amount, ',');
        std::getline(ss, time, ',');

        if (user == currentUser.username)
        {
            if (type == "ASK") asks++;
            if (type == "BID")
            {
                bids++;
                spent += std::stod(price) * std::stod(amount);
            }
        }
    }

    std::cout << "\n--- User Summary ---\n";
    std::cout << "Asks: " << asks << "\n";
    std::cout << "Bids: " << bids << "\n";
    std::cout << "Total spent: " << spent << "\n";
}

/* ================= TASK 4: SIMULATION ================= */

void MerkelMain::simulateUserTrading()
{
    // Real system timestamp (Task 4 requirement)
    auto now_time = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now_time);
    std::tm* tmPtr = std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(tmPtr, "%Y/%m/%d %H:%M:%S");
    std::string now = oss.str();

    auto products = orderBook.getKnownProducts();

    for (const std::string& product : products)
    {
        auto asks = orderBook.getOrders(OrderBookType::ask, product, currentTime);
        auto bids = orderBook.getOrders(OrderBookType::bid, product, currentTime);

        double baseAsk = asks.empty() ? 100.0 : OrderBook::getLowPrice(asks);
        double baseBid = bids.empty() ? 95.0  : OrderBook::getHighPrice(bids);

        for (int i = 0; i < 5; ++i)
        {
            double askPrice = baseAsk * (1.0 + 0.01 * i);
            double bidPrice = baseBid * (1.0 - 0.01 * i);
            double amount = 0.1 * (i + 1);

            OrderBookEntry ask(askPrice, amount, now, product, OrderBookType::ask);
            ask.username = currentUser.username;
            orderBook.insertOrder(ask);
            logTransaction("ASK", product, askPrice, amount, now);

            OrderBookEntry bid(bidPrice, amount, now, product, OrderBookType::bid);
            bid.username = currentUser.username;
            orderBook.insertOrder(bid);
            logTransaction("BID", product, bidPrice, amount, now);
        }
    }

    std::cout << "Simulated trading activity completed.\n";
}

/* ================= INPUT ================= */

int MerkelMain::getUserOption()
{
    std::string line;
    std::cout << "Type 1-12: ";
    std::getline(std::cin, line);

    try { return std::stoi(line); }
    catch (...) { return 0; }
}

/* ================= DISPATCH ================= */

void MerkelMain::processUserOption(int userOption)
{
    if (userOption < 1 || userOption > 12)
    {
        std::cout << "Invalid choice.\n";
        return;
    }

    if (userOption == 1) printHelp();
    if (userOption == 2) printMarketStats();
    if (userOption == 3) enterAsk();
    if (userOption == 4) enterBid();
    if (userOption == 5) printWallet();
    if (userOption == 6) gotoNextTimeframe();
    if (userOption == 7) showCandlesticks();
    if (userOption == 8) depositMoney();
    if (userOption == 9) withdrawMoney();
    if (userOption == 10) showRecentTransactions(currentUser.username);
    if (userOption == 11) printUserSummary();
    if (userOption == 12) simulateUserTrading();
}
