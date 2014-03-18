Bit counting
============

There are several ways of counting bits. The naive one, using cached
counts and various smart algorithms. The question that comes up every
now or then and that everyone with spare half an hour must answer
themselves is, which one is the fastest.

I expected that a cached array of 16-bit counts which fits nicely in
the CPU cache would be the fastest. Turned out that the age-old
algorithm from Hacker's Delight and various standard libraries was
still the faster one.
