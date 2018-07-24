#pragma once

#include "Statistics.hpp"
#include "Types.hpp"

#include <forward_list>
#include <functional>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <unordered_map>

class OrderBook
{
  public:

  OrderBook(std::shared_ptr<Statistics> statistics)
  : mStatistics(statistics)
  {
  }

  struct Order
  {
    OrderId const orderId;
    Price price;
    Quantity quantity;
    Side const side;
  };

  struct Trade
  {
    Price price;
    Quantity quantity;
  };

  auto addOrder(OrderId orderId, Side side, Price price, Quantity quantity)
  {
    std::list<Trade> trades;

    if (mIndex.find(orderId) != mIndex.end())
    {
      mStatistics->incrementDuplicatedAddedOrders();
      return trades;
    }

    std::forward_list<OrderId> matched;

    if (side == BUY)
    {
      for (auto it = mSellers.begin();
           it != mSellers.end() && price >= it->second->price && quantity > 0;
           ++it)
      {
        if (it->second->quantity > quantity)
        {
          it->second->quantity -= quantity;
          trades.push_front(Trade{it->second->price, quantity});
          quantity = 0;
          break;
        }
        else
        {
          trades.push_front(Trade{it->second->price, it->second->quantity});
          quantity -= it->second->quantity;
          matched.push_front(it->second->orderId);
        }
      }

      if (quantity > 0)
      {
        auto order = std::make_shared<Order>(Order{orderId, price, quantity, side});
        mBuyers.insert(std::pair<Price, std::shared_ptr<Order>>(price, order));
        mIndex.insert(std::pair<OrderId, std::shared_ptr<Order>>(orderId, order));
      }
    }
    else
    {
      for (auto it = mBuyers.begin(); it != mBuyers.end() && price <= it->second->price && quantity > 0; ++it)
      {
        if (it->second->quantity > quantity)
        {
          it->second->quantity -= quantity;
          trades.push_front(Trade{it->second->price, quantity});
          quantity = 0;
          break;
        }
        else
        {
          trades.push_front(Trade{it->second->price, it->second->quantity});
          quantity -= it->second->quantity;
          matched.push_front(it->second->orderId);
        }
      }

      if (quantity > 0)
      {
        auto order = std::make_shared<Order>(Order{orderId, price, quantity, side});
        mSellers.insert(std::pair<Price, std::shared_ptr<Order>>(price, order));
        mIndex.insert(std::pair<OrderId, std::shared_ptr<Order>>(orderId, order));
      }
    }

    for (auto orderId : matched)
    {
      removeOrder(orderId);
    }

    return trades;
  }

  auto modifyOrder(OrderId orderId, Side side, Price price, Quantity quantity)
  {
    std::list<Trade> trades;

    auto it = mIndex.find(orderId);

    if (it != mIndex.end())
    {
      auto order = it->second;

      // TODO optimize this code (quite slow if side or price is different)
      if (order->price == price && order->side == side)
      {
        order->quantity = quantity;
      }
      else
      {
        removeOrder(orderId);
        trades = addOrder(orderId, side, price, quantity);
      }
    }
    else
    {
      mStatistics->incrementInvalidParameters();
    }

    return trades;
  }

  void removeOrder(OrderId orderId)
  {
    std::shared_ptr<Order> order;

    {
      auto it = mIndex.find(orderId);

      if (it == mIndex.end())
      {
        mStatistics->incrementInvalidParameters();
        return;
      }

      order = it->second;
      mIndex.erase(it);
    }

    if (order->side == SELL)
    {
      auto range = mSellers.equal_range(order->price);

      for (auto it = range.first; it != range.second; ++it)
      {
        if (order->orderId == orderId)
        {
          mSellers.erase(it);
          break;
        }
      }
    }
    else
    {
      auto range = mBuyers.equal_range(order->price);

      for (auto it = range.first; it != range.second; ++it)
      {
        if (order->orderId == it->second->orderId)
        {
          mBuyers.erase(it);
          break;
        }
      }
    }
  }

  void print(std::ostream &os) const
  {
    std::stringstream buffer;

    buffer << "+------------ ORDER BOOK ------------+" << std::endl;
    buffer << "|    qty     bid   |     ask     qty |" << std::endl;

    auto bid = mBuyers.begin();
    auto ask = mSellers.begin();

    std::string const blank(15, ' ');

    while (bid != mBuyers.end() || ask != mSellers.end())
    {
      buffer << "| ";

      if (bid != mBuyers.end())
      {
        buffer << std::setfill(' ') << std::setw(6) << bid->second->quantity;
        buffer << " ";
        buffer << std::setfill(' ') << std::setw(8) << std::fixed << std::setprecision(2) << bid->first;
        ++bid;
      }
      else
      {
        buffer << blank;
      }

      buffer << "  | ";

      if (ask != mSellers.end())
      {
        buffer << std::setfill(' ') << std::setw(8) << std::fixed << std::setprecision(2) << ask->first;
        buffer << " ";
        buffer << std::setfill(' ') << std::setw(6) << ask->second->quantity;
        ++ask;
      }
      else
      {
        buffer << blank;
      }

      buffer << " |\n";
    }

    buffer << "+------------------------------------+" << std::endl;
    
    os << buffer.str();
  }

  private:

  std::shared_ptr<Statistics> mStatistics;
  std::unordered_map<OrderId, std::shared_ptr<Order>> mIndex;
  std::multimap<Price, std::shared_ptr<Order>, std::greater<Price>> mBuyers;
  std::multimap<Price, std::shared_ptr<Order>, std::less<Price>> mSellers;
};

