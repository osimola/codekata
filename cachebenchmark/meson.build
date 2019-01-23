project('cachebenchmarks', 'cpp')
add_global_arguments('-std=c++17', language : 'cpp')
gbench_sp = subproject('google-benchmark')
ctiming_exe = executable('cache_timing', ['cache_timing.cpp'],
           dependencies: gbench_sp.get_variable('google_benchmark_dep'))

btiming_exe = executable('branch_timing', ['branch_timing.cpp', 'countutils.cpp'],
           dependencies: gbench_sp.get_variable('google_benchmark_dep'))
