static const char *g_shader_vertex_huygens_post = ""
#if defined(USE_LD)
"huygens_post.vert.glsl"
#else
"layout(location=0)uniform vec3 b[5];"
"in vec2 o;"
"out vec2 t;"
"out vec2 d;"
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
"t=vec2(1)/b[3].st,d=(vec3(t0(o,b[3]),.0)+b[0]).st*b[3].st,e=o*.5+.5,n=e+t,s=e+vec2(-t.s,t.t),u=e-t,m=e-vec2(-t.s,t.t),gl_Position=vec4(o,.0,1.);"
"}"
#endif
"";
