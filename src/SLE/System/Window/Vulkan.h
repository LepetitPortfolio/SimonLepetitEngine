#pragma once
#include "../DataConfig.h"

#include <vector>

using VkInstance  = struct VkInstance_T*;
using VulkanFunctionPointer = void (*)();

struct VkAllocationCallbacks;


bool IsAvailable(bool _RequireGraphics = true);

VulkanFunctionPointer GetFunction(const char* _Name);

const std::vector<const char*>& GetGraphicsRequiredInstanceExtensions();

