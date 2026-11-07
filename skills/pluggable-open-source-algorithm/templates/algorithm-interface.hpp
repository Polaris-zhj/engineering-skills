#pragma once

#include <memory>
#include <string>
#include <vector>

namespace project::algorithm {

// Replace these examples with project-owned domain types.
// Do not include third-party library headers in this file.
struct Input {
  // Stable, documented domain data.
};

struct Output {
  // Stable, documented domain result.
};

enum class Status {
  kOk,
  kInvalidInput,
  kUnsupported,
  kBackendError,
};

struct Result {
  Status status{Status::kBackendError};
  Output output{};
  std::string message;
};

struct Config {
  // Common project-owned configuration only.
};

class AlgorithmInterface {
 public:
  virtual ~AlgorithmInterface() = default;

  // Define constness, concurrency, ownership and error semantics
  // according to the actual domain contract.
  virtual Result Execute(const Input& input) = 0;

  // Optional capability discovery can be added if implementations
  // legitimately support different subsets of the contract.
};

}  // namespace project::algorithm
