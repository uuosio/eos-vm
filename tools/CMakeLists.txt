# ##################################################################################################
# Create the eos-vm interpreter driver.
# ##################################################################################################
add_executable(eos-vm-interp ${CMAKE_CURRENT_SOURCE_DIR}/interp.cpp)
target_link_libraries(eos-vm-interp eos-vm)

add_executable(bench-interp ${CMAKE_CURRENT_SOURCE_DIR}/bench_interp.cpp)
target_link_libraries(bench-interp eos-vm)

add_executable(hello-driver ${CMAKE_CURRENT_SOURCE_DIR}/hello_driver.cpp)
target_link_libraries(hello-driver eos-vm)

if (APPLE)
    set(_symbols_list "${CMAKE_SOURCE_DIR}/libraries/eos-vm/tools/symbols.list")
    set(LINK_FLAGS "${LINK_FLAGS} -Wl,-exported_symbols_list,'${_symbols_list}'")
else()
    set(_version_script "${CMAKE_SOURCE_DIR}/libraries/eos-vm/tools/version.script")
    set(LINK_FLAGS "${LINK_FLAGS} -Wl,--version-script,\"${_version_script}\"")
endif (APPLE)

add_library(vm_eos SHARED ${CMAKE_CURRENT_SOURCE_DIR}/vm_eos.cpp)
target_link_libraries(vm_eos eos-vm eosiolib_native)

set_target_properties(vm_eos  PROPERTIES LINK_FLAGS "${LINK_FLAGS}")

target_include_directories( vm_eos
                              PRIVATE "${CMAKE_SOURCE_DIR}/contracts"
                              PRIVATE "${CMAKE_BINARY_DIR}/contracts"
                              PRIVATE ${CMAKE_SOURCE_DIR}/externals/magic_get/include
                          )
