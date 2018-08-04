in vec2 vertex;

out vec2 texcoord;

out gl_PerVertex
{
  vec4 gl_Position;
};

void main()
{
  texcoord = vertex * 0.5 + 0.5;
  gl_Position=vec4(vertex, 0.0, 1.0);
}
