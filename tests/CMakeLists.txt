enable_testing()

file(GLOB UNIT_TESTS "varint_tests.cpp" "variant_tests.cpp" "outcome_tests.cpp" )

### disable catch2 from installing it's own signal handlers
add_definitions(-DCATCH_CONFIG_NO_POSIX_SIGNALS)
file(GLOB SPEC_TESTS "spec_tests/address_tests.cpp" )

add_subdirectory(wasms)
add_executable( eos_vm_unit_tests main.cpp ${UNIT_TESTS} ${SPEC_TESTS} )
target_link_libraries( eos_vm_unit_tests eos-vm Catch2::Catch2 )

target_include_directories( eos_vm_unit_tests PUBLIC ${CMAKE_SOURCE_DIR}/include )

catch_discover_tests( eos_vm_unit_tests )
