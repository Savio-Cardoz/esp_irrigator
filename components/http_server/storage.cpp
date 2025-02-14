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
        auto fileSize = inStream.tellg();
        std::string contents(fileSize, '\0');
        inStream.seekg(0);
        inStream.read(&contents[0], fileSize);
        return contents;
    }
    else
    {
        ESP_LOGE(funcName, "Failed to open %s", filename.c_str());
    }
    return "";
}