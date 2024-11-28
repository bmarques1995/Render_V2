# CMakeList.txt: projeto de CMake para Render_V2, incluir origem e definir
# lógica específica a um projeto aqui.
#

# Adicione a origem ao executável deste projeto.
add_library (Render STATIC "src/Render.cpp" "include/Render.hpp")
target_include_directories(Render PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Render PROPERTY CXX_STANDARD 20)
endif()

# TODO: Adicione testes e instale destinos, se necessário.
