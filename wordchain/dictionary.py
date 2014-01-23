import collections
import numpy as np

class Dict:
    def __init__(self, fname = '/usr/share/dict/words'):
        self.dic = collections.defaultdict(list)
        with open(fname, 'r') as f:
            for word in [x.strip() for x in f]:
                self.dic[len(word)].append(word)

    def neighbors(self, word, excludes = None):
        if excludes is None:
            excludes = set()
        nbors = []
        for w in self.dic[len(word)]:
            if w not in excludes and swapneighbor(w, word):
                nbors.append(w)
        if len(word) > 1:
            for w in self.dic[len(word) - 1]:
                if w not in excludes and editneighbor(w, word):
                    nbors.append(w)
        for w in self.dic[len(word) + 1]:
            if w not in excludes and editneighbor(w, word):
                nbors.append(w)
        return nbors

    def contains(self, word):
        return word in self.dic[len(word)]

def swapdist(w1, w2):
    dist = 0
    for i in range(len(w1)):
        if w1[i] != w2[i]:
            dist += 1
    return dist

def swapneighbor(w, word):
    diffs = 0
    for i in range(len(word)):
        if w[i] != word[i]:
            diffs += 1
            if diffs > 1:
                break;
    return diffs == 1

    
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

def editneighbor(w1, w2):
    costs = np.zeros((len(w1), len(w2)), np.int)
    if w1[0] != w2[0]:
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
            if min(costs[:, col]) > 1:
                return False
    return costs[len(w1) - 1, len(w2) - 1] == 1
