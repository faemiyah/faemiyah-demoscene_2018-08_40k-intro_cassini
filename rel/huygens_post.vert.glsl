layout(location=0) uniform vec3[5] uniform_array;

in vec2 vertex;

out vec2 pixel_size;
out vec2 screen_coord;
out vec2 texcoord;
out vec2 texcoordNE;
out vec2 texcoordNW;
out vec2 texcoordSW;
out vec2 texcoordSE;

out gl_PerVertex
{
  vec4 gl_Position;
};

void main()
{
  pixel_size = vec2(1.0) / uniform_array[3].xy;
  screen_coord = (vec3(calculate_aspect(vertex, uniform_array[3]), 0.0) + uniform_array[0]).xy * uniform_array[3].xy;
  texcoord = vertex * 0.5 + 0.5;
  texcoordNE = texcoord + pixel_size;
  texcoordNW = texcoord + vec2(-pixel_size.x, pixel_size.y);
  texcoordSW = texcoord - pixel_size;
  texcoordSE = texcoord - vec2(-pixel_size.x, pixel_size.y);
  gl_Position=vec4(vertex, 0.0, 1.0);
}
