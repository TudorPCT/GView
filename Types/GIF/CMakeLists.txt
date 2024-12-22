# Find giflib
find_library(GIF_LIBRARY gif PATHS /usr/local/lib)
find_path(GIF_INCLUDE_DIR gif_lib.h PATHS /usr/local/include)

# Check if giflib was found
if(GIF_LIBRARY AND GIF_INCLUDE_DIR)
    message(STATUS "Found giflib library: ${GIF_LIBRARY}")
    message(STATUS "Found giflib include directory: ${GIF_INCLUDE_DIR}")
else()
    message(FATAL_ERROR "giflib not found. Please ensure it is installed.")
endif()

# Create the GIF target
include(type)
create_type(GIF)

# Add the include directory and link giflib to the GIF target
target_include_directories(GIF PRIVATE ${GIF_INCLUDE_DIR})
target_link_libraries(GIF PRIVATE ${GIF_LIBRARY})
