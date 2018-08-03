project(
  'Quiver',
  'cpp',
  version : '0.9',
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

quiver_flags = []
cpp = meson.get_compiler('cpp')

# SSE3
if get_option('sse3')
  if cpp.has_argument('-msse3')
    quiver_flags += '-msse3'
  endif
endif

foreach cflag: [
  '-Wformat=1']
    if cpp.has_argument(cflag)
      quiver_flags += cflag
    endif
endforeach

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
