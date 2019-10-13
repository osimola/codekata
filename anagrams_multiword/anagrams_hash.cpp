#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

int main(const int argc, const char* argv[]) {
    if (argc < 2)
        return 1;

    std::unordered_map<std::string, std::vector<std::string>> map;

    {
        std::ifstream input(argv[1]);
        std::string line;

        while (std::getline(input, line)) {
            std::string value = line;
            std::string sorted = line;
            std::sort(sorted.begin(), sorted.end());

            map[sorted].push_back(std::move(value));
        }
    }

    for (const auto& entry : map) {
        if (entry.second.size() > 1) {
            for (size_t i = 0; i < entry.second.size() - 1; ++i)
                std::cout << entry.second[i] << ", ";
            std::cout << entry.second[entry.second.size() - 1]
                      << std::endl;
        }
    }

    return 0;
}
