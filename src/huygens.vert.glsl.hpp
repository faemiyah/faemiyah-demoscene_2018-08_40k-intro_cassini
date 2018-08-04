static const char *g_shader_vertex_huygens = ""
#if defined(USE_LD)
"huygens.vert.glsl"
#else
"layout(location=0)uniform vec3 h[5];"
"in vec2 o;"
"out vec2 t;"
"out vec3 e;"
"out gl_PerVertex"
"{"
"vec4 gl_Position;"
"}"
";"
"void main()"
"{"
"t=vec2(1)/h[3].st,e=vec3(t0(o,h[3]),.0)+h[0],gl_Position=vec4(o,.0,1.);"
"}"
#endif
"";
