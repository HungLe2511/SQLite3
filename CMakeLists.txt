cmake_minimum_required(VERSION 3.0)

# C++ std 11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXE_LINKER_FLAGS "-no-pie")

project(SQLitest)

# Sqlite
set(SQLITE_LIBS "-lsqlite3") 
#find_library(CURL REQUIRED)

#json
find_package(nlohmann_json 3.2.0 REQUIRED)
add_library(nlohmann_json INTERFACE IMPORTED)
set(JSON_LIB )


#Setup
file(GLOB SOURCES *.cpp )

add_executable(${PROJECT_NAME} ${SOURCES})

include_directories(${PROJECT_SOURCE_DIR}/)

target_link_libraries(
    ${PROJECT_NAME} 
    
    PRIVATE 
    
    nlohmann_json::nlohmann_json
    ${SQLITE_LIBS}
)

