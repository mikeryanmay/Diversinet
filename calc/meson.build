calc_sources = ['main.cpp']

calc = executable('calcDiversinet',
                   calc_sources,
                   include_directories : inc_for_calc,
                   dependencies : [eigen_dep, boost_dep_for_calc],
                   link_with : diversinet,
                   install : false)