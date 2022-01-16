#pragma once
#include <cstdint>
#include <filesystem>
#include <future>
#include <math.h>
#include <numbers>
#include <stack>
#include <utility>
#include <vector>
#include <x86intrin.h>
/// A thread safe 128-bit SIMD sound middleware
namespace statey {
using U8 = std::uint8_t;
using U16 = std::uint16_t;
using U32 = std::uint32_t;
using U64 = std::uint64_t;

using S8 = std::int8_t;
using S16 = std::int16_t;
using S32 = std::int32_t;
using S64 = std::int64_t;

using F32 = float;
using F64 = double;
using F32x4 = F32 __attribute__((vector_size(16)));

enum class role : U8 {
  lpf,
  hpf,
  echo,
  gen,
};
class block {
  std::vector<F32x4> _storage;

public:
  const role type;
  void render(F32x4 &, F32x4 &, std::vector<F32x4> &&, F32x4);
  explicit block(const role);
};

struct instrument {
  std::stack<block> blocks{};
};

class context {
  std::vector<instrument> _instruments{};

public:
  using chunk = std::vector<F32>;

  explicit context(std::filesystem::path &&); /// loads a notes dump
  std::future<chunk> render(const U32);       /// renders sound data in stereo
};
} // namespace statey
