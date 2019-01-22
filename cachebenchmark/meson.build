project('cachebenchmarks', 'cpp')
add_global_arguments('-std=c++11', language : 'cpp')
gbench_sp = subproject('google-benchmark')
timing_exe = executable('cache_timing', ['timing.cpp', 'countutils.cpp'],
           dependencies: gbench_sp.get_variable('google_benchmark_main_dep'))
