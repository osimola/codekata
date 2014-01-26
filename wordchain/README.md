Word chain
==========

Looks for a route from a word to another by changing, adding or
removing a single letter a time so that every change results in a word
contained in the dictionary.

The original exercise was with changing letters only, but that didn't
result in very interesting chains.

The search trie is self-implemented because why not, and also because
this is not done on company time. There are ways to optimize it.

Example: 

```
python chain.py easy crowd
('easy', 'ease', 'case', 'cased', 'cawed', 'cowed', 'crowed', 'crowd')
```