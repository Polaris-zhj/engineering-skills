#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

#include "algorithm-interface.hpp"

namespace project::algorithm {

enum class BackendType {
  kThirdParty,
  kCustom,
};

class AlgorithmFactory {
 public:
  using Creator =
      std::unique_ptr<AlgorithmInterface> (*)(const Config& config);

  void Register(BackendType type, Creator creator) {
    if (creator == nullptr) {
      throw std::invalid_argument("Algorithm creator must not be null");
    }
    const auto [it, inserted] = creators_.emplace(type, creator);
    if (!inserted) {
      throw std::logic_error("Algorithm backend already registered");
    }
  }

  std::unique_ptr<AlgorithmInterface> Create(
      BackendType type, const Config& config) const {
    const auto it = creators_.find(type);
    if (it == creators_.end()) {
      throw std::invalid_argument("Unknown algorithm backend");
    }
    return it->second(config);
  }

 private:
  std::unordered_map<BackendType, Creator> creators_;
};

// This is a minimal illustrative factory. In production, decide:
// - whether registration is mutable after startup;
// - whether access is concurrent;
// - how backend-specific configuration is represented and validated;
// - whether instances are per-owner or cached/shared;
// - how construction errors are reported without exceptions, if required.

}  // namespace project::algorithm
