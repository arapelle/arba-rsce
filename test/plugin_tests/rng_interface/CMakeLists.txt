add_library(${project_base_name}_rng_interface INTERFACE)
target_sources(${project_base_name}_rng_interface PUBLIC FILE_SET HEADERS FILES include/rng_interface.hpp BASE_DIRS include)
