
# set the name of the sample
set(SAMPLE_NAME "rukh-test")

# avoid listing all the files
file(GLOB_RECURSE srcs ./*.cpp)

add_executable(${SAMPLE_NAME} ${srcs})

target_link_libraries(${SAMPLE_NAME} ntools-0.1.1)
