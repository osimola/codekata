Medians
=======

This one time, there was talk at work, whether median can be
calculated faster than sorting the entire array and taking element at
middle. This is how you do it:

- Pivot as if you were doing Quicksort

- Pick the half that contains the center of the original array. Unless
  your pivot happened to be at center, in that case it's your median.

- Repeat until done. Linear time, unless you hit pathological pivots.

If you need to be fast for small arrays, do insertion sort as usual,
just don't bother to keep array in order past halfway. N^2 time but
with smaller constant. For real use, you'd switch from quicksort to
insertion at some point.

How fast is it compared to sorting entire array with std::sort and
taking the middle element? For large-ish arrays the results are all
over the map depending on what the CPU cache does. For above L3 cache
arrays the expected linear vs. N log N can be seen. For small arrays,
about below 40 elements (which naturally stay in L1) the difference is
at most 1.5x.

Built with Meson (http://mesonbuild.com), because it's really cool
these days and everyone should be using it.

Maybe up some day: Sliding windows, which are doable with faster
linear. Logarithmic, if you have space and time for balanced trees.

For the record, the production code that inspired this uses full
insertion sort for sliding window and is not CPU bound.