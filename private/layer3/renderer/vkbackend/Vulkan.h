#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

static constexpr uint32_t FRAMES_IN_FLIGHT = 2;

#define CHECK_VK(x) \
    if ((x) != VK_SUCCESS) do { printf("ERROR: " #x " failed!\n"); exit(1); } while (0)