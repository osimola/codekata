ld = find_program('ld')
clsourceobj = custom_target('renderer.o',
      output: 'renderer.o',
      input: 'renderer.cl',
      command: [ld, '-r', '--format=binary', '-o', '@OUTPUT@', '@INPUT@']
)

exe = executable('spherecast', ['main.cpp', 'spherecast.cpp', 'clplat.cpp', clsourceobj],
      dependencies: [sdl_dep], link_args: '-lOpenCL')
