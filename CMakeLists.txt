cmake_minimum_required(VERSION 3.1)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
project(cassini)

enable_language(C)
enable_language(CXX)

include("defaults.cmake")

find_boost("filesystem" "program_options" "system" "thread" "wave")
find_freetype()
find_glew()
find_opengl()
find_png()
find_sdl2()

output_flags("DEBUG" on)

add_definitions(-DUSE_LD)

include_directories("${PROJECT_SOURCE_DIR}/src")

add_executable(cassini
  "src/bsd_rand.c"
  "src/bsd_rand.h"
  "src/clouds.frag.glsl.hpp"
  "src/clouds_post.frag.glsl.hpp"
  "src/crater.hpp"
  "src/crater_map.hpp"
  "src/crawler_2d.hpp"
  "src/crawler.hpp"
  "src/crawler_map.hpp"
  "src/direction.hpp"
  "src/enceladus.frag.glsl.hpp"
  "src/fluidsimulation.hpp"
  "src/font.frag.glsl.hpp"
  "src/font.vert.glsl.hpp"
  "src/global_data.hpp"
  "src/global_data_temporary.hpp"
  "src/glsl_program.cpp"
  "src/glsl_program.hpp"
  "src/glsl_shader.cpp"
  "src/glsl_shader.hpp"
  "src/glsl_wave.cpp"
  "src/glsl_wave.hpp"
  "src/header.glsl.hpp"
  "src/huygens.frag.glsl.hpp"
  "src/huygens_post.frag.glsl.hpp"
  "src/huygens_post.vert.glsl.hpp"
  "src/huygens.vert.glsl.hpp"
  "src/image_png.cpp"
  "src/image_png.hpp"
  "src/intro.cpp"
  "src/precalc.frag.glsl.hpp"
  "src/precalc.vert.glsl.hpp"
  "src/simple.frag.glsl.hpp"
  "src/simple_post.frag.glsl.hpp"
  "src/space.frag.glsl.hpp"
  "src/space_post.frag.glsl.hpp"
  "src/space_post.vert.glsl.hpp"
  "src/space.vert.glsl.hpp"
  "src/star_location.hpp"
  "src/star_location_side.hpp"
  "src/star_location_tree.hpp"
  "src/verbatim_character.hpp"
  "src/verbatim_cond.hpp"
  "src/verbatim_font.hpp"
  "src/verbatim_frame_buffer.hpp"
  "src/verbatim_gl.hpp"
  "src/verbatim_image_2d_gray.hpp"
  "src/verbatim_image_2d.hpp"
  "src/verbatim_image_2d_la.hpp"
  "src/verbatim_image_2d_rgba.hpp"
  "src/verbatim_image_2d_rgb.hpp"
  "src/verbatim_image_3d_gray.hpp"
  "src/verbatim_image_3d.hpp"
  "src/verbatim_image_3d_rgb.hpp"
  "src/verbatim_image_cube_gray.hpp"
  "src/verbatim_image_cube.hpp"
  "src/verbatim_image_cube_la.hpp"
  "src/verbatim_image_cube_rgba.hpp"
  "src/verbatim_image_cube_rgb.hpp"
  "src/verbatim_image.hpp"
  "src/verbatim_mat2.hpp"
  "src/verbatim_mat3.hpp"
  "src/verbatim_mat4.hpp"
  "src/verbatim_mutex.hpp"
  "src/verbatim_opt.hpp"
  "src/verbatim_png.hpp"
  "src/verbatim_quat.hpp"
  "src/verbatim_realloc.hpp"
  "src/verbatim_scoped_lock.hpp"
  "src/verbatim_seq.hpp"
  "src/verbatim_spline.hpp"
  "src/verbatim_spline_point.hpp"
  "src/verbatim_texture_2d.hpp"
  "src/verbatim_texture_3d.hpp"
  "src/verbatim_texture_cube.hpp"
  "src/verbatim_texture_format.hpp"
  "src/verbatim_texture.hpp"
  "src/verbatim_thread.hpp"
  "src/verbatim_uarr.hpp"
  "src/verbatim_uptr.hpp"
  "src/verbatim_vec2.hpp"
  "src/verbatim_vec3.hpp")
if(MSVC)
  target_link_libraries(cassini "Imm32")
  target_link_libraries(cassini "Version")
  target_link_libraries(cassini "WinMM")
  target_link_libraries(cassini "${OPENGL_gl_LIBRARY}")
  target_link_libraries(cassini debug "${GLEW_LIBRARY_DEBUG}" optimized "${GLEW_LIBRARY}")
  target_link_libraries(cassini debug "${FREETYPE_LIBRARY_DEBUG}" optimized "${FREETYPE_LIBRARY}")
  target_link_libraries(cassini debug "${PNG_LIBRARY_DEBUG}" optimized "${PNG_LIBRARY}")
  target_link_libraries(cassini debug "${SDL2_LIBRARY_DEBUG}" optimized "${SDL2_LIBRARY}")
  target_link_libraries(cassini debug "${SNDFILE_LIBRARY_DEBUG}" optimized "${SNDFILE_LIBRARY}")
  target_link_libraries(cassini debug "${ZLIB_LIBRARY_DEBUG}" optimized "${ZLIB_LIBRARY}")
else()
  target_link_libraries(cassini "${BOOST_FILESYSTEM_LIBRARY}")
  target_link_libraries(cassini "${BOOST_PROGRAM_OPTIONS_LIBRARY}")
  target_link_libraries(cassini "${BOOST_SYSTEM_LIBRARY}")
  target_link_libraries(cassini "${BOOST_THREAD_LIBRARY}")
  target_link_libraries(cassini "${BOOST_WAVE_LIBRARY}")
  target_link_libraries(cassini "${GLEW_LIBRARY}")
  target_link_libraries(cassini "${FREETYPE_LIBRARY}")
  target_link_libraries(cassini "${PNG_LIBRARY}")
  target_link_libraries(cassini "${SDL2_LIBRARY}")
  target_link_libraries(cassini "${SNDFILE_LIBRARY}")
endif()
