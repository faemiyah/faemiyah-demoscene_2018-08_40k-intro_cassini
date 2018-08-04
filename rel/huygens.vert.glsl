layout(location=0) uniform vec3[5] uniform_array;

in vec2 vertex;

out vec2 pixel_size;
out vec3 screen_coord;

out gl_PerVertex
{
  vec4 gl_Position;
};

void main()
{
  pixel_size = vec2(1.0) / uniform_array[3].xy;
  screen_coord = vec3(calculate_aspect(vertex, uniform_array[3]), 0.0) + uniform_array[0];
  gl_Position=vec4(vertex, 0.0, 1.0);
}
