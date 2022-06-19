#pragma once
#include "FLInstance.hpp"
#include "../core/logger.hpp"

class FLDevice {
public:
	FLDevice() { FL_TRACE("FLDevice created"); };
	~FLDevice(){};

	FLDevice(const FLDevice&) = delete;
	FLDevice& operator=(const FLDevice&) = delete;

private:
	FLInstance instance;
};
