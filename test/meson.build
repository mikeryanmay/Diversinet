test_sources = ['test.cpp',
    '../src/Test/Catch2/catch.hpp'
]

test = executable('testDiversinet',
                   test_sources,
                   include_directories : inc_for_lib,
                   dependencies : boost_dep_for_calc,
                   link_with : diversinet,
                   install : false)

test('diversinet_test', test, args : [])