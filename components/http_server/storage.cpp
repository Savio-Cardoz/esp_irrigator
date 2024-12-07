#include "storage.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <array>

std::string getFileContents(std::string filename)
{
    const char *funcName = "getFileContents";

    if (std::ifstream inStream{"/spiffs/" + filename, std::ifstream::ate | std::ios::binary})
    {
        // auto fileSize = inStream.tellg();

        // std::array output<std::byte, fileSize>;

        // std::string contents(fileSize, '\0');
        // inStream.seekg(0);

        // inStream.read(output.data(), fileSize);
        // return output;
    }
    else
    {
        ESP_LOGE(funcName, "Failed to open %s", filename.c_str());
    }
    // return std::array;
    return "";
}