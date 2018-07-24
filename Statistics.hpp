#pragma once

#include <iostream>

class Statistics
{
  public:

  void incrementCorruptedMessages(int nb = 1)
  {
    mCorruptedMessagesCounter += nb;
  }

  void incrementInvalidParameters(int nb = 1)
  {
    mInvalidParametersCounter += nb;
  }

  void incrementDuplicatedAddedOrders(int nb = 1)
  {
    mDuplicatedAddedOrdersCounter += nb;
  }

  void incrementMissingExpectedTrades(int nb = 1)
  {
    mMissingExpectedTradesCounter += nb;
  }

  void print(std::ostream &os) const
  {
    os << "\nStatistics report:\n"
       << "- " << mCorruptedMessagesCounter << " corrupted message(s)\n"
       << "- " << mInvalidParametersCounter << " invalid parameter(s)\n"
       << "- " << mDuplicatedAddedOrdersCounter << " duplicated added order(s)\n"
       << "- " << mMissingExpectedTradesCounter << " missing expected trade(s)\n"
       << std::endl;
  }

  private:

  int mCorruptedMessagesCounter{0};
  int mInvalidParametersCounter{0};
  int mDuplicatedAddedOrdersCounter{0};
  int mMissingExpectedTradesCounter{0};
};

