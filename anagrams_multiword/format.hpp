#include <ostream>
#include <vector>
#include <string>

inline std::ostream& operator<<(std::ostream& output,
                         const std::vector<std::string>& values) {
    for (size_t i = 0; i < values.size() - 1; ++i)
        output << values[i] << " ";
    return output << values[values.size() - 1];
}
