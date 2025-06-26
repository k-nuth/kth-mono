
if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()

    remove_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
    remove_definitions(-D_GLIBCXX_USE_CXX11_ABI=1)

    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
        if (NOT NOT_USE_CPP11_ABI)
            add_definitions(-D_GLIBCXX_USE_CXX11_ABI=1)
            message( STATUS "Knuth: Using _GLIBCXX_USE_CXX11_ABI=1")
        else()
            add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
            message( STATUS "Knuth: Using _GLIBCXX_USE_CXX11_ABI=0")
        endif()
    endif()
else()
    message(WARNING "The file conanbuildinfo.cmake doesn't exist, you have to run conan install first")
endif()

# message( STATUS "CONAN_CXX_FLAGS: " ${CONAN_CXX_FLAGS} )
# message( STATUS "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS} )

