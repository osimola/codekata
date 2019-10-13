#include <iostream>

#include "trie.hpp"

int main(const int argc, const char* argv[]) {
    if (argc < 2)
        return 1;

    trie::Node root = trie::constructSortedPrefixes(argv[1]);

    trie::traverse(&root, [](auto& node) {
        if (node.values.size() > 1) {
            for (size_t i = 0; i < node.values.size() - 1; ++i)
                std::cout << node.values[i] << ", ";
            std::cout << node.values[node.values.size() - 1] << std::endl;
        }
    });
}
