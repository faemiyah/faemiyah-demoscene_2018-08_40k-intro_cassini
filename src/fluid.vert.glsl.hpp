static const char *g_shader_vertex_fluid = ""
#if defined(USE_LD)
"fluid.vert.glsl"
#else
"in vec2 o;"
"out vec2 e;"
"out gl_PerVertex"
"{"
"vec4 gl_Position;"
"}"
";"
"void main()"
"{"
"e=o*.5+.5,gl_Position=vec4(o,.0,1.);"
"}"
#endif
"";
