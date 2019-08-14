#pragma once
#define SDK_VERSION_10_0_9638 0
#define CURRENT_SDK_VERSION SDK_VERSION_10_0_9638
#define VERSION_EQUALS(v) CURRENT_SDK_VERSION == v

#if VERSION_EQUALS(SDK_VERSION_10_0_9638)
#include "10.0.9638/solidsdk/SolidFramework.h"
#include "10.0.9638/glue/glue.h"
#endif

