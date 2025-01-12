cmake_minimum_required(VERSION 3.17)
project(EasyRender LANGUAGES CXX CUDA)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)
set(CMAKE_BINARY_DIR build)

set(ENV{http_proxy} "http://127.0.0.1:7890")
set(ENV{https_proxy} "http://127.0.0.1:7890")

find_package(CUDAToolkit)
link_libraries(CUDA::cuda_driver)
link_libraries(CUDA::cudart)

set(CMAKE_PREFIX_PATH "${LibPaths}")

include(FetchContent)
include(ExternalProject)

set(SPDLOG_USE_STD_FORMAT ON)
if(MSVC)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/wd4996>)
endif()

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY    https://github.com/gabime/spdlog.git
  GIT_TAG           v1.13.0
)

FetchContent_Declare(
  glm
  GIT_REPOSITORY    https://github.com/g-truc/glm.git
  GIT_TAG           1.0.1
)

FetchContent_MakeAvailable(spdlog glm)
include_directories(SYSTEM
    "${CUDAToolkit_INCLUDE_DIRS}"
    "$ENV{OptiX_ROOT}/include")

add_definitions(-DSPDLOG_USE_STD_FORMAT)

# build glfw
set(OpenGL_GL_PREFERENCE LEGACY)
if (WIN32)
#  set(glfw_dir ${PROJECT_SOURCE_DIR}/submodules/glfw/)
  set(glfw_dir ${PROJECT_SOURCE_DIR}/3rdParty/glfw/)
  include_directories(${glfw_dir}/include)
  add_subdirectory(${glfw_dir})
else()
  find_package(glfw3 REQUIRED)
endif()


# build minipbrt
set(minipbrt_dir ${PROJECT_SOURCE_DIR}/3rdParty/minipbrt/)
include_directories(${minipbrt_dir})
add_subdirectory(${minipbrt_dir})

include_directories(${PROJECT_SOURCE_DIR}/3rdParty/stb_image/)

# Enable UTF-8 and warnings.
if(MSVC)
    set(UTFFlags /utf-8 /W3)
    add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:${UTFFlags}>")
    foreach(flag IN LISTS UTFFlags)
        add_compile_options("$<$<COMPILE_LANGUAGE:CUDA>:-Xcompiler=${flag}>")
    endforeach()
else()
    set(UTFFlags -fexec-charset=UTF-8 -finput-charset=UTF-8 -Wall)
    add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:${UTFFlags}>")
    foreach(flag IN LISTS UTFFlags)
        add_compile_options("$<$<COMPILE_LANGUAGE:CUDA>:--compiler-options=${flag}>")
    endforeach()
endif()

option(DEBUG OFF)
if(DEBUG)
    add_definitions(-DERROR_DEBUG -DNEED_VALIDATION_MODE -DNEED_IN_RANGE_CHECK 
                    -DNEED_VALID_DEVICE_POINTER_CHECK -DNEED_SAFE_INT_CHECK)
    message("Enabling debug mode...")
endif()

option(RELEASE OFF)
if(RELEASE)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE) # enable LTO
endif()

add_subdirectory(src)

option(NEED_BACKEND_EXAMPLE OFF)
if (NEED_BACKEND_EXAMPLE)
    add_subdirectory(examples)
endif()
