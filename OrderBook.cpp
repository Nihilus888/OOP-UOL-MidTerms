#include "OrderBook.h"
#include "CSVReader.h"
#include <map>
#include <algorithm>
#include <iostream>


OrderBook::OrderBook(std::string filename)
{
    orders = CSVReader::readCSV(filename);
}

std::vector<std::string> OrderBook::getKnownProducts()
{
    std::vector<std::string> products;

    std::map<std::string,bool> prodMap;

    for (OrderBookEntry& e : orders)
    {
        prodMap[e.product] = true;
    }
    
    // flatten the map to a vector of strings
    for (auto const& e : prodMap)
    {
        products.push_back(e.first);
    }

    return products;
}

std::vector<OrderBookEntry> OrderBook::getOrders(OrderBookType type, 
                                        std::string product, 
                                        std::string timestamp)
{
    std::vector<OrderBookEntry> orders_sub;
    for (OrderBookEntry& e : orders)
    {
        if (e.orderType == type && 
            e.product == product && 
            e.timestamp == timestamp )
            {
                orders_sub.push_back(e);
            }
    }
    return orders_sub;
}


double OrderBook::getHighPrice(std::vector<OrderBookEntry>& orders)
{
    double max = orders[0].price;
    for (OrderBookEntry& e : orders)
    {
        if (e.price > max)max = e.price;
    }
    return max;
}


double OrderBook::getLowPrice(std::vector<OrderBookEntry>& orders)
{
    double min = orders[0].price;
    for (OrderBookEntry& e : orders)
    {
        if (e.price < min)min = e.price;
    }
    return min;
}

std::string OrderBook::getEarliestTime()
{
    return orders[0].timestamp;
}

std::string OrderBook::getNextTime(std::string timestamp)
{
    std::string next_timestamp = "";
    for (OrderBookEntry& e : orders)
    {
        if (e.timestamp > timestamp) 
        {
            next_timestamp = e.timestamp;
            break;
        }
    }
    if (next_timestamp == "")
    {
        next_timestamp = orders[0].timestamp;
    }
    return next_timestamp;
}

void OrderBook::insertOrder(OrderBookEntry& order)
{
    orders.push_back(order);
}

std::vector<Candlestick> OrderBook::getCandlesticks(
    std::string product,
    OrderBookType type,
    TimeFrame timeframe)
{
    std::map<std::string, std::vector<OrderBookEntry>> groupedOrders;

    // Filter orders and group by timeframe
    for (const OrderBookEntry& order : orders)
    {
        if (order.product != product || order.orderType != type)
            continue;

        std::string key;

        // timestamp format: YYYY/MM/DD HH:MM:SS.xxxxxx
        if (timeframe == TimeFrame::daily)
        {
            key = order.timestamp.substr(0, 10);   // YYYY/MM/DD
        }
        else if (timeframe == TimeFrame::monthly)
        {
            key = order.timestamp.substr(0, 7);    // YYYY/MM
        }
        else // yearly
        {
            key = order.timestamp.substr(0, 4);    // YYYY
        }

        groupedOrders[key].push_back(order);
    }

    // Create candlesticks
    std::vector<Candlestick> candlesticks;

    for (auto& pair : groupedOrders)
    {
        auto& entries = pair.second;

        // sort by timestamp to get Open & Close
        std::sort(entries.begin(), entries.end(),
                  [](const OrderBookEntry& a, const OrderBookEntry& b)
                  {
                      return a.timestamp < b.timestamp;
                  });

        double open  = entries.front().price;
        double close = entries.back().price;
        double high  = entries.front().price;
        double low   = entries.front().price;

        for (const auto& e : entries)
        {
            if (e.price > high) high = e.price;
            if (e.price < low)  low  = e.price;
        }

        Candlestick c(
            pair.first,
            open,
            high,
            low,
            close
        );

        candlesticks.push_back(c);
    }

    return candlesticks;
}

std::vector<OrderBookEntry> OrderBook::matchAsksToBids(std::string product, std::string timestamp)
{
    std::vector<OrderBookEntry> asks = getOrders(OrderBookType::ask, 
                                                 product, 
                                                 timestamp);

    std::vector<OrderBookEntry> bids = getOrders(OrderBookType::bid, 
                                                 product, 
                                                    timestamp);

    std::vector<OrderBookEntry> sales; 

    if (asks.size() == 0 || bids.size() == 0)
    {
        std::cout << " OrderBook::matchAsksToBids no bids or asks" << std::endl;
        return sales;
    }

    // sort asks lowest first
    std::sort(asks.begin(), asks.end(), OrderBookEntry::compareByPriceAsc);
    // sort bids highest first
    std::sort(bids.begin(), bids.end(), OrderBookEntry::compareByPriceDesc);
    // for ask in asks:
    std::cout << "max ask " << asks[asks.size()-1].price << std::endl;
    std::cout << "min ask " << asks[0].price << std::endl;
    std::cout << "max bid " << bids[0].price << std::endl;
    std::cout << "min bid " << bids[bids.size()-1].price << std::endl;
    
    for (OrderBookEntry& ask : asks)
    {
        for (OrderBookEntry& bid : bids)
        {
            if (bid.price >= ask.price)
            {

            OrderBookEntry sale{ask.price, 0, timestamp, 
                product, 
                OrderBookType::asksale};

                if (bid.username == "simuser")
                {
                    sale.username = "simuser";
                    sale.orderType = OrderBookType::bidsale;
                }
                if (ask.username == "simuser")
                {
                    sale.username = "simuser";
                    sale.orderType =  OrderBookType::asksale;
                }
            
                if (bid.amount == ask.amount)
                {
                    sale.amount = ask.amount;
                    sales.push_back(sale);
                    bid.amount = 0;
                    break;
                }

                if (bid.amount > ask.amount)
                {

                    sale.amount = ask.amount;

                    sales.push_back(sale);

                    bid.amount =  bid.amount - ask.amount;
                    break;
                }


                if (bid.amount < ask.amount && 
                   bid.amount > 0)
                {
                    sale.amount = bid.amount;
                    sales.push_back(sale);
                    ask.amount = ask.amount - bid.amount;
                    bid.amount = 0;
                    continue;
                }
            }
        }
    }
    return sales;             
}
