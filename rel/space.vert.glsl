layout(location=0) uniform vec3[5] uniform_array;

in vec2 vertex;

out vec3 direction;

out gl_PerVertex
{
  vec4 gl_Position;
};

void main()
{
  direction = calculate_direction(vertex, uniform_array[3], uniform_array[1], uniform_array[2]);
  gl_Position=vec4(vertex, 0.0, 1.0);
}
