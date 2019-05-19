include(ExternalProject)
find_package(Git REQUIRED)
if (NOT USE_EXISTING_SOFTFLOAT)
   include(softfloat.cmake)
endif()

if (NOT USE_EXISTING_OUTCOME)
   include(outcome.cmake)
endif()
