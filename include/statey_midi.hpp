#pragma once
#include "statey.hpp"
namespace statey {
struct midi_message {
  unsigned char code;
  std::optional<unsigned char> data0 = std::nullopt;
  std::optional<unsigned char> data1 = std::nullopt;
};
struct midi {
  midi(unsigned int);
  ~midi();
  std::unique_ptr<RtMidiIn> input;
  static void callback(double, std::vector<unsigned char> *, void *);
  std::queue<midi_message> queue;
};
} // namespace statey