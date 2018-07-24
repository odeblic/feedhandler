#pragma once

#include "Statistics.hpp"
#include "Types.hpp"

#include <iostream>
#include <memory>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

class Parser
{
  public:

  Parser(std::shared_ptr<Statistics> statistics)
  : mStatistics(statistics)
  {
  }

  struct Message
  {
    MsgType  type;
    Price    price;
    Quantity quantity;
    OrderId  orderId;
    Side     side;
  };

  // TODO optimize this method by parsing on-the-fly
  auto parse(std::string const &line) const
  {
    std::unique_ptr<Message> message = std::make_unique<Message>();

    if (line.size() == 0 || line[0] == '#')
    {
      message.reset();
      return message;
    }

    std::vector<std::string> fields;
    boost::split(fields, line, boost::is_any_of(","), boost::token_compress_off);

    int invalidParametersCounter{0};

    try
    {
      message->type = static_cast<MsgType>(boost::lexical_cast<char>(fields[0]));

      switch (message->type)
      {
        case ADD_ORDER:
        case MODIFY_ORDER:
        case REMOVE_ORDER:

        if (fields.size() != 5)
        {
          throw boost::bad_lexical_cast{};
        }

        message->orderId = boost::lexical_cast<OrderId>(fields[1]);
        message->side = static_cast<Side>(boost::lexical_cast<char>(fields[2]));

        if (message->side != BUY && message->side != SELL)
        {
          throw boost::bad_lexical_cast{};
        }

        message->quantity = boost::lexical_cast<Quantity>(fields[3]);
        message->price    = boost::lexical_cast<Price>(fields[4]);

        if (message->price <= 0)
        {
          ++invalidParametersCounter;
        }

        if (message->quantity <= 0)
        {
          ++invalidParametersCounter;
        }

        break;

        case TRADE:

        if (fields.size() != 3)
        {
          throw boost::bad_lexical_cast{};
        }

        message->quantity = boost::lexical_cast<Quantity>(fields[1]);
        message->price    = boost::lexical_cast<Price>(fields[2]);

        if (message->price <= 0)
        {
          ++invalidParametersCounter;
        }

        if (message->quantity <= 0)
        {
          ++invalidParametersCounter;
        }

        break;

        default:
        throw boost::bad_lexical_cast{};
      }
    }
    catch (boost::bad_lexical_cast const &e)
    {
      mStatistics->incrementCorruptedMessages();
      message.reset();
    }

    if (invalidParametersCounter > 0)
    {
      mStatistics->incrementInvalidParameters(invalidParametersCounter);
      message.reset();
    }

    return message;
  }

  private:

  std::shared_ptr<Statistics> mStatistics;
};

