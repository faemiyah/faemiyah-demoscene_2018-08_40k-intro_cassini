layout(location=0) uniform vec3[5] uniform_array;
layout(location=6) uniform vec4 glyph_rectangle;

in vec2 vertex;

out vec2 glyph_texcoord;

out gl_PerVertex
{
  vec4 gl_Position;
};

/// Calculates Rectangularized coordinates on screen.
/// \param screen_coords Normalized screen coordinates.
/// \param screen_params Screen width, height and FoV.
/// \return Aspect ratio -mapped screen coordinates.
vec2 screen_rect_coord(vec2 screen_coords, vec3 screen_params)
{
  float ratio = screen_params.x / screen_params.y;
  if(ratio > 1.0)
  {
    return screen_coords / vec2(ratio, 1.0);
  }
  return screen_coords / vec2(1.0, ratio);
}

void main()
{
  glyph_texcoord = vertex;
  vec2 aspect = screen_rect_coord(glyph_rectangle.xy + vertex * glyph_rectangle.zw, uniform_array[3]);
  gl_Position=vec4(aspect, 0.0, 1.0);
}
