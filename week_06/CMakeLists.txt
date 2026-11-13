cmake_minimum_required(VERSION 3.14) # Минимальная требуемая версия CMake
project(06_week) # Необходим для инициализации cmake

set(CMAKE_CXX_STANDARD 20) # Версия стандарта C++
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Гарантирует использование указанной версии стандарта

set(EXAMPLES_DIR examples) # Определим переменную с именем директории
set(TASKS_DIR tasks)

add_subdirectory(tasks)

# Создать исполняемый файл для каждого примера
if (BUILD_EXAMPLES_06_WEEK)
    add_example(unique_ptr_base ${EXAMPLES_DIR}/unique_ptr_base.cpp)
    add_example(smart_ptr ${EXAMPLES_DIR}/smart_ptr.cpp)
endif()