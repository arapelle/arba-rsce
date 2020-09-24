add_cpp_library_tests(SHARED ${PROJECT_NAME}
                      STATIC ${PROJECT_NAME}-static
                      SOURCES
                        resource_store_tests.cpp
                        basic_resource_manager_tests.cpp
                        basic_resource_manager_tests_2.cpp
                        resource_manager_tests.cpp
                        resource_manager_tests_2.cpp
                      )
