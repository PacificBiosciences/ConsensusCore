quiver_python_mod = import('python')
quiver_python = quiver_python_mod.find_installation('python2.7', required : true)

########
# SWIG #
########

quiver_swig = find_program('swig', required : true)

quiver_swig_interface = files('ConsensusCore.i')
quiver_swig_interface_depends = files([
  'Edna.i',
  'Evaluator.i',
  'Exceptions.i',
  'Features.i',
  'Matrix.i',
  'Mutation.i',
  'PairwiseAlignment.i',
  'PoaConsensus.i',
  'QuiverConsensus.i',
  'Statistics.i',
  'Types.i',
  'Utils.i',
  'Version.i',
  'numpy.i'])

quiver_swig_gen = custom_target(
  'quiver_swig2cpp',
  output : [
    'ConsensusCore_wrap.cpp',
    'ConsensusCore.py'],
  input : quiver_swig_interface,
  depend_files : quiver_swig_interface_depends,
  command : [
    quiver_swig,
    '-Wextra',
    '-w309', # private inheritance from base '...'
    '-c++',
    '-python',
    '-builtin',
    '-module',
    'ConsensusCore',
    '-I' + join_paths([meson.source_root(), 'include']),
    '-o',
    '@OUTPUT0@',
    '-outdir',
    meson.current_build_dir(),
    '@INPUT@'],
  install : true,
  install_dir : [
    false,
    quiver_python.get_install_dir()])

##########
# python #
##########

# ConsensusCore requires access to numpy's C API:
#
#   #include <numpy/arrayobject.h>
#
# this is a bit hacky, as there is no official way to
# retrieve this information using Meson's python module
quiver_numpy_incdir = run_command(
  quiver_python, [
    '-c',
    'import sys; import numpy; sys.stdout.write(numpy.get_include())']).stdout()

quiver_cc1_pylib = quiver_python.extension_module(
  '_ConsensusCore',
  quiver_swig_gen[0],
  install : true,
  install_dir : quiver_python.get_install_dir(),
  dependencies : [
    quiver_boost_dep,
    quiver_python.dependency(required : true)],
  include_directories : [
    quiver_include_directories],
  link_with : quiver_cc1_lib,
  cpp_args : [
    quiver_perf_flags,
    # this is also hacky and won't work on Windows
    '-I' + quiver_numpy_incdir,
    # SWIG generates functions with unused arguments
    '-Wno-unused-parameter'])

# used for tests
quiver_pythonpath = meson.current_build_dir()
