#include "ConsoleSinkImpl.hpp"
#include <iostream>

void ConsoleSinkImpl::writeMessage(const LogMessage &msg) {
  std::cout << msg << '\n';
}