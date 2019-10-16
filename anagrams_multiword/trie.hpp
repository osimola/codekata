#pragma once

#include <functional>
#include <string>
#include <vector>

namespace trie {
struct Node {
    Node* sibling = nullptr;
    Node* firstChild = nullptr;
    std::vector<std::string> values;
    char key;

    Node(char key) : key(key) {}
};

void add(Node* node, const char* key, std::string&& value);
Node constructSortedPrefixes(std::string filename);

inline void traverse(const Node* node, std::function<void(const Node&)> func) {
    while (node) {
        func(*node);
        if (node->firstChild)
            traverse(node->firstChild, func);
        node = node->sibling;
    }
}

} // namespace trie
