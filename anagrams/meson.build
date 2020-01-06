project('anagrams', 'c', default_options : ['c_std=c99'])

glib_dep = dependency('glib-2.0')

anagrams_sisterlink = executable('anagrams_sisterlink',
        'anagrams_sisterlink.c',
        'stringsort.c',
        'sisterlinkedtree.c',
        'memprint.c',
    dependencies: glib_dep)

anagrams_hash = executable('anagrams_hash',
        'anagrams_hash.c',
        'stringsort.c',
        'memprint.c',
    dependencies: glib_dep)