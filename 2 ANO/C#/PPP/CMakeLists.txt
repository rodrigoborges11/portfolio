cmake_minimum_required(VERSION 3.10)

project(projeto C)

add_compile_options(-Wall -Wextra)

set(SOURCES 
    projeto_funcao.c 
    projeto.c
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(projeto ${SOURCES})

set(DATA_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/doentes.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/registos.txt
)

foreach(DATA_FILE ${DATA_FILES})
    get_filename_component(FILE_NAME ${DATA_FILE} NAME_WE)
    add_compile_definitions(DATA_FILE_PATH_${FILE_NAME}="${DATA_FILE}")
endforeach()