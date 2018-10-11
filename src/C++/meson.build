###########
# sources #
###########

quiver_cc1_cpp_sources = files([
  'Checksum.cpp',
  'Coverage.cpp',
  'Feature.cpp',
  'Features.cpp',
  'Mutation.cpp',
  'Read.cpp',
  'Sequence.cpp',
  'Utils.cpp',
  'Version.cpp',

  # -------
  # Align
  # -------
  'Align/AffineAlignment.cpp',
  'Align/AlignConfig.cpp',
  'Align/LinearAlignment.cpp',
  'Align/PairwiseAlignment.cpp',

  # ------
  # Edna
  # ------
  'Edna/EdnaCounts.cpp',

  # ---------
  # Logging
  # ---------
  'Logging/Logging.cpp',

  # --------
  # Matrix
  # --------
  'Matrix/DenseMatrix.cpp',
  'Matrix/SparseMatrix.cpp',

  # -----
  # Poa
  # -----
  'Poa/PoaConsensus.cpp',
  'Poa/PoaGraph.cpp',
  'Poa/PoaGraphImpl.cpp',
  'Poa/PoaGraphTraversals.cpp',
  'Poa/RangeFinder.cpp',

  # --------
  # Quiver
  # --------
  'Quiver/Diploid.cpp',
  'Quiver/MultiReadMutationScorer.cpp',
  'Quiver/MutationEnumerator.cpp',
  'Quiver/MutationScorer.cpp',
  'Quiver/QuiverConfig.cpp',
  'Quiver/QuiverConsensus.cpp',
  'Quiver/ReadScorer.cpp',
  'Quiver/SimpleRecursor.cpp',
  'Quiver/SseRecursor.cpp',
  'Quiver/detail/RecursorBase.cpp',

  # ------------
  # Statistics
  # ------------
  'Statistics/Binomial.cpp'])

# install library if
# - either running as a proper project
# - or using shared libraries
quiver_lib_install = (not meson.is_subproject()) or (get_option('default_library') == 'shared')

quiver_cc1_lib = library(
  'quiver',
  quiver_cc1_cpp_sources,
  install : quiver_lib_install,
  # use boost SONAME practice:
  #   cause ld startup issues before
  #   you even have the chance of running
  #   into ABI issues.
  soversion : meson.project_version(),
  version : meson.project_version(),
  dependencies : [
    quiver_boost_dep],
  include_directories : [
    quiver_include_directories],
  cpp_args : quiver_flags)
