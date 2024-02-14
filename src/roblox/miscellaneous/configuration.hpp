#pragma once
#include <Windows.h>
#include <string>

#define objectiveExpose extern "C" __declspec(dllexport) 

#define EXPLOIT_KEYSYSTEM_NAME "Objective"
#define EXPLOIT_USE_KEYSYSTEM FALSE
#define EXPLOIT_USE_RVM FALSE /* STABILITY ISSUES */
#define EXPLOIT_DEBUG_MODE TRUE
#define EXPLOIT_NAME "Objective"
#define EXPLOIT_VERSION "1.0.0"
#define PIPE_NAME "Objective"
#define USE_PID_FOR_PIPE FALSE