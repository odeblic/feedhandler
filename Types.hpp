#pragma once

#include <cstdint>

typedef uint64_t OrderId;

enum MsgType : char
{
  ADD_ORDER    = 'A',
  MODIFY_ORDER = 'M',
  REMOVE_ORDER = 'X',
  TRADE        = 'T',
};

enum Side : char
{
  BUY  = 'B',
  SELL = 'S',
};

typedef double Price;

typedef int32_t Quantity;

