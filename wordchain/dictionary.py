import collections
import numpy as np

class Dict:
    def __init__(self, fname = '/usr/share/dict/words'):
        self.root = _mknode()
        with open(fname, 'r') as f:
            for word in [x.strip() for x in f]:
                self.add(word)

    def add(self, word):
        _add(self.root, word)

    def __contains__(self, word):
        return len(self.find(word)) > 0

    def neighbors(self, word):
        return self.find(word, 1)

    def find(self, word, tolerance = 0):
        # Experimental value: Pruning duplicate edit paths starts really paying
        # its overhead at edit distance 4
        if tolerance < 4:
            return _find(self.root, word, tolerance)
        else:
            return _find(self.root, word, tolerance, collections.defaultdict(list))
    
def editdist(w1, w2):
    costs = np.zeros((len(w1) + 1, len(w2) + 1), np.int)
    costs[:, 0] = range(len(w1) + 1)
    costs[0, :] = range(len(w2) + 1)
        
    for col in range(1, len(w2) + 1):
        costs[0, col] = costs[0, col - 1] + 1
        for row in range(1, len(w1) + 1):
            if w1[row-1] == w2[col-1]:
                costs[row, col] = min(costs[row - 1, col - 1],
                                      costs[row - 1, col] + 1,
                                      costs[row, col - 1] + 1)
            else:
                costs[row, col] = min(costs[row - 1, col - 1] + 1,
                                      costs[row - 1, col] + 1,
                                      costs[row, col - 1] + 1)
    return costs[len(w1), len(w2)]

def _mknode():
    return collections.defaultdict(_mknode)

def _add(node, word):
    # None is the end tag
    if (len(word) == 0):
        node[None] = None
    else:
        _add(node[word[0]], word[1:])

def _find(node, suffix, tolerance, visited = None):
    if visited is not None:
        key = id(node) << 8 + len(suffix)
        if tolerance in visited[key]:
            return []
        else:
            visited[key].append(tolerance)
    
    result = set()

    if len(suffix) == 0:
        if None in node:
            result.update([''])
            # Added character
        if tolerance > 0:
            for k in node.keys():
                if k is not None:
                    result.update(k + subresult
                                  for subresult in _find(node[k], '', tolerance - 1, visited))

        return result
        

    # Exact match
    if suffix[0] in node:
        result.update(suffix[0] + subresult
                      for subresult in _find(node[suffix[0]], suffix[1:], tolerance, visited))

    if tolerance > 0:
        # Deleted character(s)
        if len(suffix) > 1 and tolerance > 0 and suffix[1] in node:
            result.update(suffix[1] + subresult
                           for subresult in _find(node[suffix[1]], suffix[2:], tolerance - 1, visited))
        if tolerance == len(suffix) and None in node:
            result.update([''])

        for k in node.keys():
            if k is not None:
                # Inserted character
                result.update(k + subresult
                              for subresult in _find(node[k], suffix, tolerance - 1, visited))
                # Swapped character
                if (k != suffix[0]):
                    result.update(k + subresult
                                  for subresult in _find(node[k], suffix[1:], tolerance - 1, visited))
    return result

