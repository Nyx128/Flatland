#pragma once
#include <iostream>
#include "components/component.hpp"
#include <bitset>

using FLEntity = std::uint32_t;

const FLEntity MAX_ENTITIES = 9999;
	
using Signature = std::bitset<MAX_COMPONENTS>;
