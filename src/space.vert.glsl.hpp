static const char *g_shader_vertex_space = ""
#if defined(USE_LD)
"space.vert.glsl"
#else
"layout(location=0)uniform vec3 h[5];"
"in vec2 o;"
"out vec3 f;"
"out gl_PerVertex"
"{"
"vec4 gl_Position;"
"}"
";"
"void main()"
"{"
"f=d0(o,h[3],h[1],h[2]),gl_Position=vec4(o,.0,1.);"
"}"
#endif
"";
