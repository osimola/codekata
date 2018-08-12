Medians
=======

This one time, there was talk at work, whether median can be
calculated faster than sorting the entire array and taking element at
middle. This is how you do it:

- Pivot as if you were doing Quicksort

- Pick the half that contains the center of the original array. Unless
  your pivot happened to be at center, in that case it's your median.

- Repeat until done. Linear time, unless you hit pathological pivots.

- Looks like this is known as Quickselect but not taught at freshman
  courses.

Similar variant can be also done to radix sort. This avoids the bad
pivot problem, but requires sort key that can be represented as bits
and is slightly slower most of the time.

If you need to be fast for small arrays, do insertion sort as usual,
just don't bother to keep array in order past halfway. Still N^2 time
but with slightly smaller constant. For real use, the common wisdom is to
switch from quicksort to insertion sort at some point but here it
looks like quickselect is at least as fast all the way.

How fast is it compared to sorting entire array with std::sort and
taking the middle element? For large-ish arrays the results are all
over the map depending on what the CPU cache does. For above L3 cache
arrays the expected linear vs. N log N can be seen. For small arrays,
about below 40 elements (which naturally stay in L1) the difference is
at most 1.5x.

For sliding window, you could just keep a sorted copy of window
contents, find insertion and removal points in logarithmic time and
move data around in linear time. With even distribution of random
inputs, the extremes of the window seem to stay in place and the
entire operation go somewhere closer to logarithmic.

As somewhat interesting side note, in the benchmark insertion sort
seems to scale slightly subquadracally because while there are N^2
comparisons, most insertions succeed with smaller amount of
swaps. Bubblesort in turn shows very accurate N^2 scaling.

Built with Meson (http://mesonbuild.com), because it's really cool
these days and everyone should be using it.

For the record, the production code that inspired this uses full
insertion sort for sliding window and is not CPU bound so optimizing
this corner wouldn't really do anything.