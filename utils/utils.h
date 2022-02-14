#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>

template<class T>
std::vector<T> parseFile(const std::string& filename, size_t expectedItems = 13000)
{
    std::vector<T> result;
    std::cout << "Start loading list from file: " << std::endl << filename << std::endl;
    auto tpBegin = std::chrono::system_clock::now();
    result.reserve(expectedItems);
    std::ifstream listFile(filename);
    if (listFile.is_open())
    {
        std::string line;
        // get length of file:
        listFile.seekg(0, listFile.end);
        std::streamoff fileSize = listFile.tellg();
        listFile.seekg(0, listFile.beg);
        while (std::getline(listFile, line))
        {
            result.emplace_back(line);
        }
        auto tpEnd = std::chrono::system_clock::now();
        auto microsecondsElapsed = std::chrono::duration_cast<std::chrono::microseconds>(tpEnd - tpBegin).count();

        double secondsElapsed = microsecondsElapsed / 1000000.0;

        std::cout << "List loaded in " << std::setprecision(3) << std::fixed << secondsElapsed << " s" << std::endl;
        std::cout << "List count: " << result.size() << std::endl;
        std::cout << "Speed: " << std::setprecision(3) << fileSize / secondsElapsed / 1024 / 1024 << " MB/s" << std::endl;

    }
    else
    {
        std::cout << "Error opening file" << std::endl;
    }
    return result;
}
