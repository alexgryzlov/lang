execute_process(COMMAND python generate_code.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

add_executable(execute processor.cpp text_proc.cpp trie.cpp)
add_executable(compile compiler.cpp text_proc.cpp)
