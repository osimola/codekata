#include "trie.hpp"

#include <algorithm>
#include <fstream>

namespace trie {
void add(Node* node, const char* key, std::string&& value) {
    if (*key == node->key && *(key + 1) == '\0')
        node->values.push_back(std::move(value));
    else {
        if (node->key == *key) {
            if (!node->firstChild)
                node->firstChild = new Node(*(key + 1));
            return add(node->firstChild, key + 1, std::move(value));
        } else {
            if (!node->sibling)
                node->sibling = new Node(*key);
            return add(node->sibling, key, std::move(value));
        }
    }
}

Node constructSortedPrefixes(std::string filename) {
    std::ifstream input(filename);
    std::string line;

    trie::Node root{static_cast<char>(input.peek())};

    while (std::getline(input, line)) {
        std::string value = line;
        std::string sorted = line;
        std::sort(sorted.begin(), sorted.end());

        add(&root, sorted.c_str(), std::move(value));
    }

    return root;
}

} // namespace trie
