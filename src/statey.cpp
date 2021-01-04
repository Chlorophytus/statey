#include "statey.hpp"
#include "statey_audio.hpp"
#include "statey_midi.hpp"
int main(int argc, char const *argv[]) {
  auto audio = std::make_unique<statey::audio>();
  auto midi = std::make_unique<statey::midi>(1);
  for (;;) {
    if (!midi->queue.empty()) {
      auto mesg = midi->queue.front();
      switch (mesg.code) {
      case 0x80: {
        audio->step_off();
        break;
      }
      case 0x90: {
        audio->step_on(mesg.data0.value(), mesg.data1.value() / 127.0f);
        break;
      }
      case 0xB0: {
        auto control_command = mesg.data0.value();
        switch (control_command) {
        case 1: {
          auto setting = std::pow(mesg.data1.value() / 127.0f, 2.0f) / 1024.0f;
          for (auto &&voice : audio->voices) {
            voice->osc0.vibrato_freqattenuate = setting;
            voice->osc1.vibrato_freqattenuate = setting;
          }
          std::fprintf(stderr, "set vib amp to %f\n", setting);
          break;
        }
        case 21: {
          auto setting = std::pow(mesg.data1.value() / 127.0f, 2.0f) *
                         (SAMPLE_RATE / 8.0f);
          for (auto &&voice : audio->voices) {
            voice->svf0.frequency = setting;
          }
          std::fprintf(stderr, "set Fc Hi to %f\n", setting);
          break;
        }
        case 22: {
          auto setting =
              0.5f + (std::pow(mesg.data1.value() / 127.0f, 2.0f) * 99.5f);
          for (auto &&voice : audio->voices) {
            voice->svf0.q_factor = setting;
          }
          std::fprintf(stderr, "set Q Hi to %f\n", setting);
          break;
        }
        case 23: {
          auto setting = std::pow(mesg.data1.value() / 127.0f, 2.0f) *
                         (SAMPLE_RATE / 8.0f);
          for (auto &&voice : audio->voices) {
            voice->svf1.frequency = setting;
          }
          std::fprintf(stderr, "set Fc Lo to %f\n", setting);
          break;
        }
        case 24: {
          auto setting =
              0.5f + (std::pow(mesg.data1.value() / 127.0f, 2.0f) * 99.5f);
          for (auto &&voice : audio->voices) {
            voice->svf1.q_factor = setting;
          }
          std::fprintf(stderr, "set Q Lo to %f\n", setting);
          break;
        }
        case 25: {
          auto setting = mesg.data1.value() / 4096.0f;
          for (auto &&voice : audio->voices) {
            voice->osc1.frequency_multi2fine = setting;
          }
          std::fprintf(stderr, "set V1detune fine to %f\n", setting);
          break;
        }
        case 44: {
          auto setting = std::pow(mesg.data1.value() / 127.0f, 4.0f);
          for (auto &&voice : audio->voices) {
            voice->amp0.attack_rate = setting;
          }
          std::fprintf(stderr, "set initial to %f\n", setting);
          break;
        }
        case 45: {
          auto setting = std::pow(mesg.data1.value() / 127.0f, 4.0f);
          for (auto &&voice : audio->voices) {
            voice->amp0.release_rate = setting;
          }
          std::fprintf(stderr, "set release to %f\n", setting);
          break;
        }
        case 41: {
          auto setting = mesg.data1.value() / 127.0f;
          for (auto &&voice : audio->voices) {
            voice->osc0.morph = setting;
          }
          std::fprintf(stderr, "set V0morph to %f\n", setting);
          break;
        }
        case 42: {
          auto setting = mesg.data1.value() / 127.0f;
          for (auto &&voice : audio->voices) {
            voice->osc1.morph = setting;
          }
          std::fprintf(stderr, "set V1morph to %f\n", setting);
          break;
        }
        case 43: {
          auto rawsetting =
              static_cast<uint8_t>(mesg.data1.value() / 127.0f * 4.0f);
          auto setting = "what";
          auto realsetting = 1.0f;
          switch (rawsetting) {
          case 0: {
            setting = "16'";
            realsetting = 0.25f;
            break;
          }
          case 1: {
            setting = "8'";
            realsetting = 0.5f;
            break;
          }
          case 2: {
            setting = "4'";
            realsetting = 1.0f;
            break;
          }
          case 3: {
            setting = "2'";
            realsetting = 2.0f;
            break;
          }
          case 4: {
            setting = "1 1/2'";
            realsetting = 3.0f;
            break;
          }
          }

          for (auto &&voice : audio->voices) {
            voice->osc1.frequency_multi2coarse = realsetting;
          }
          std::fprintf(stderr, "set V1detune coarse to %s\n", setting);
          break;
        }
        default:
          break;
        }
        break;
      }
      case 0xD0: {
        auto setting = std::pow(mesg.data0.value() / 127.0f, 2.0f) / 16.0f;
        for (auto &&voice : audio->voices) {
          voice->osc0.vibrato_amplitude = setting;
          voice->osc1.vibrato_amplitude = setting;
        }
        std::fprintf(stderr, "channel aftertouch %f\n", setting);
        break;
      }
      case 0xE0: {
        auto setting = (mesg.data1.value() / 127.0f) + 0.5f;
        for (auto &&voice : audio->voices) {
          voice->osc0.frequency_multi = setting;
          voice->osc1.frequency_multi = setting;
        }
        break;
      }
      default: {
        std::fprintf(stderr, "DM:%2hhX ", mesg.code);
        if (mesg.data0.has_value()) {
          std::fprintf(stderr, "D0:%2hhX ", mesg.data0.value());
        }
        if (mesg.data1.has_value()) {
          std::fprintf(stderr, "D1:%2hhX ", mesg.data1.value());
        }
        std::fprintf(stderr, "\n");
        break;
      }
      }
      midi->queue.pop();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  return 0;
}
