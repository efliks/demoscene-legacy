# Follow these steps to build flagtro2 with DJGPP on Linux. 
# Replace the DJGPP directory with your own.

# flatgrd2 > DJGPP_DIR=/home/mikolaj/local/opt/djgpp-12.1.0
# flatgrd2 > source ${DJGPP_DIR}/setenv
# flatgrd2 > mkdir build
# flatgrd2 > cd build

# flatgrd2/build > cmake -D CMAKE_C_COMPILER=${DJGPP_DIR}/i586-pc-msdosdjgpp/bin/gcc ..
# flatgrd2/build > make

# To run the program, you need CWSDPMI.EXE in the build directory, 
# and one of the *.3d models.

cmake_minimum_required(VERSION 3.13)

project(flatgrd2.exe)

set(GCC_COVERAGE_COMPILE_FLAGS "-W -Wall -pedantic -O2")
add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})

add_executable(flatgrd2.exe 3dmath.c 3dobj.c bumpmap.c dump_scr.c envmap.c font.c graph.c main.c phongpal.c triangle.c)
target_link_libraries(flatgrd2.exe PUBLIC m)
