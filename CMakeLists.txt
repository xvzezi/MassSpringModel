cmake_minimum_required(VERSION 3.13)
set(PROJECT_NAME MassSpringModel)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 14)

set(GLFW_INC_DIR ../global/include)
set(GLFW_LIB_DIR ../global/lib)

include_directories(${GLFW_INC_DIR})
link_directories(${GLFW_LIB_DIR})
link_libraries(glfw3)

set(QX_MODULE qx/stb_image.h qx/stb_image.cpp qx/qxobject.h
        qx/qxshader.h qx/qxtexture.h qx/qxworld.h qx/qxheader.h qx/qxcamera.h)

set(IMGUI imgui/imgui.h imgui/imgui.cpp imgui/imgui_demo.cpp imgui/imgui_draw.cpp
        imgui/imgui_impl_glfw.h imgui/imgui_impl_glfw.cpp
        imgui/imgui_impl_opengl3.h imgui/imgui_impl_opengl3.cpp
        imgui/imgui_internal.h imgui/imgui_widgets.cpp
        imgui/imstb_rectpack.h imgui/imstb_textedit.h imgui/imstb_truetype.h)

set(PROJ main.cpp glad.c MassSpringSolver.h MassSpringObject.h)


add_executable(${PROJECT_NAME} ${PROJ} ${QX_MODULE} ${IMGUI})
target_link_libraries(${PROJECT_NAME} glfw3)