#####################
# sources + library #
#####################

subdir('C++')

########
# SWIG #
########

if get_option('swig')
  subdir('SWIG')
endif

#########
# tests #
#########

if not meson.is_subproject()
  if get_option('tests')
    subdir('Tests')
  endif
endif
