import collections
import numpy as np

class Dict:
    def __init__(self, fname = '/usr/share/dict/words'):
        self.root = _mknode()
        with open(fname, 'r') as f:
            for word in [x.strip() for x in f]:
                self.insert(word)

    def insert(self, word):
        _insert(self.root, word)

    def contains(self, word):
        return len(self.find(word)) > 0

    def neighbors(self, word):
        return self.find(word, 1)

    def find(self, word, tolerance = 0):
        return _find(self.root, word, tolerance)
    
def editdist(w1, w2):
    costs = np.zeros((len(w1), len(w2)), np.int)
    if (w1[0] != w2[0]):
        costs[0, 0] = 1
    for row in range(1, len(w1)):
        costs[row, 0] = row + costs[0, 0]
    for col in range(1, len(w2)):
        costs[0, col] = costs[0, col - 1] + 1
        for row in range(1, len(w1)):
            if w1[row] == w2[col]:
                costs[row, col] = min(costs[row - 1, col - 1],
                                      costs[row - 1, col] + 1,
                                      costs[row, col - 1] + 1)
            else:
                costs[row, col] = min(costs[row - 1, col - 1] + 1,
                                      costs[row - 1, col] + 1,
                                      costs[row, col - 1] + 1)
#    print(costs)
    return costs[len(w1) - 1, len(w2) - 1]

def _mknode():
    return collections.defaultdict(_mknode)

def _insert(node, word):
    # None is the end tag
    if (len(word) == 0):
        node[None] = None
    else:
        _insert(node[word[0]], word[1:])

def _find(node, suffix, tolerance):
    result = set()

    if len(suffix) == 0:
        # Usually tolerance >= 0 would make sense, but here we want only
        # the entries at specified edit distance
        if tolerance == 0:
            if None in node:
                return ['']
            else:
                return [None]
        else:
            # Added character
            for k in node.keys():
                if k is not None:
                    result.update(k + subresult
                                  for subresult in _find(node[k], '', tolerance - 1)
                                  if subresult is not None)

            return result
            

    # Exact match
    if suffix[0] in node:
        result.update(suffix[0] + subresult
                      for subresult in _find(node[suffix[0]], suffix[1:], tolerance)
                      if subresult is not None)

    if tolerance > 0:
        # Deleted character(s)
        if len(suffix) > 1 and tolerance > 0 and suffix[1] in node:
            result.update([suffix[1] + subresult
                           for subresult in _find(node[suffix[1]], suffix[2:], tolerance - 1)
                           if subresult is not None])
        if tolerance == len(suffix) and None in node:
            result.update([''])

        for k in node.keys():
            if k is not None:
                # Inserted character
                result.update(k + subresult
                              for subresult in _find(node[k], suffix, tolerance - 1)
                              if subresult is not None)
                # Swapped character
                if (k != suffix[0]):
                    result.update(k + subresult
                                  for subresult in _find(node[k], suffix[1:], tolerance - 1)
                                  if subresult is not None)
    return result

