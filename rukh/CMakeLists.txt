##
## CONFIGURATION FOR rukh
##

# Add your sources here:
set(source_list "rukh.cpp")

add_executable("${PROJECT_NAME}-target" "${source_list}")
sf_init_target("${PROJECT_NAME}-target")

# TODO: Fix scaffold
# sf_add_filter_group("${source_list}" "${CMAKE_CURRENT_SOURCE_DIR}")
