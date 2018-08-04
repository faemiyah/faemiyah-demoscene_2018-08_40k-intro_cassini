static const char *g_shader_vertex_space_post = ""
#if defined(USE_LD)
"space_post.vert.glsl"
#else
"layout(location=0)uniform vec3 d[5];"
"in vec2 o;"
"out vec3 f;"
"out vec2 t;"
"out vec2 e;"
"out vec2 n;"
"out vec2 s;"
"out vec2 u;"
"out vec2 m;"
"out gl_PerVertex"
"{"
"vec4 gl_Position;"
"}"
";"
"void main()"
"{"
"f=d0(o,d[3],d[1],d[2]),t=vec2(1)/d[3].st,e=o*.5+.5,n=e+t,s=e+vec2(-t.s,t.t),u=e-t,m=e-vec2(-t.s,t.t),gl_Position=vec4(o,.0,1.);"
"}"
#endif
"";
