# basic project info
project('diversinet', 'cpp',
  version : '0.1.0',
  default_options : ['cpp_std=c++20', 'optimization=3'],
  license : 'GPL-3.0-or-later')

# dependencies (different for library and programs)
eigen_dep          = dependency('eigen3')
boost_dep_for_lib  = dependency('boost', modules : ['random'])

if get_option('programs') or get_option('tests')
  boost_dep_for_calc = dependency('boost', modules : ['filesystem', 'program_options'])
endif

# locations of header files
inc_for_lib  = include_directories('include', 'src')
inc_for_calc = include_directories('include')

# which subdirectories to search for meson.build files
subdir('include')
subdir('src')

if get_option('programs')
  subdir('sim')
  subdir('calc')
endif

if get_option('tests')
  subdir('test')
endif

# the package
pkg_mod = import('pkgconfig')
pkg_mod.generate(libraries : diversinet,
                 version : meson.project_version(),
                 name : 'libdiversinet',
                 filebase : 'diversinet',
                 description : 'Likelihood calculation and simulation for phylogenetic networks')

install_data(
  'LICENSE',
  install_dir : join_paths(get_option('datadir'), 'licenses', 'Diversinet'),
)
