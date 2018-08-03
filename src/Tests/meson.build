quiver_test_cpp_sources = files([
  'ParameterSettings.cpp',
  'TestCoverage.cpp',
  'TestDiploidQuiver.cpp',
  'TestMatrixFacades.cpp',
  'TestMultiReadMutationScorer.cpp',
  'TestMutationEnumerator.cpp',
  'TestMutationScorer.cpp',
  'TestMutations.cpp',
  'TestPairwiseAlignment.cpp',
  'TestPoaConsensus.cpp',
  'TestQvEvaluator.cpp',
  'TestRecursors.cpp',
  'TestSparseVector.cpp'])

# find GoogleTest and GoogleMock
quiver_gtest_dep = dependency('gtest_main', fallback : ['gtest', 'gtest_dep'])
quiver_gmock_dep = dependency('gmock_main', fallback : ['gtest', 'gmock_main_dep'])

quiver_test = executable(
  'quiver_test',
  quiver_test_cpp_sources,
  dependencies : [
    quiver_gtest_dep,
    quiver_gmock_dep],
  include_directories : [
    quiver_include_directories],
  link_with : quiver_cc1_lib,
  cpp_args : quiver_flags,
  install : false)

#########
# tests #
#########

test(
  'quiver formatting check',
  quiver_clang_formatter,
  args : [
    '--all'],
  workdir : meson.source_root())

test(
  'quiver gtest unittests',
  quiver_test,
  args : [
    '--gtest_output=xml:' + join_paths(meson.build_root(), 'quiver-gtest-unittests.xml')],
  env : [
    'ARGS=-V',
    'VERBOSE=1'],
  timeout : 1800)
