project(
  'Quiver',
  'cpp',
  version : '1.1.0',
  default_options : [
    'buildtype=release',
    'warning_level=3',
    'cpp_std=c++11',
    'b_ndebug=if-release'],
  license : 'BSD-3',
  meson_version : '>= 0.46.0')

############
# CXXFLAGS #
############

cpp = meson.get_compiler('cpp')

# SSE3
quiver_perf_flags = []
if get_option('sse3')
  if cpp.has_argument('-msse3')
    quiver_perf_flags += '-msse3'
  endif
endif

quiver_warning_flags = []
foreach cflag: [
  '-Wduplicated-cond',
  '-Wduplicated-branches',
  '-Wlogical-op',
  '-Wrestrict',
  '-Wuseless-cast',
  '-Wdouble-promotion',
  '-Wshadow',
  '-Wold-style-cast',
  '-Wformat=1']
    if cpp.has_argument(cflag)
      quiver_warning_flags += cflag
    endif
endforeach

# Clang emits annoying
#
#   clang: warning: argument unused during compilation:
#     '-pthread' [-Wunused-command-line-argument]
#
# warnings on macOS, which adds no value and is taken care
# of by '-Wl,-dead_strip_dylibs' anyways.
if cpp.get_id() == 'clang'
  foreach cflag: [
    '-Wno-unused-command-line-argument']
      if cpp.has_argument(cflag)
        add_global_link_arguments(cflag, language : 'cpp')
      endif
  endforeach
endif

quiver_flags = [
  quiver_perf_flags,
  quiver_warning_flags]

################
# dependencies #
################

# boost
quiver_boost_dep = dependency('boost', required : true)

quiver_include_directories = []

############
# external #
############

subdir('external')

###########
# headers #
###########

subdir('include')

###################
# sources + tests #
###################

quiver_clang_formatter = find_program('tools/check-formatting')
subdir('src')
