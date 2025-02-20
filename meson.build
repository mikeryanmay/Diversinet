# basic project info
project('diversinet', 'cpp',
  version : '0.0.1',
  default_options : ['cpp_std=c++17', 'optimization=3'],
  license : 'GPL3')

# dependencies (different for library and programs)
eigen_dep          = dependency('eigen3')
boost_dep_for_lib  = dependency('boost', modules : ['random'])
boost_dep_for_calc = dependency('boost', modules : ['filesystem', 'program_options'])

# locations of header files
inc_for_lib  = include_directories('api', 'src')
inc_for_calc = include_directories('api')

# which subdirectories to search for meson.build files
subdir('api')
subdir('src')
subdir('sim')
subdir('calc')
subdir('test')

# the package
pkg_mod = import('pkgconfig')
pkg_mod.generate(libraries : diversinet,
                 version : '0.0',
                 name : 'libdiversinet',
                 filebase : 'diversinet',
                 description : '')