// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <limits>

#include "core/framework/arena_extend_strategy.h"
#include "core/framework/ortdevice.h"
#include "core/framework/provider_options.h"
#include "core/session/onnxruntime_c_api.h"

namespace onnxruntime {
// Information needed to construct CUDA execution providers.
struct CUDAExecutionProviderExternalAllocatorInfo {
  void* alloc{nullptr};
  void* free{nullptr};
  void* empty_cache{nullptr};

  CUDAExecutionProviderExternalAllocatorInfo() {
    alloc = nullptr;
    free = nullptr;
    empty_cache = nullptr;
  }

  CUDAExecutionProviderExternalAllocatorInfo(void* a, void* f, void* e) {
    alloc = a;
    free = f;
    empty_cache = e;
  }

  bool UseExternalAllocator() const {
    return (alloc != nullptr) && (free != nullptr);
  }
};

struct CUDAExecutionProviderInfo {
  OrtDevice::DeviceId device_id{0};
  size_t gpu_mem_limit{std::numeric_limits<size_t>::max()};                         // Will be over-ridden by contents of `default_memory_arena_cfg` (if specified)
  ArenaExtendStrategy arena_extend_strategy{ArenaExtendStrategy::kNextPowerOfTwo};  // Will be over-ridden by contents of `default_memory_arena_cfg` (if specified)
  OrtCudnnConvAlgoSearch cudnn_conv_algo_search{OrtCudnnConvAlgoSearch::EXHAUSTIVE};
  bool do_copy_in_default_stream{true};
  bool has_user_compute_stream{false};
  void* user_compute_stream{nullptr};
  // The following OrtArenaCfg instance only characterizes the behavior of the default memory
  // arena allocator and not any other auxiliary allocator that may also be part of the CUDA EP.
  // For example, auxiliary allocators `CUDA_PINNED` and `CUDA_CPU` will not be configured using this
  // arena config.
  OrtArenaCfg* default_memory_arena_cfg{nullptr};
  CUDAExecutionProviderExternalAllocatorInfo external_allocator_info{};
  // By default use fix workspace size (32M) for Conv algo search, the final algo might not be the best.
  // If set to true, try to use as much as possible memory for algo search.
  bool cudnn_conv_use_max_workspace{false};

  static CUDAExecutionProviderInfo FromProviderOptions(const ProviderOptions& options);
  static ProviderOptions ToProviderOptions(const CUDAExecutionProviderInfo& info);
};
}  // namespace onnxruntime
