#pragma once

#include <iostream>
#include <string>
#include "HRTimer.h"
#include <cuda.h>
#include <cuda_d3d11_interop.h>
#include <cuda_runtime.h>

#include "ThreadsDefines.h"
#include <DirectXMath.h>

using namespace DirectX;

std::string threadMsgToString(UINT msg);
void sendThreadMsg(DWORD receiver, UINT msg, LPARAM lpParam);

DWORD WINAPI SlaveGPUThreadMain(LPVOID lpParam);