project(
  'diversinet-installed-consumer',
  'cpp',
  default_options : ['cpp_std=c++20'],
)

diversinet_dep = dependency('diversinet')
boost_dep = dependency('boost')

executable(
  'diversinet-installed-consumer',
  'main.cpp',
  dependencies : [diversinet_dep, boost_dep],
)
