// Following principle from https://github.com/tunabrain/incremental-fluids

layout(location=0) uniform int control;
layout(location=5) uniform sampler2D tex;
layout(location=6) uniform sampler2D boundary;
layout(location=7) uniform sampler2D initial;

in vec2 texcoord;

out vec4 output_color;

const vec2 pixel_size = vec2(1.0) / textureSize(tex, 0);
const vec2 offset_p = 0.5*vec2(pixel_size);
const vec2 offset_vx = 0.5*vec2(0.0, pixel_size.y);
const vec2 offset_vy = 0.5*vec2(pixel_size.x, 0.0);
const vec2 texcoord_up = texcoord + vec2(0.0, pixel_size.y);
const vec2 texcoord_dn = texcoord + vec2(0.0, -pixel_size.y);
const vec2 texcoord_lt = texcoord + vec2(-pixel_size.x, 0.0);
const vec2 texcoord_rt = texcoord + vec2(pixel_size.x, 0.0);
const int ADD_INFLOW = 0;
const int BUILD_PRESSURE = ADD_INFLOW + 1;
const int PROJECT = BUILD_PRESSURE + 20;
const int APPLY_PRESSURE = PROJECT + 1;
const int ADVECT_DYE = APPLY_PRESSURE + 1;
const int ADVECT_SIMULATIONSPACE = ADVECT_DYE + 1;

// Physical constants
const float dx = pixel_size.x;
const float dt = 0.1;
const float rho = 0.1;

/// Boundary values must be -1, 1 or 0.
/// 8-bit color channels cannot encode exactly half.
vec4 bval(vec4 op)
{
  vec4 ret = op * 2.0 - 1.0;

  if(abs(ret.x) < 0.1)
  {
    ret.x = 0.0;
  }
  if(abs(ret.y) < 0.1)
  {
    ret.y = 0.0;
  }
  if(abs(ret.z) < 0.1)
  {
    ret.z = 0.0;
  }
  if(abs(ret.w) < 0.1)
  {
    ret.w = 0.0;
  }

  return ret;
}

vec4 fetch_boundary(sampler2D tex_to_sample, sampler2D boundary, vec2 coord, vec2 offset)
{
  vec4 fetch_pars = bval(texture(boundary, coord));

  vec4 ret = texture(tex_to_sample, coord + vec2(fetch_pars.rg) * pixel_size);
  ret.rg *= fetch_pars.ba;

  return ret;
}

float build_pressure()
{
  float scale = 1/dx;

  vec4 C = texture(tex, texcoord);
  vec4 T = texture(tex, texcoord_up);
  vec4 R = texture(tex, texcoord_rt);
//  vec4 T = fetch_boundary(tex, boundary, texcoord_up);
//  vec4 B = fetch_boundary(tex, boundary, texcoord_dn);
//  vec4 L = fetch_boundary(tex, boundary, texcoord_lt);
//  vec4 R = fetch_boundary(tex, boundary, texcoord_rt);

//  return -scale*(R.r - L.r + T.g - B.g);
  return scale*(C.r - R.r + C.g - T.g);
}

float project()
{
  float scale = dt/(rho*dx*dx);

  float T = texture(tex, texcoord_up).b;
  float B = texture(tex, texcoord_dn).b;
  float L = texture(tex, texcoord_lt).b;
  float R = texture(tex, texcoord_rt).b;

//  float T = fetch_boundary(tex, boundary, texcoord_up).b;
//  float B = fetch_boundary(tex, boundary, texcoord_dn).b;
//  float L = fetch_boundary(tex, boundary, texcoord_lt).b;
//  float R = fetch_boundary(tex, boundary, texcoord_rt).b;

  float offdiag = -scale*(T+B+L+R);

//  return (fetch_boundary(initial, boundary, texcoord).b - offdiag) / (4*scale);
  return (texture(initial, texcoord).b - offdiag) / (4*scale);
}

vec2 apply_pressure()
{
  float scale = dt/(rho*dx);

//  float T = fetch_boundary(tex, boundary, texcoord_up).b;
//  float B = fetch_boundary(tex, boundary, texcoord_dn).b;
//  float L = fetch_boundary(tex, boundary, texcoord_lt).b;
//  float R = fetch_boundary(tex, boundary, texcoord_rt).b;

//  return vec2(R-L, T-B)*scale;
  float C = texture(tex, texcoord).b;
  float B = texture(tex, texcoord_dn).b;
  float L = texture(tex, texcoord_lt).b;

  return vec2(L-C, B-C)*scale;
}

vec4 advect_dye(sampler2D source)
{
//  vec2 advect_vec = - fetch_boundary(source, boundary, texcoord).rg * dt;
//  return fetch_boundary(tex, boundary, texcoord + advect_vec);
  vec2 offset = texture(source, texcoord).rg;
  return texture(tex, texcoord - offset*pixel_size*dt);
}

vec4 advect_simulationspace(sampler2D source)
{
	vec2 off_vx = texture(source, texcoord - offset_vy).rg;
	float vx = texture(tex, texcoord - (off_vx*pixel_size)*dt).r;
	vec2 off_vy = texture(source, texcoord - offset_vx).rg;
	float vy = texture(tex, texcoord - (off_vy*pixel_size)*dt).g;
	
	vec2 off_p = texture(source, texcoord - offset_p).rg;
	vec2 pa = texture(tex, texcoord - (off_p+offset_p)*pixel_size*dt).ba;
	
  return vec4(vx, vy, pa.x, pa.y);
}

void main()
{
  if(control <= ADD_INFLOW)
  {
    vec4 add_color = texture(initial, texcoord);
    output_color = texture(tex, texcoord);
    if(add_color.r != 0.0)
    {
    	output_color.r = add_color.r;
    }
    if(add_color.g != 0.0)
    {
    	output_color.g = add_color.g;
    }
    if(add_color.b != 0.0)
    {
    	output_color.b = add_color.b;
    }
    if(add_color.a != 1.0)
    {
    	output_color.a = add_color.a;
    }
  }
  else if(control <= BUILD_PRESSURE)
  {
//  	output_color = texture(tex, texcoord);
    output_color.b = build_pressure();
  }
  else if(control <= PROJECT)
  {
  	output_color = texture(tex, texcoord);
  	output_color.b = project();
  }
  else if(control <= APPLY_PRESSURE)
  {
	  output_color = texture(tex, texcoord);
	  output_color.rg += apply_pressure();
  }
	else if(control <= ADVECT_DYE)
  {
    // In this phase, Initial will contain the simulation space and tex will contain the dye texture
    output_color = advect_dye(initial);
  }
  else if(control <= ADVECT_SIMULATIONSPACE)
  {
    //output_color = advect_dye(tex);
    output_color = advect_simulationspace(tex);
  }
  // Default: just copy from input to output.
  else
  {
    output_color = texture(initial, texcoord);
  }
}
