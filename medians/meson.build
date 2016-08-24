project('medians', 'cpp')
gtest_dep = dependency('gtest')
test_exe = executable('unittest', ['tests.cpp'], dependencies: gtest_dep)
test('unittest', test_exe)