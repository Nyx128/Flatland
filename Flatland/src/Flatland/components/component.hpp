#pragma once
#include <iostream>
#include <bitset>

using component_ID = std::uint8_t;
const component_ID MAX_COMPONENTS = 32;

using bitSignature = std::bitset<MAX_COMPONENTS>;
