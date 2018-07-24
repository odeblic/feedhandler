#include "FeedHandler.hpp"

#include <fstream>
#include <iostream>

int main(int argc, char ** argv)
{
  if (argc != 2)
  {
    std::cout << "usage: " << argv[0] << " messages.txt" << std::endl;
    return 1;
  }

  FeedHandler feed;
  std::string line;

  std::string const filename(argv[1]);
  std::ifstream file(filename.c_str(), std::ios::in);

  int counter = 0;

  while (std::getline(file, line))
  {
    feed.processMessage(line);

    if (++counter % 10 == 0)
    {
      feed.printOrderBook(std::cout);
    }
  }

  feed.printOrderBook(std::cout);

  feed.printStatistics(std::cout);

  return 0;
}

