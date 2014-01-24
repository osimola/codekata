Word chain
==========

Looks for a route from a word to another by changing, adding or
removing a single letter a time so that every change results in a word
contained in the dictionary.

The original exercise was with changing letters only, but that didn't
result in very interesting chains.

There are better ways to implement the string search. It is possible
that one day I will implement one of those.

Example: 

```
python chain.py steely sleety
('steely', 'steel', 'steep', 'sleep', 'sleepy', 'sleety')
```