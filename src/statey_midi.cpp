#include "statey_midi.hpp"

using namespace statey;

midi::midi(unsigned int port) {
  input = std::unique_ptr<RtMidiIn>(new RtMidiIn());
  input->openPort(port);
  input->setCallback(midi::callback, reinterpret_cast<void *>(this));
}

void midi::callback(double stamp, std::vector<unsigned char> *msg,
                    void *userdata) {
  auto self = reinterpret_cast<midi *>(userdata);
  const auto size = msg->size();
  switch (size) {
  case 1:
    self->queue.emplace(midi_message{msg->at(0)});
    break;

  case 2:
    self->queue.emplace(midi_message{msg->at(0), msg->at(1)});
    break;

  case 3:
    self->queue.emplace(midi_message{msg->at(0), msg->at(1), msg->at(2)});
    break;

  default:
    break;
  }
}

midi::~midi() { input->closePort(); }