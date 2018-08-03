#####################
# sources + library #
#####################

subdir('C++')

#########
# tests #
#########

if not meson.is_subproject()
  if get_option('tests')
    subdir('Tests')
  endif
endif
