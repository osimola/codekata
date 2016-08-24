project('quicksudoku', 'c')
add_global_arguments('-std=c99', language : 'c')

glib_dep = dependency('glib-2.0')
qsudoku = executable('qsudoku', 'qsudoku.c', dependencies: glib_dep)