Anagrams
========

This started off as the exercise of finding anagrams in a large-ish
list of words (/usr/share/dict/words on linux distributions is a good
example) and turned into seeing if it was possible to optimize a naive
tree structure into something that could be compared to hash map
implementation in terms of time and memory requirements.

It turned out that on a Core 2 Duo machine the hash map was clearly
faster way, but the improved branch prediction on Core i5 makes the
speeds comparable. Despite the tricks with block allocation, hash map
runs in slightly less memory.

Built with SCons, because I was kind of enthusiastic about it at the time.

Compiling and running example:

```
scons -j 4
time ./anagrams_sisterlink < /usr/share/dict/words
```