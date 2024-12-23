cmake_minimum_required(VERSION 3.31.2)
project(tetris VERSION 0.1.0 LANGUAGES C CXX)

set(SDL2_DIR "C:/Users/washy/scoop/apps/sdl2/current/cmake")
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

set(SDL2_TTF_DIR "C:/Users/washy/scoop/apps/sdl2_ttf/2.22.0/cmake")
find_package(SDL2_ttf REQUIRED CONFIG PATHS ${SDL2_TTF_DIR})
include_directories("C:/Users/washy/scoop/apps/sdl2_ttf/2.22.0/include")

# Manually set SDL2_TTF_LIBRARIES if not set by find_package
if(NOT SDL2_TTF_LIBRARIES)
    set(SDL2_TTF_LIBRARIES "C:/Users/washy/scoop/apps/sdl2_ttf/2.22.0/lib/x64/SDL2_ttf.lib")
endif()

add_executable(tetris tetris.cpp)

target_link_libraries(tetris ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES})

add_custom_command(TARGET tetris POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${SDL2_DIR}/../lib/SDL2.DLL"
    "C:/Users/washy/scoop/apps/sdl2_ttf/2.22.0/lib/x64/SDL2_ttf.dll"
    $<TARGET_FILE_DIR:tetris>)

add_custom_command(TARGET tetris POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/fonts/ARIAL.TTF"
    $<TARGET_FILE_DIR:tetris>/fonts/ARIAL.TTF
)

include(CTest)
enable_testing()
