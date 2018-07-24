#pragma once

#include "OrderBook.hpp"
#include "Parser.hpp"
#include "Statistics.hpp"
#include "Types.hpp"

#include <iostream>
#include <list>
#include <memory>
#include <sstream>

class FeedHandler
{
  public:

  FeedHandler()
  : mStatistics(std::make_shared<Statistics>()),
    mParser(mStatistics), mBook(mStatistics)
  {
  }

  void processMessage(std::string const &line)
  {
    auto message = mParser.parse(line);

    if (!message)
    {
      return;
    }

    switch (message->type)
    {
      case ADD_ORDER:
      ignoreTrades();
      mTrades = mBook.addOrder(message->orderId,
                               message->side,
                               message->price,
                               message->quantity);
      break;

      case MODIFY_ORDER:
      ignoreTrades();
      mTrades = mBook.modifyOrder(message->orderId,
                                  message->side,
                                  message->price,
                                  message->quantity);
      break;

      case REMOVE_ORDER:
      ignoreTrades();
      mBook.removeOrder(message->orderId);
      break;

      case TRADE:
      acknowledgeTrade(message->price, message->quantity);
      return;
    }

    printTransaction(line, std::cout);
  }

  void printOrderBook(std::ostream &os) const
  {
    mBook.print(os);
  }

  void printStatistics(std::ostream &os) const
  {
    mStatistics->print(os);
  }

  private:

  void acknowledgeTrade(Price price, Quantity quantity)
  {
    for (auto it = mTrades.begin(); it != mTrades.end(); ++it)
    {
      if (it->price == price && it->quantity == quantity)
      {
        mTrades.erase(it);
        break;
      }
    }
  }

  void ignoreTrades()
  {
    // TODO unnecessary branch : check if this is faster
    if (mTrades.size() > 0)
    {
      mStatistics->incrementMissingExpectedTrades(mTrades.size());
      mTrades.clear();
    }
  }

  void printTransaction(std::string const &msg, std::ostream &os) const
  {
    std::stringstream buffer;

    if (mTrades.empty())
    {
      os << msg << std::endl;
      return;
    }

    os << msg << " => ";

    for (auto it = mTrades.begin(); it != mTrades.end(); ++it)
    {
      if (it != mTrades.begin())
      {
        os << ", ";
      }

      os << it->quantity << '@' << it->price;
    }

    os << std::endl;

    os << buffer.str();
  }

  std::shared_ptr<Statistics> mStatistics;
  Parser mParser;
  OrderBook mBook;
  std::list<OrderBook::Trade> mTrades;
};

