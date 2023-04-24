sim_sources = ['main.cpp']

sim = executable('simDiversinet',
                   sim_sources,
                   include_directories : inc_for_calc,
                   dependencies : [eigen_dep, boost_dep_for_calc],
                   link_with : diversinet,
                   install : false)