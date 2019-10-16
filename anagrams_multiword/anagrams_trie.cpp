#include <iostream>

#include "format.hpp"
#include "trie.hpp"

int main(const int argc, const char* argv[]) {
    if (argc < 2)
        return 1;

    trie::Node root = trie::constructSortedPrefixes(argv[1]);

    trie::traverse(&root, [](auto& node) {
        if (node.values.size() > 1) {
            std::cout << node.values << std::endl;
        }
    });
}
