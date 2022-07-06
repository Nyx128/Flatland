#include "FLUtil.hpp"
#include <fstream>
#include "../core/asserts.hpp"

std::vector<char> FLUtil::readFile(const char* filepath){
    std::ifstream file(filepath, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        FL_ASSERT_MSG("Failed to read file:( %s )", filepath);
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}
