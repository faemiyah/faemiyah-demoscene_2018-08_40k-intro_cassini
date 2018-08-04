#version 430

/**********************************************************************
*** CONSTANTS *********************************************************
**********************************************************************/

const float i_PI = 3.141593;
const float epsilon = 0.002;

/**********************************************************************
*** COLORS ************************************************************
**********************************************************************/

const vec3 COLOR_GOLD = vec3(0.9, 0.75, 0.25);
const vec3 COLOR_ALUMINUM = vec3(0.68, 0.7, 0.74);
const vec3 COLOR_CERAMIC = vec3(0.74, 0.76, 0.78);

/// Calculates 2D screen aspect-mapped coordinates.
/// \param screen_coords Normalized screen coordinates.
/// \param screen_params Screen width, height and FoV.
/// \return Aspect ratio -mapped screen coordinates.
vec2 calculate_aspect(vec2 screen_coords, vec3 screen_params)
{
  vec2 aspect = screen_coords * (0.5 * 2.0) * screen_params.z;
  float ratio = screen_params.x / screen_params.y;
  if(ratio > 1.0)
  {
    aspect.x *= ratio;
  }
  else
  {
    aspect.y /= ratio;
  }
  return aspect;
}

/// Calculates raycast direction vector in screen space.
/// \param screen_coords Normalized screen coordinates.
/// \param screen_params Screen width, height and FoV.
/// \param forward_param Forward direction.
/// \param up_param Up direction.
vec3 calculate_direction(vec2 screen_coords, vec3 screen_params, vec3 forward_param, vec3 up_param)
{
  vec2 aspect = calculate_aspect(screen_coords, screen_params);
  vec3 fw = normalize(forward_param);
  vec3 rt = normalize(cross(fw, up_param));
  return aspect.x * rt + aspect.y * normalize(cross(rt, fw)) + fw;
}

/// Convert color to luminance.
/// \param color Input color.
/// \return Output luminance.
float color_to_luma(vec3 color)
{
  const vec3 LUMA = vec3(0.2126, 0.7152, 0.0722);
  return dot(color, LUMA);
}

/// Convert color to luminance (FXAA version).
/// FXAA seems to use slightly different luma as compared to normal.
/// \param color Input color.
/// \return Output luminance.
float color_to_luma_fxaa(vec3 color)
{
  const vec3 LUMA_FXAA = vec3(0.299, 0.587, 0.114);
  return dot(color, LUMA_FXAA);
}

/// FXAA implementation.
///
/// Adapted from GLSL example from Armin Ronacher:
/// https://github.com/mitsuhiko
///
/// In turn adapted from FXAA by Timothy Lottes from Nvidia:
/// https://developer.download.nvidia.com/assets/gamedev/files/sdk/11/FXAA_WhitePaper.pdf
///
/// \param tex Sampled texture.
/// \param pixel_size Size of pixels in screen space.
/// \param v_rgbM Texture coordinate in middle.
/// \param v_rgbNE Texture coordinate one texel northeast.
/// \param v_rgbNW Texture coordinate one texel northwest.
/// \param v_rgbSW Texture coordinate one texel southwest.
/// \param v_rgbSE Texture coordinate one texel southeast.
/// \return Antialiased color.
vec4 fxaa(sampler2D tex, vec2 pixel_size, vec2 v_rgbM, vec2 v_rgbNE, vec2 v_rgbNW, vec2 v_rgbSW, vec2 v_rgbSE)
{
  const float FXAA_REDUCE_MIN = 1.0 / 128.0;
  const float FXAA_REDUCE_MUL = 1.0 / 8.0;
  const float FXAA_SPAN_MAX = 8.0;

  vec4 texColor = texture2D(tex, v_rgbM);
  vec3 rgbM  = texColor.xyz * texColor.a;
  vec3 rgbNW = texture(tex, v_rgbNW).xyz;
  vec3 rgbNE = texture(tex, v_rgbNE).xyz;
  vec3 rgbSW = texture(tex, v_rgbSW).xyz;
  vec3 rgbSE = texture(tex, v_rgbSE).xyz;

  float lumaM  = color_to_luma_fxaa(rgbM);
  float lumaNW = color_to_luma_fxaa(rgbNW);
  float lumaNE = color_to_luma_fxaa(rgbNE);
  float lumaSW = color_to_luma_fxaa(rgbSW);
  float lumaSE = color_to_luma_fxaa(rgbSE);
  float luma_min = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
  float luma_max = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

  vec2 luma_dir = vec2(lumaSW + lumaSE - lumaNW - lumaNE, lumaNW + lumaSW - lumaNE - lumaSE);

  float i_dir_reduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);

  float i_rcp_dir_min = 1.0 / (min(abs(luma_dir.x), abs(luma_dir.y)) + i_dir_reduce);

  vec2 dir = min(vec2(FXAA_SPAN_MAX), max(vec2(-FXAA_SPAN_MAX), luma_dir * i_rcp_dir_min)) * pixel_size;

  vec3 rgbA = 0.5 * (
      texture(tex, v_rgbM + dir * (1.0 / 3.0 - 0.5)).rgb +
      texture(tex, v_rgbM + dir * (2.0 / 3.0 - 0.5)).rgb);
  vec3 rgbB = rgbA * 0.5 + 0.25 * (
      texture(tex, v_rgbM - dir * 0.5).rgb +
      texture(tex, v_rgbM + dir * 0.5).rgb);

  float lumaB = color_to_luma_fxaa(rgbB);
  if((lumaB < luma_min) || (lumaB > luma_max))
  {
    return vec4(rgbA, texColor.a);
  }
  return vec4(rgbB, texColor.a);
}

float sample_noise(sampler2D tex, vec2 pos, float deriv = 0.0)
{
  mat2 rot = mat2(0.88, 0.48, -0.48, 0.88);
  vec2 der = vec2(deriv);
  vec2 pos0 = pos;
  vec2 pos1 = rot * (pos0 * 0.5);
  vec2 pos2 = rot * (pos1 * 0.5);
  vec2 pos3 = rot * (pos2 * 0.5);
  vec2 pos4 = rot * (pos3 * 0.5);
  vec2 pos5 = rot * (pos4 * 0.5);
  return 
    - sin(textureGrad(tex, pos0, der, der).r * i_PI * 0.5) * 0.1
    + sin(textureGrad(tex, pos1, der, der).r * i_PI * 0.5) * 0.2
    - sin(textureGrad(tex, pos2, der, der).r * i_PI * 0.5) * 0.3
    + sin(textureGrad(tex, pos3, der, der).r * i_PI * 0.5) * 0.4
    - sin(textureGrad(tex, pos4, der, der).r * i_PI * 0.5) * 0.5;
    //+ sin(textureGrad(tex, pos5, der, der).r * i_PI * 0.5) * 0.6;
}

#if 0
float sample_noise(sampler3D tex, vec3 pos)
{
  const mat3 rot = mat3(-0.99, -0.16, 0.02, 0.14, -0.77, 0.63, -0.08, 0.62, 0.78);
  float fmul = 2.1;
  float vmul = 0.2;
  float scale = 1.0;
  float ret = 0.0;
  for(int ii = 0; ii < 4; ++ii)
  {
    ret += textureGrad(tex, pos, vec3(0.0), vec3(0.0)).r * scale;
    scale *= vmul;
    pos = rot * pos * fmul;
  }
  return ret;
}
#else
float sample_noise(sampler3D tex, vec3 pos)
{
  const mat3 rot = mat3(-0.99, -0.16, 0.02, 0.14, -0.77, 0.63, -0.08, 0.62, 0.78);
  //const mat3 rot = mat3(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
  vec3 der = vec3(0.0);
  vec3 pos0 = pos;
  vec3 pos1 = rot * (pos0 * 0.49);
  vec3 pos2 = rot * (pos1 * 0.49);
  vec3 pos3 = rot * (pos2 * 0.49);
  vec3 pos4 = rot * (pos3 * 0.49);

  return
    textureGrad(tex, pos0, der, der).r * 0.2
    - textureGrad(tex, pos1, der, der).r * 0.4
    + textureGrad(tex, pos2, der, der).r * 0.6
    - textureGrad(tex, pos3, der, der).r * 0.8
    + textureGrad(tex, pos4, der, der).r * 1.0;
}
#endif

/// Congruence function.
///
/// \param val Value.
/// \param divisor Divisor.
/// \return Congruence of value and divisor.
float congr(float val, float divisor)
{
  if(0.0 <= val)
  {
    return mod(val, divisor);
  }
  return divisor - mod(-val, divisor);
}

/// Exponential smooth minimum between two SDFs.
/// By Inigo Quilez, see: http://iquilezles.org/www/articles/smin/smin.htm
/// \param op1 First SDF result.
/// \param op2 Second SDF result.
/// \param k Exponent multiplier.
float sdf_smin(float op1, float op2, float k)
{
  float res = exp(-k * op1) + exp(-k * op2);
  return -log(res) / k;
}

/// Subtractional sdf.
/// \param op1 First SDF result, positive space.
/// \param op2 Second SDF result, negative space.
/// \return Distance to SDF that is first SDF but not second SDF.
float sdf_subtract(float op1, float op2)
{
  return max(op1, -op2);
}

/// Union sdf.
/// \param op1 First SDF result.
/// \param op2 Second SDF result.
/// \return Distance to SDF that is the combination of first and second SDF.
float sdf_union(float op1, float op2)
{
  return min(op1, op2);
}

/// Difference sdf.
/// \param op1 First SDF result.
/// \param op2 Second SDF result.
/// \return Distance to SDF that is the intersection of first and second SDF.
float sdf_intersect(float op1, float op2)
{
  return max(op1, op2);
}

float sdf_plane(vec3 pos, vec3 plane_pos, vec3 plane_normal)
{
  return dot(pos - plane_pos, normalize(plane_normal));
}

float sdf_plane_two_sided(vec3 pos, vec3 plane_pos, vec3 plane_normal, float thickness)
{
  vec3 diff = plane_normal * thickness * 0.5;
  float dist_pos = sdf_plane(pos, plane_pos + diff, plane_normal);
  float dist_neg = sdf_plane(pos, plane_pos - diff, -plane_normal);
  return max(dist_pos, dist_neg);
}

float sdf_sphere(vec3 pos, vec3 sphere_pos, float radius)
{
  float dist = length(sphere_pos - pos);
  return dist - radius;
}

float sdf_sphere_sector(vec3 pos, vec3 sphere_pos, vec3 sector_dir, float sector_angle, float wall_thickness)
{
  vec3 pos_dir = pos - sphere_pos;

  float radius = length(sector_dir);
  float radius_pos = length(pos_dir);

  float angle = acos(dot(pos_dir, sector_dir)/(radius*radius_pos));

  float dist = abs(radius_pos - radius) - wall_thickness;
  
  if(angle > sector_angle)
  {
    dist += (angle-sector_angle)*radius;
  }
  
  return dist;
}

/// Shell of two spheres.
float sdf_shell(vec3 pos, vec3 shell_pos, float radius_inner, float radius_outer)
{
  float dist = length(shell_pos - pos);
  float dist_pos = dist - radius_outer;
  float dist_neg = radius_inner - dist;
  return max(dist_pos, dist_neg);
}

float cubicsolver(float a, float b, float c, float d)
{
	float x1 = -b/(3*a);
	float x2 = -b*b*b/(27*a*a*a) + b*c/(6*a*a) - d/(2*a);
	float x3 = sqrt( pow(x2, 2) + pow(c/(3*a) - b*b/(9*a*a), 3) );

	return x1 + pow(x2 + x3, 1/3) + pow(x2 - x3, 1/3);
}

/// SDF of a paraboloid.
/// \param pos Position of ray.
/// \param p1 Origin of paraboloid.
/// \param p2 Axis vector of paraboloid.
/// \param radius Cutoff of paraboloid in axis-ortho direction.
/// \param steepness Paraboloid steepness (i.e. steepness*x^2).
/// \param wall_thickness Paraboloid wall thickness.
float sdf_paraboloid(vec3 pos, vec3 p1, vec3 p2, float radius, float steepness, float wall_thickness)
{
	vec3 dir_pos = pos - p1;
	
	vec3 proj_p2 = p2*(dot(dir_pos, p2)/dot(p2,p2));
	vec3 proj_p2_ortho = dir_pos - proj_p2;
	
	float len_proj_p2_ortho = length(proj_p2_ortho);
	float len_proj_p2 = length(proj_p2);

	float x = cubicsolver(steepness*steepness*4, 0, -steepness*4*len_proj_p2+2, -2*len_proj_p2_ortho);
	
	float dist = sqrt(pow(x*x-len_proj_p2, 2) + pow(len_proj_p2_ortho-x, 2));

	/*if(angle > 0)
	{
		dist = abs(len_proj_p2 - wall_thickness - steepness*pow(len_proj_p2_ortho, 2)) - wall_thickness;
	}
	else
	{
		dist = len_proj_p2 + steepness*pow(len_proj_p2_ortho, 2);
	}*/
	
	if(len_proj_p2_ortho > radius)
	{
		dist = max(dist, len_proj_p2_ortho - radius);
	}
	
	return dist;
}

float sdf_paraboloid_filled(vec3 pos, vec3 p1, vec3 p2, float steepness)
{
  float dist;
	
	vec3 dir_pos = pos - p1;
	
	vec3 proj_p2 = p2*(dot(dir_pos, p2)/dot(p2,p2));
	vec3 proj_p2_ortho = dir_pos - proj_p2;
	
	float len_proj_p2_ortho = length(proj_p2_ortho);
	float len_proj_p2 = length(proj_p2);

	float angle = dot(proj_p2, p2);

	if(angle > 0)
	{
		dist = - len_proj_p2 + steepness*pow(len_proj_p2_ortho, 2);
	}
	else
	{
		dist = len_proj_p2 + steepness*pow(len_proj_p2_ortho, 2);
	}
	
	return dist;
}

float sdf_cuboid(vec3 pos, vec3 p1, vec3 dir, vec3 up, vec3 dims)
{
	vec3 relpos = pos - p1;
	
	vec3 relpos_ax = dir * (dot(dir, relpos) / dot(dir, dir));
	vec3 relpos_up = up * (dot(up, relpos) / dot(up, up));
	
	vec3 ax_up_ortho = cross(dir, up);
	
	vec3 relpos_ax_up_ortho = ax_up_ortho*(dot(ax_up_ortho, relpos)/dot(ax_up_ortho, ax_up_ortho));
	
  float l_ax = length(relpos_ax) - dims.x/2;
	float l_up = length(relpos_up) - dims.y/2;
	float l_ax_ortho = length(relpos_ax_up_ortho) - dims.z/2;

	if(l_ax < 0 && l_up < 0 && l_ax_ortho < 0)
	{
		return max(l_ax, max(l_up, l_ax_ortho));
	}
	else
	{
		return max(l_ax, max(l_up, l_ax_ortho));
	}
}

/// Project a direction vector to another direction vector.
/// The vectors do not need to be unit vectors.
/// \param src Source direction to project.
/// \param tgt Target direction to project to.
/// \return Projected vector.
vec2 project2(vec2 src, vec2 tgt)
{
  return dot(src, tgt) / dot(tgt, tgt) * tgt;
}

/// Project a direction vector to another direction vector.
/// The vectors do not need to be unit vectors.
/// \param src Source direction to project.
/// \param tgt Target direction to project to.
/// \return Projected vector.
vec3 project3(vec3 src, vec3 tgt)
{
  return dot(src, tgt) / dot(tgt, tgt) * tgt;
}

/// Cone.
float sdf_cone(vec3 pos, vec3 p1, vec3 p2, float rad1, float rad2)
{
  vec3 cylinder_line = p2 - p1;
  float len2 = length(cylinder_line);
  vec3 dir = cylinder_line / len2;
  vec3 pp1 = pos - p1;
  float len1 = dot(dir, pp1);
  float rad = length(pp1 - len1 * dir);

  float dcap = max(-len1, len1 - len2);
  vec2 edge_pos = vec2(len1, rad - rad1);
  vec2 edge_dir = vec2(len2, rad2 - rad1);
  float diff_edge = length(project2(edge_pos, edge_dir) - edge_pos);

  if(len1 < 0.0)
  {
    return max(-len1, min(rad - rad1, diff_edge));
  }
  if(len1 > len2)
  {
    return max(len1 - len2, min(rad - rad2, diff_edge));
  }

  float sgn_radd = sign(rad - mix(rad1, rad2, len1 / len2));
  return max(sgn_radd * diff_edge, dcap);
}

/// Cone with wall thickness.
float sdf_cone_only_edges(vec3 pos, vec3 p1, vec3 p2, float rad1, float rad2, float wall_thickness)
{
  vec3 cylinder_line = p2 - p1;
  float len2 = length(cylinder_line);
  vec3 dir = cylinder_line / len2;
  vec3 pp1 = pos - p1;
  float len1 = dot(dir, pp1);
  float rdiff = length(pp1 - len1 * dir);

  float dcap = max(-len1, len1 - len2);
  vec2 edge_pos = vec2(len1, rdiff - rad1);
  vec2 edge_dir = vec2(len2, rad2 - rad1);
  float diff_edge = length(dot(edge_pos, edge_dir) / dot(edge_dir, edge_dir) * edge_dir - edge_pos) - wall_thickness;

  return max(diff_edge, max(-len1, len1 - len2));
}

/// Cylinder.
float sdf_cylinder(vec3 pos, vec3 p1, vec3 p2, float rad)
{
  vec3 cylinder_line = p2 - p1;
  float len2 = length(cylinder_line);
  vec3 dir = cylinder_line / len2;
  vec3 pp1 = pos - p1;
  float len1 = dot(dir, pp1);
  float rdiff = length(pp1 - len1 * dir) - rad;
  return max(rdiff, max(-len1, len1 - len2));
}

/// Ellipsoid.
float sdf_ellipsoid(vec3 pos, vec3 ellipsoid_pos, vec3 ea, vec3 eb, vec3 ec)
{
  vec3 diff = pos - ellipsoid_pos;
  vec3 len = vec3(length(ea), length(eb), length(ec));
  vec3 comp = vec3(dot(diff, ea / len.x), dot(diff, eb / len.y), dot(diff, ec / len.z));
  return (length(comp / len) - 1.0) * min(min(len.x, len.y), len.z);
}

/// Bring position nearer to given SDF to begin bisect search better.
///
///
vec3 sdf_nearify(vec3 pos, vec3 pos_prev, vec3 dir, float dist_prev, float limit_distance)
{
  if(dist_prev > limit_distance)
  {
    return pos - limit_distance * dir;
  }
  return pos_prev;
}

/// Intersects a ray starting from a position with a sphere.
/// See: https://gamedev.stackexchange.com/questions/96459/fast-ray-sphere-collision-code (user: Steven)
/// \param pos Ray starting position.
/// \param dir Ray direction.
/// \param sphere_pos Center of sphere.
/// \param rad Radius of sphere.
/// \return 4-component vector of the position and distance to sphere surface.
vec4 intersect_sphere(vec3 pos, vec3 dir, vec3 sphere_pos, float rad) 
{
  vec3 diff = pos - sphere_pos;
  float along = dot(diff, dir); // Distance along direction vector.
  float dist_surface = dot(diff, diff) - rad * rad;

  // Outside the sphere and direction not pointing towards center.
  if((dist_surface > 0.0) && (along > 0.0))
  {
    return vec4(0.0);
  }

  // Negative discriminant means ray misses sphere.
  float discr = along * along - dist_surface; 
  if(discr < 0.0)
  {
    return vec4(0.0);
  }

  float dist_dir = -along - sqrt(discr); 
  return vec4(pos + dist_dir * dir, dist_dir);
}

float sdf_probe_dish_boundary(vec3 pos)
{
  return sdf_cone(pos, vec3(0.0, 0.0, 0.0), vec3(0.65, 0.0, 0.0), 0.5, 1.0);
}

float sdf_probe_dish_gold(vec3 pos)
{
  float dist_dish = sdf_ellipsoid(pos, vec3(0.78, 0.0, 0.0), vec3(0.50, 0.0, 0.0), vec3(0.0, 0.6, 0.0), vec3(0.0, 0.0, 0.6));
  float dist_dish_sub1 = sdf_ellipsoid(pos, vec3(0.78, 0.0, 0.0), vec3(0.49, 0.0, 0.0), vec3(0.0, 0.58, 0.0), vec3(0.0, 0.0, 0.58));
  float dist_dish_sub2 = sdf_plane(pos, vec3(0.55, 0.0, 0.0), vec3(-1.0, 0.0, 0.0))/*sdf_cylinder(pos, vec3(0.55, 0.0, 0.0), vec3(0.7, 0.0, 0.0), 0.6)*/;
  float dist_dish_sub = sdf_union(dist_dish_sub1, dist_dish_sub2);
  
  return sdf_subtract(dist_dish, dist_dish_sub);
}

float sdf_probe_dish_gray(vec3 pos)
{
  float dist_cub1 = sdf_cuboid(pos, vec3(0.6, 0.0, 0.0), vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.8, 1.0, 0.025));
  float dist_cub2 = sdf_cuboid(pos, vec3(0.6, 0.0, 0.0), vec3(1.0, 0.0, 0.0), vec3(0.0, 0.0, 1.0), vec3(0.8, 1.0, 0.025));
  float dist_cub3 = sdf_cuboid(pos, vec3(0.6, 0.0, 0.0), vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 1.0), vec3(0.8, 1.0, 0.025));
  float dist_cub4 = sdf_cuboid(pos, vec3(0.6, 0.0, 0.0), vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, -1.0), vec3(0.8, 1.0, 0.025));

  float dist_cub_union_1 = sdf_union(dist_cub1, dist_cub2);
  float dist_cub_union_2 = sdf_union(dist_cub3, dist_cub4);
  float dist_cub_union = sdf_union(dist_cub_union_1, dist_cub_union_2);

  float dist_attach_1 = sdf_cuboid(pos, vec3(0.3, 0.345, 0.0), vec3(1.0, 0.5, 0.0), vec3(-0.5, 1.0, 0.0), vec3(0.54, 0.05, 0.05));
  float dist_attach_2 = sdf_cuboid(pos, vec3(0.3, 0.0, 0.345), vec3(1.0, 0.0, 0.5), vec3(-0.5, 0.0, 1.0), vec3(0.54, 0.05, 0.05));
  float dist_attach_3 = sdf_cuboid(pos, vec3(0.3, -0.345, 0.0), vec3(1.0, -0.5, 0.0), vec3(0.5, 1.0, 0.0), vec3(0.54, 0.05, 0.05));
  float dist_attach_4 = sdf_cuboid(pos, vec3(0.3, 0.0, -0.345), vec3(1.0, 0.0, -0.5), vec3(0.5, 0.0, 1.0), vec3(0.54, 0.05, 0.05));

  float dist_attach_union_1 = sdf_union(dist_attach_1, dist_attach_2);
  float dist_attach_union_2 = sdf_union(dist_attach_3, dist_attach_4);
  float dist_attach = sdf_union(dist_attach_union_1, dist_attach_union_2);

  float dist_diff_cone_1 = sdf_cone(pos, vec3(0.2, 0.0, 0.0), vec3(1.0, 0.0, 0.0), 0.1, 2.0);
  float dist_diff_cone_2 = sdf_cone(pos, vec3(0.2, 0.0, 0.0), vec3(1.0, 0.0, 0.0), 0.2, 1.1);

  dist_cub_union = sdf_intersect(dist_cub_union, dist_diff_cone_1);
  dist_cub_union = sdf_intersect(dist_cub_union, dist_diff_cone_2);

  dist_cub_union = sdf_union(dist_cub_union, dist_attach);

  float dist_sub = sdf_ellipsoid(pos, vec3(0.78, 0.0, 0.0), vec3(0.50, 0.0, 0.0), vec3(0.0, 0.6, 0.0), vec3(0.0, 0.0, 0.6));
//  float dist_sub_bound = sdf_cylinder(pos, vec3(0.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0), 0.5);
//  dist_sub = sdf_union(dist_sub, dist_dish_sub2);
  //sdf_paraboloid_filled(pos, vec3(0.25, 0.0, 0.0), vec3(1.0, 0.0, 0.0), 1.0);
  dist_cub_union = sdf_subtract(dist_cub_union, dist_sub);

  //sdf_paraboloid(pos, vec3(0.25, 0.0, 0.0), vec3(1.0, 0.0, 0.0), 0.5, 1.0, 0.01);

  float dist_dish_holder_1 = sdf_cone(pos, vec3(0.0, 0.0, 0.0), vec3(0.05, 0.0, 0.0), 0.20, 0.25);
  float dist_dish_holder_2 = sdf_cone(pos, vec3(0.05, 0.0, 0.0), vec3(0.15, 0.0, 0.0), 0.25, 0.25);

  float dist_dish_holder = sdf_union(dist_dish_holder_1, dist_dish_holder_2);

  return sdf_union(dist_dish_holder, dist_cub_union);
}

float sdf_probe_dish(vec3 pos)
{
	return sdf_union(sdf_probe_dish_gold(pos), sdf_probe_dish_gray(pos));
}

float sdf_probe_body_boundary(vec3 pos)
{
  return sdf_cone(pos, vec3(-0.6, 0.1, 0.0), vec3(0.0, 0.1, 0.0), 0.35, 0.35);
}

float sdf_probe_body_gray(vec3 pos)
{
  float dist_mount = sdf_cone(pos, vec3(-0.3, 0.23, 0.0), vec3(-0.3, 0.33, 0.0), 0.03, 0.18);
  float dist_mount_holder = sdf_cylinder(pos, vec3(-0.3, 0.0, 0.0), vec3(-0.3, 0.23, 0.0), 0.03);
  float dist_mount_full = min(dist_mount, dist_mount_holder);

  float dist_body_upper = sdf_cone(pos, vec3(-0.6, 0.0, 0.0), vec3(0.0, 0.0, 0.0), 0.20, 0.20);

  // Composite of all body structures
  return sdf_union(dist_mount_full, dist_body_upper);
}

float sdf_probe_body_gold(vec3 pos)
{
  float dist_mw_dish = sdf_cone_only_edges(pos, vec3(-0.3, 0.25, 0.0), vec3(-0.3, 0.35, 0.0), 0.29, 0.15, 0.01);
  float dist_mw_head = sdf_sphere_sector(pos, vec3(-0.3, 0.1, 0.0), vec3(0.0, 0.275, 0.0), 0.5, 0.01);

  return sdf_union(dist_mw_head, dist_mw_dish);
}

float sdf_probe_body(vec3 pos)
{
	return sdf_union(sdf_probe_body_gold(pos), sdf_probe_body_gray(pos));
}

float sdf_probe_tail_boundary(vec3 pos)
{
  //return sdf_cone(pos, vec3(-0.8, 0.0, 0.0), vec3(-0.6, 0.0, 0.0), 0.8, 0.8);
  return sdf_cylinder(pos, vec3(-0.8, 0.0, 0.0), vec3(-0.6, 0.0, 0.0), 0.8);
}

float sdf_probe_tail_white(vec3 pos)
{
  float dist_engine_1 = sdf_cone_only_edges(pos, vec3(-0.8, 0.1, 0.0), vec3(-0.7, 0.1, 0.0), 0.05, 0.03, 0.005);
  float dist_engine_2 = sdf_cone_only_edges(pos, vec3(-0.8, 0.0, 0.1), vec3(-0.7, 0.0, 0.1), 0.05, 0.03, 0.005);
  float dist_engine_3 = sdf_cone_only_edges(pos, vec3(-0.8, -0.1, 0.0), vec3(-0.7, -0.1, 0.0), 0.05, 0.03, 0.005);
  float dist_engine_4 = sdf_cone_only_edges(pos, vec3(-0.8, 0.0, -0.1), vec3(-0.7, 0.0, -0.1), 0.05, 0.03, 0.005);

  float dist_fin_cap_1 = sdf_cone(pos, vec3(-0.65, -0.7, 0.0), vec3(-0.65, -0.73, 0.0), 0.03, 0.01);
  float dist_fin_cap_2 = sdf_cone(pos, vec3(-0.65, 0.7, 0.0), vec3(-0.65, 0.73, 0.0), 0.03, 0.01);
  float dist_fin_cap_3 = sdf_cone(pos, vec3(-0.65, 0.0, 0.7), vec3(-0.65, 0.0, 0.73), 0.03, 0.01);
  float dist_fin_cap_4 = sdf_cone(pos, vec3(-0.65, 0.0, -0.7), vec3(-0.65, 0.0, -0.73), 0.03, 0.01);

  float dist_fin_cap_5 = sdf_cone(pos, vec3(-0.65, -0.7/sqrt(2), -0.7/sqrt(2)), vec3(-0.65, -0.73/sqrt(2), -0.73/sqrt(2)), 0.03, 0.01);
  float dist_fin_cap_6 = sdf_cone(pos, vec3(-0.65, -0.7/sqrt(2), 0.7/sqrt(2)), vec3(-0.65, -0.73/sqrt(2), 0.73/sqrt(2)), 0.03, 0.01);
  float dist_fin_cap_7 = sdf_cone(pos, vec3(-0.65, 0.7/sqrt(2), 0.7/sqrt(2)), vec3(-0.65, 0.73/sqrt(2), 0.73/sqrt(2)), 0.03, 0.01);
  float dist_fin_cap_8 = sdf_cone(pos, vec3(-0.65, 0.7/sqrt(2), -0.7/sqrt(2)), vec3(-0.65, 0.73/sqrt(2), -0.73/sqrt(2)), 0.03, 0.01);
  
  float dist_fin_cap_union_1 = sdf_union(dist_fin_cap_1, dist_fin_cap_2);
  float dist_fin_cap_union_2 = sdf_union(dist_fin_cap_3, dist_fin_cap_4);
  float dist_fin_cap_union_3 = sdf_union(dist_fin_cap_5, dist_fin_cap_6);
  float dist_fin_cap_union_4 = sdf_union(dist_fin_cap_7, dist_fin_cap_8);
  dist_fin_cap_union_1 = sdf_union(dist_fin_cap_union_1, dist_fin_cap_union_2);
  dist_fin_cap_union_2 = sdf_union(dist_fin_cap_union_3, dist_fin_cap_union_4);
  float dist_fin_cap_union = sdf_union(dist_fin_cap_union_1, dist_fin_cap_union_2);

  float dist_engine_union_1 = sdf_union(dist_engine_1, dist_engine_2);
  float dist_engine_union_2 = sdf_union(dist_engine_3, dist_engine_4);
  float dist_engine_union = sdf_union(dist_engine_union_1, dist_engine_union_2);

	return sdf_union(dist_fin_cap_union, dist_engine_union);
}

float sdf_probe_tail_gray(vec3 pos)
{
  float dist_body_lower = sdf_cone(pos, vec3(-0.7, 0.0, 0.0), vec3(-0.6, 0.0, 0.0), 0.15, 0.20);

  float dist_fin_1 = sdf_cylinder(pos, vec3(-0.65, -0.7, 0.0), vec3(-0.65, 0.7, 0.0), 0.03);
  float dist_fin_2 = sdf_cylinder(pos, vec3(-0.65, 0.0, -0.7), vec3(-0.65, 0.0, 0.7), 0.03);
  float dist_fin_3 = sdf_cylinder(pos, vec3(-0.65, -0.7/sqrt(2), -0.7/sqrt(2)), vec3(-0.65, 0.7/sqrt(2), 0.7/sqrt(2)), 0.03);
  float dist_fin_4 = sdf_cylinder(pos, vec3(-0.65, -0.7/sqrt(2), 0.7/sqrt(2)), vec3(-0.65, 0.7/sqrt(2), -0.7/sqrt(2)), 0.03);

  float dist_fin_union_1 = sdf_union(dist_fin_1, dist_fin_2);
  float dist_fin_union_2 = sdf_union(dist_fin_3, dist_fin_4);
  float dist_fin_union = sdf_union(dist_fin_union_1, dist_fin_union_2);
  dist_fin_union = sdf_union(dist_fin_union, dist_body_lower);

  return sdf_union(dist_fin_union, dist_body_lower);
}

float sdf_probe_tail(vec3 pos)
{
  return sdf_union(sdf_probe_tail_gray(pos), sdf_probe_tail_white(pos));
}

#define SDF_BISECT_FUNC_PROBE(bisect_func, sdf_func) vec3 bisect_func(vec3 prev, vec3 next) { vec3 mid = mix(prev, next, 0.5); for(int ii = 0; (ii < 5); ++ii) { float dist = sdf_func(mid); if(dist < 0.0) { next = mid; } else { prev = mid; } mid = (prev + next) * 0.5; } return mid; }
SDF_BISECT_FUNC_PROBE(sdf_bisect_probe_dish, sdf_probe_dish)
SDF_BISECT_FUNC_PROBE(sdf_bisect_probe_body, sdf_probe_body)
SDF_BISECT_FUNC_PROBE(sdf_bisect_probe_tail, sdf_probe_tail)

/// March the probe shadow.
/// \param pos Position hit in probe.
/// \param lit Light direction.
/// \return Proportion in shadow.
float shadow_probe(vec3 pos, vec3 lit)
{
  const int STEPS = 27;
  const float OVERSHOOT = 0.009;
  int body_found = 0;
  int dish_found = 0;
  int tail_found = 0;

  int ii = 0;
  for(; (ii < STEPS); ++ii)
  {
    float dist = 2.0;

    float dist_body = 2.0;
    if(body_found == 0)
    {
      dist_body = sdf_probe_body_boundary(pos);
      if(dist_body > 0.0)
      {
        dist = min(dist_body, dist);
      }
      else
      {
        ++body_found;
      }
    }
    if(body_found == 1)
    {
      dist_body = sdf_probe_body(pos);
      if(dist_body < /*0.17*/0.5)
      {
        dist = min(dist_body, dist);
      }
      else
      {
        ++body_found;
      }
    }

    float dist_dish = 2.0;
    if(dish_found == 0)
    {
      dist_dish = sdf_probe_dish_boundary(pos);
      if(dist_dish > 0.0)
      {
        dist = min(dist_dish, dist);
      }
      else
      {
        ++dish_found;
      }
    }
    if(dish_found == 1)
    {
      dist_dish = sdf_probe_dish(pos);
      if(dist_dish < 0.6)
      {
        dist = min(dist_dish, dist);
      }
      else
      {
        ++dish_found;
      }
    }

    float dist_tail = 2.0;
    if(tail_found == 0)
    {
      dist_tail = sdf_probe_tail_boundary(pos);
      if(dist_tail > 0.0)
      {
        dist = min(dist_tail, dist);
      }
      else
      {
        ++tail_found;
      }
    }
    if(tail_found == 1)
    {
      dist_tail = sdf_probe_tail(pos);
      if(dist_tail < 0.25)
      {
        dist = min(dist_tail, dist);
      }
      else
      {
        ++tail_found;
      }
    }

    if(ii > 0)
    {
      if(dist_dish < 0.0)
      {
        return 0.0;
      }
      if(dist_body < 0.0)
      {
        return 0.0;
      }
      if(dist_tail < 0.0)
      {
        return 0.0;
      }
    }

    pos += lit * (dist + OVERSHOOT);

    if(dot(pos, pos) >= 1.0)
    {
      break;
    }
  }

  return 1.0;
}

/// March the probe.
/// \param pos0 Initial position at edge of unit sphere.
/// \param rot Rotation of the probe.
/// \param dir Direction.
/// \param lit Light direction 
/// \param shading Shading intensity.
/// \param volume 3D sampler to use for some low-key noise on probe.
/// \param out_closest Filled with gradient at closest point to probe. Alpha component is closest distance.
/// \return Hit color. Alpha component of zero means no hit.
vec4 march_probe(vec3 pos, vec3 dir, vec3 lit, mat3 rot, float shading, sampler3D volume, out vec4 out_closest)
{
  const int STEPS = 110;
  const float OVERSHOOT = 0.006;
  const vec2 mag = vec2(OVERSHOOT * 0.1, 0.0);
  float dist_burn;
  int body_found = 0;
  int dish_found = 0;
  int tail_found = 0;

  pos = rot * pos;
  dir = rot * dir;
  lit = rot * lit;

  vec3 pos_prev = pos;
  out_closest = vec4(2.0);

  int ii = 0;
  for(; (ii < STEPS); ++ii)
  {
    float dist_boundary = 2.0;
    dist_burn = dist_boundary;

    float dist_body = 2.0;
    if(body_found == 0)
    {
      dist_body = sdf_probe_body_boundary(pos);
      if(dist_body > 0.0)
      {
        dist_boundary = min(dist_body, dist_boundary);
      }
      else
      {
        ++body_found;
      }
    }
    if(body_found == 1)
    {
      dist_body = sdf_probe_body(pos);
      if(dist_body < /*0.17*/0.5)
      {
        dist_burn = min(dist_body, dist_burn);
      }
      else
      {
        ++body_found;
      }
    }

    float dist_dish = 2.0;
    if(dish_found == 0)
    {
      dist_dish = sdf_probe_dish_boundary(pos);
      if(dist_dish > 0.0)
      {
        dist_boundary = min(dist_dish, dist_boundary);
      }
      else
      {
        ++dish_found;
      }
    }
    if(dish_found == 1)
    {
      dist_dish = sdf_probe_dish(pos);
      if(dist_dish < 0.6)
      {
        dist_burn = min(dist_dish, dist_burn);
      }
      else
      {
        ++dish_found;
      }
    }

    float dist_tail = 2.0;
    if(tail_found == 0)
    {
      dist_tail = sdf_probe_tail_boundary(pos);
      if(dist_tail > 0.0)
      {
        dist_boundary = min(dist_tail, dist_boundary);
      }
      else
      {
        ++tail_found;
      }
    }
    if(tail_found == 1)
    {
      dist_tail = sdf_probe_tail(pos);
      if(dist_tail < 0.25)
      {
        dist_burn = min(dist_tail, dist_burn);
      }
      else
      {
        ++tail_found;
      }
    }

    float dist = min(dist_burn, dist_boundary);

    if(out_closest.a > dist_burn)
    {
      out_closest = vec4(pos, dist_burn);
    }

    if(dist_dish < 0.0)
    {
      pos = sdf_bisect_probe_dish(pos_prev, pos);

      vec3 grad = normalize(vec3(sdf_probe_dish(pos + mag.xyy), sdf_probe_dish(pos + mag.yxy), sdf_probe_dish(pos + mag.yyx)) - sdf_probe_dish(pos));
      out_closest.rgb = grad;
      float i_ns = smoothstep(-0.4, 0.7, sample_noise(volume, pos * 0.25));

      float luminosity = mix(0.5 - shading, 0.5 + shading, max(dot(grad, lit), 0.0) * shadow_probe(pos, lit)) * (1.0 + pow(dot(reflect(dir, grad), lit) * 0.5 + 0.5, 2.0)) * i_ns;

      // Color decision
      if(sdf_probe_dish_gold(pos) < epsilon)
      {
        return vec4(COLOR_GOLD * luminosity, 1.0);
      }
      return vec4(COLOR_ALUMINUM * luminosity, 1.0);
    }
    else if(dist_body < 0.0)
    {
      pos = sdf_bisect_probe_body(pos_prev, pos);

      vec3 grad = normalize(vec3(sdf_probe_body(pos + mag.xyy), sdf_probe_body(pos + mag.yxy), sdf_probe_body(pos + mag.yyx)) - sdf_probe_body(pos));
      out_closest.rgb = grad;

      float ns = smoothstep(-0.9, 0.1, sample_noise(volume, pos * vec3(0.05, 0.9, 0.9)));
      float luminosity = mix(0.5 - shading, 0.5 + shading, max(dot(grad, lit) * shadow_probe(pos, lit), 0.0)) * (1.0 + pow(max(dot(reflect(dir, grad), lit), 0.0), 2.0)) * ns * ns;

      // Color decision
      if(sdf_probe_body_gold(pos) < epsilon)
      {
        return vec4(COLOR_GOLD * luminosity, 1.0);
      }
      return vec4(COLOR_ALUMINUM * luminosity, 1.0);
    }
    else if(dist_tail < 0.0)
    {
      pos = sdf_bisect_probe_tail(pos_prev, pos);

      vec3 grad = normalize(vec3(sdf_probe_tail(pos + mag.xyy), sdf_probe_tail(pos + mag.yxy), sdf_probe_tail(pos + mag.yyx)) - sdf_probe_tail(pos));
      out_closest.rgb = grad;

      float ns = smoothstep(-1.1, 0.4, sample_noise(volume, pos * 1.1));
      float luminosity = mix(0.5 - shading, 0.5 + shading, max(dot(grad, lit), 0.0) * shadow_probe(pos, lit)) * (1.0 + pow(max(dot(reflect(dir, grad), lit), 0.0), 2.0)) * ns * ns * 0.5;

      if(sdf_probe_tail_gray(pos) < epsilon)
      {
        return vec4(COLOR_ALUMINUM * luminosity, 1.0);
      }
      return vec4(COLOR_CERAMIC * luminosity, 1.0);
    }

    pos_prev = pos;
    pos += dir * (dist + OVERSHOOT);

    if(dot(pos, pos) >= 1.0)
    {
      break;
    }
  }

  return vec4(0.0);
}

/// Bloom function
/// \return bloom color.
vec4 vhbloom(sampler2D tex, vec2 pixel_size, vec2 texcoord)
{
  vec4 sum = vec4(0.0);
  int hrad = 16;
  int vrad = 8;

  for(int ii = 1 - hrad; ii <= hrad - 1; ++ii)
  {
    vec4 col = texture(tex, texcoord + vec2(pixel_size.x, 0.0) * ii);
    if(color_to_luma(col.rgb) > 0.0)
    {
      sum += col * (1.0 - abs(ii) / float(hrad));
    }
  }

  for(int ii = 1 - vrad; ii <= vrad - 1; ++ii)
  {
    vec4 col = texture(tex, texcoord + vec2(0.0, pixel_size.y) * ii);
    if(color_to_luma(col.rgb) > 0.0)
    {
      sum += col * (1.0 - abs(ii) / float(vrad));
    }
  }

  return sum;
}

/// Tonemap a color value.
/// Uses Haarm-Peter Duiker’s curve optimized by Jim Hejl and Richard Burgess-Dawson.
/// See: http://filmicworlds.com/blog/filmic-tonemapping-operators/
/// \param vec3 Color in.
/// \return Mapped color.
vec3 tonemap(vec3 color)
{
  //color *= 16.0; // Hard-coded exposure.
  vec3 ret = max(vec3(0.0), color - 0.004);
  return pow((ret * (6.2 * ret + 0.5)) / (ret * (6.2 * ret + 1.7) + 0.06), vec3(1/2.2));
}

#if 0
/// Hash function for noise
/// By Inigo Quilez, see: http://iquilezles.org/www/articles/morenoise/morenoise.htm
float iqhash(float op)
{
  return fract(op * 17.0 * fract(op * 0.3183099));
}

/// Hash function for noise
/// By Inigo Quilez, see: http://iquilezles.org/www/articles/morenoise/morenoise.htm
float iqhash(vec2 op)
{
  op = 50.0 * fract(op * 0.3183099);
  return fract(op.x * op.y * (op.x + op.y));
}

/// Noise function.
/// By Inigo Quilez, see: http://iquilezles.org/www/articles/morenoise/morenoise.htm
float iqnoise(vec2 op)
{
    vec2 pp = floor(op);
    vec2 ww = fract(op);
    vec2 uu = ww * ww * ww * (ww * (ww * 6.0 - 15.0) + 10.0);
    
    float aa = iqhash(pp + vec2(0.0, 0.0));
    float bb = iqhash(pp + vec2(1.0, 0.0));
    float cc = iqhash(pp + vec2(0.0, 1.0));
    float dd = iqhash(pp + vec2(1.0, 1.0));
    
    return -1.0 + 2.0 * (aa + (bb - aa) * uu.x + (cc - aa) * uu.y + (aa - bb - cc + dd) * uu.x * uu.y);
}
#endif

#if 0
vec3 nearest_point_sphere_origo(vec3 position, vec3 direction)
{
  float product = dot(-position, direction);
  return product * direction + uniform_array[0];
}

float distance_sphere_squared(vec3 position, float radius, vec3 direction)
{
  float product = dot(-position, direction);
  vec3 collision = product * direction + uniform_array[0];
  return dot(collision, collision) - radius * radius;
}
#endif
