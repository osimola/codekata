#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "trie.hpp"

using CharCounts = std::unordered_map<char, uint8_t>;

struct ResultItem {
    const trie::Node& leaf;
    const ResultItem* const prev;

    bool contains(const trie::Node* node) const {
        return (&leaf == node) || (prev ? prev->contains(node) : false);
    }
};

void printResult(const ResultItem& resultRef) {
    auto result = &resultRef;
    while (result) {
        std::cout << "[";
        for (const auto& value: result->leaf.values)
            std::cout << value << " ";
        std::cout << "] ";
        result = result->prev;
    }
    std::cout << std::endl;
}

std::ostream& operator<<(std::ostream& output, const CharCounts& counts) {
    for (auto& c: counts) {
        output << "(" << c.first << ":" << (int)c.second << ")";
    }
    return output;
}

void search(const trie::Node& root, CharCounts& counts, const trie::Node* node,
            const ResultItem* prev) {
    while (node) {
        if (counts.count(node->key)) {
            if (--counts[node->key] == 0)
                counts.erase(node->key);
            if (counts.empty()) {
                if (!(node->values.empty()))
                    printResult({*node, prev});
            } else {
                if (node->firstChild != nullptr)
                    search(root, counts, node->firstChild, prev);
                if (!node->values.empty() && (!prev || !prev->contains(node))) {
                    ResultItem r = {*node, prev};
                    search(root, counts, &root, &r);
                }
            }
            ++counts[node->key];
        }

        node = node->sibling;
    }
}

void search(const trie::Node& root, CharCounts& counts) {
    return search(root, counts, &root, nullptr);
}

int main(const int argc, const char* argv[]) {
    if (argc < 2)
        return 1;

    trie::Node root = trie::constructSortedPrefixes(argv[1]);

    CharCounts counts;
    for (int i = 2; i < argc; ++i)
        for (auto c: std::string(argv[i]))
            ++counts[c];

    search(root, counts);

    return 0;
}
