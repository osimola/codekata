from dictionary import *
from heapq import *
import sys

def main(src, target):
    if src == target:
        print("Elementary!")
        return
    
    dic = Dict()

    if src not in dic:
        print("Not in dict: " + src)
        return
    if target not in dic:
        print("Not in dict: " + target)
        return
    
    visited = set(src)
    heap = []
    heappush(heap, (editdist(src, target), (src,)))
    visited.add(src)

    while len(heap) > 0:
        dist, path = heappop(heap)
        nbors = dic.neighbors(path[-1]).difference(visited)
        if (target in nbors):
            print(path + (target, ))
            return
        for nbor in nbors:
            visited.add(nbor)
            # A* heuristic: at least editdist() steps needed to get to target
            heappush(heap, (editdist(nbor, target) + len(path), path + (nbor,)))

if __name__ == '__main__':
    if len(sys.argv) == 3:
        main(sys.argv[1], sys.argv[2])
