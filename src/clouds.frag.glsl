layout(location=0) uniform vec3[5] uniform_array;
layout(location=6) uniform sampler2D saturn_rings;
layout(location=7) uniform sampler2D noise_soft;
layout(location=8) uniform sampler3D noise_hq;
layout(location=9) uniform sampler3D noise_lq;
layout(location=10) uniform mat3 rotation;

// 1 unit = 10km for background Saturn, otherwise completely arbitrary.
const vec3 light = normalize(vec3(0.56, 0.35, 0.69));
const vec3 SATURN_RINGS_NORMAL = normalize(vec3(-0.8, 0.7, 0.9));
const vec3 CENTER_SATURN = vec3(0.0, 0.0, -3500.0);

const vec3 PROBE_START = vec3(30000.0, 300000.0, -1000000.0);
const vec3 PROBE_END = vec3(0.0, 15500.0, 0.0);
const vec3 PROBE_DIR = normalize(PROBE_END - PROBE_START);

const vec3 FALLING_EXPLOSION_POS = vec3(0.0, 50000.0, 0.0);
const vec3 FALLING_EXPLOSION_TRAIL_DIR = normalize(FALLING_EXPLOSION_POS - PROBE_START);

const vec3 DEBRIS_END_1 = vec3(-17000.0, 40000.0, 22000.0);
const vec3 DEBRIS_END_2 = vec3(-1000.0, 42000.0, 28000.0);
const vec3 DEBRIS_END_3 = vec3(10000.0, 25000.0, 2000.0);
const vec3 DEBRIS_START_1 = vec3(2000.0, 50000.0, -2000.0);
const vec3 DEBRIS_START_2 = vec3(0000.0, 51000.0, -2500.0);
const vec3 DEBRIS_START_3 = vec3(-2500.0, 50000.0, 0.0);
const vec3 DEBRIS_DIR_1 = normalize(DEBRIS_END_1 - DEBRIS_START_1);
const vec3 DEBRIS_DIR_2 = normalize(DEBRIS_END_2 - DEBRIS_START_2);
const vec3 DEBRIS_DIR_3 = normalize(DEBRIS_END_3 - DEBRIS_START_3);

const float SDF_DIST_OVERSHOOT = 50.0;

const float SATURN_RINGS_START = 7400.0;
const float SATURN_RINGS_END = 14050.0;
const float SATURN_RINGS_CUTOFF = 222222222;

const float DEBRIS_RADIUS = 1500;
const float PROBE_TRAIL_RADIUS = 1500.0;
const float PROBE_START_TIME = 14000.0;
const float PROBE_END_TIME = 27000.0;
const float PROBE_RADIUS = 1100.0;
const float TRAIL_END_TIME = 36000.0;
const float EXPLOSION_START_TIME = 32000.0;
const float EXPLOSION_END_TIME = 36000.0;
const float EXPLOSION_RADIUS = 7000.0 * smoothstep(EXPLOSION_START_TIME, EXPLOSION_END_TIME, uniform_array[4].x);
const float FALLING_EXPLOSION_RADIUS = 7000.0 * (1.0 + smoothstep(TRAIL_END_TIME, TRAIL_END_TIME + 7000.0, uniform_array[4].x) * 0.4);

in vec3 direction;

out vec4 output_color;

float sdf_rings(vec3 pos)
{
  float dist_plane = sdf_plane_two_sided(pos, CENTER_SATURN, SATURN_RINGS_NORMAL, SDF_DIST_OVERSHOOT);
  float dist_shell = sdf_shell(pos, CENTER_SATURN, SATURN_RINGS_START, SATURN_RINGS_END);
  return max(dist_plane, dist_shell);
}

float sdf_clouds(vec3 pos)
{
  vec3 cpos = pos * 0.000004;
  float fmul = 3.7;
  float vmul = -0.39;
  float scale = 1.0;
  float ret = 0.0;
  for(int ii = 0; ii < 3; ++ii)
  {
    ret += (texture(noise_lq, cpos).r - 0.1) * scale;
    scale *= vmul;
    cpos = rotation * cpos * fmul;
  }
  return ret * 255.0 + max(pos.y * 0.03, -77.0);
}

/// Explosion position.
vec3 g_explosion_pos = PROBE_END;

/// Explosion radius.
float g_explosion_radius = EXPLOSION_RADIUS;

float sdf_explosion(vec3 pos)
{
  vec3 cpos = pos * 0.000009;
  float fmul = 2.7;
  float vmul = -0.47;
  float scale = 1.0;
  float ret = 0.0;
  for(int ii = 0; ii < 4; ++ii)
  {
    ret += (texture(noise_lq, cpos).r - 0.1) * scale;
    scale *= vmul;
    cpos = rotation * cpos * fmul;
  }

  float explosion_dist = length(pos - g_explosion_pos) / max(g_explosion_radius, 1.0);

  return ret * 277.0 - 211.0 + explosion_dist * 222.0;
}

vec2 march_explosion(vec3 pos, vec3 dir)
{
  vec3 prev_pos = pos;
  vec2 ret = vec2(0.0);

  for(int ii = 0; ii < 333; ++ii)
  {
    float explosion_dist = length(pos - g_explosion_pos);
    if(explosion_dist > g_explosion_radius)
    {
      break;
    }

    float dist = sdf_explosion(pos);

    if(dist < 0.0)
    {
      float tunneling = 0.0007;
      float strength = abs(dist) * 0.0005;

      vec2 mag = vec2(1444.0, 0.0);
      vec3 grad = normalize(vec3(sdf_explosion(pos + mag.xyy), sdf_explosion(pos + mag.yxy), sdf_explosion(pos + mag.yyx)) - dist);
      float luminosity = dot(grad, light) * 0.5 + 0.5;

      ret += vec2(luminosity, 1.0) * (tunneling + strength);

      if(ret.y >= 1.0)
      {
        break;
      }
    }

    prev_pos = pos;
    pos += dir * (abs(dist) + SDF_DIST_OVERSHOOT * 0.5);
  }

  return ret;
}

/// Trail start position.
vec3 g_trail_start = PROBE_START;

/// Trail direction.
vec3 g_trail_dir = PROBE_DIR;

/// Trail step add.
float g_trail_step_add = (uniform_array[4].x - PROBE_END_TIME) * 11.0;

/// Trail step start.
float g_trail_step_start = 930000.0 + g_trail_step_add;

/// Trail step end.
float g_trail_step_end = 1010000.0 + g_trail_step_add;

float sdf_trail(vec3 pos)
{
  vec3 cpos = pos * 0.00004;
  float fmul = 2.7;
  float vmul = -0.49;
  float scale = 1.0;
  float ret = 0.0;
  for(int ii = 0; ii < 3; ++ii)
  {
    ret += (texture(noise_lq, cpos).r - 0.1) * scale;
    scale *= vmul;
    cpos = rotation * cpos * fmul;
  }

  vec3 diff = pos - g_trail_start;
  float len_trail = dot(g_trail_dir, diff);
  float trail_dist = length(pos - (g_trail_start + len_trail * g_trail_dir));
  float trail_add = trail_dist * (0.18+smoothstep(g_trail_step_start, g_trail_step_end, len_trail) * 2);

  return ret * 155.0 - 188.0 + trail_add;
}

vec2 march_trail(vec3 pos, vec3 dir)
{
  vec3 prev_pos = pos;
  vec2 ret = vec2(0.0);

  for(int ii = 0; ii < 333; ++ii)
  {
    vec3 diff = pos - g_trail_start;
    float trail_dist = length(pos - (g_trail_start + dot(g_trail_dir, diff) * g_trail_dir));
    if(trail_dist > PROBE_TRAIL_RADIUS)
    {
      break;
    }

    float dist = sdf_trail(pos);

    if(dist < 0.0)
    {
      float tunneling = 0.001;
      float strength = abs(dist) * 0.0009;

      vec2 mag = vec2(222.0, 0.0);
      vec3 grad = normalize(vec3(sdf_trail(pos + mag.xyy), sdf_trail(pos + mag.yxy), sdf_trail(pos + mag.yyx)) - dist);
      float luminosity = dot(grad, light) * 0.5 + 0.5;

      ret += vec2(luminosity, 1.0) * (tunneling + strength);

      if(ret.y >= 1.0)
      {
        break;
      }
    }

    prev_pos = pos;
    pos += dir * (abs(dist) + SDF_DIST_OVERSHOOT * 0.5);
  }

  return ret;
}

#define SDF_BISECT_FUNC(bisect_func, sdf_func) vec3 bisect_func(vec3 prev, vec3 next) { vec3 mid = mix(prev, next, 0.5); for(int ii = 0; (ii < 5); ++ii) { float dist = sdf_func(mid); if(dist < 0.0) { next = mid; } else { prev = mid; } mid = (prev + next) * 0.5; } return mid; }
SDF_BISECT_FUNC(sdf_bisect_clouds, sdf_clouds)
SDF_BISECT_FUNC(sdf_bisect_rings, sdf_rings)

float shadow(vec3 pos)
{
  float increment = 0.2;
  float ret = 0.0;

  for(float ii = 0.0; (ii < 1.0); ii += increment)
  {
    float dist_clouds = sdf_clouds(pos);

    if(dist_clouds > 0.0)
    {
      break;
    }

    ret += increment;
    pos += (abs(dist_clouds) + SDF_DIST_OVERSHOOT) * light;
  }

  return ret;
}

void main()
{
  vec4 blend_color = vec4(0.0);
  vec4 solid_color = vec4(0.0);
  vec4 burn_color = vec4(0.0);
  vec3 dir = normalize(direction);
  vec3 pos = uniform_array[0];
  vec3 first_pos = pos; // Must work if start inside object.
  vec3 pos_prev = pos; // Must work if start inside object.
  vec2 cloud_str = vec2(0.0);
  vec2 trail_str = vec2(0.0);
  float dist_prev = 0.0; // Must work if starting inside object.
  float travelled = 0.0;
  float burn = 0.0;
  bool cassini_found = false;
  bool explosion_found = false;
  bool trail_found = false;

  const float FAR = 120000.0;
  const float NEAR = 44444.0;
  const float CLOUD_TOP_LEVEL = 8888.0;
  const float scene_time = uniform_array[4].x;

  if((scene_time >= PROBE_START_TIME) && (scene_time <= PROBE_END_TIME))
  {
    vec3 cassini_pos = mix(PROBE_START, PROBE_END, 0.829 + (scene_time - PROBE_START_TIME) * 0.0000026);
    vec4 bounding_pos = intersect_sphere(pos, dir, cassini_pos, PROBE_RADIUS);
    if(bounding_pos.a != 0)
    {
//      mat3 probe_rot = mat3(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
      mat3 probe_rot = transpose(mat3(-0.408289, -0.889558, 0.204907, -0.0877096, 0.261659, 0.961167, -0.908629, 0.374462, -0.184855));
      vec3 probe_pos = (bounding_pos.xyz - cassini_pos) / PROBE_RADIUS;

      vec4 closest;
      //vec4 cassini_color = march_probe(probe_pos, dir, light, rotation, 0.35, noise_lq, closest);
      vec4 cassini_color = march_probe(probe_pos, dir, light, probe_rot, 0.35, noise_lq, closest);
      closest.xyz = normalize(transpose(probe_rot) * closest.xyz);
      burn = smoothstep(-0.045, -0.0, -closest.a) * max(dot(closest.xyz, PROBE_DIR) * 0.7 + 0.3, 0.0);
      burn_color = vec4(1.0, 1.0, 0.95, 1.0) * burn * 1.0;

      if(cassini_color.a > 0.0)
      {
        solid_color = cassini_color;
        output_color = solid_color * (1.0 - burn_color.a) + burn_color;
        return;
      }
    }
  }

  for(int ii = 0; (ii < 222); ++ii)
  {
    if(travelled >= FAR)
    {
      break;
    }

    float dist_explosion = FAR;
    float dist_trail = FAR;
    float dist_trail_1 = FAR;
    float dist_trail_2 = FAR;
    float dist_debris_1 = FAR;
    float dist_debris_2 = FAR;
    float dist_debris_3 = FAR;
    float dist_clouds;

    if(pos.y > CLOUD_TOP_LEVEL)
    {
      dist_clouds = (pos.y - CLOUD_TOP_LEVEL) / abs(dir.y);

      if(scene_time > PROBE_END_TIME)
      {
        if(scene_time < TRAIL_END_TIME)
        {
          if(!explosion_found)
          {
            dist_explosion = sdf_sphere(pos, PROBE_END, EXPLOSION_RADIUS);
          }
          if(!trail_found)
          {
            dist_trail_1 = sdf_cylinder(pos, PROBE_START, PROBE_END, PROBE_TRAIL_RADIUS);
            dist_trail = dist_trail_1;
          }
        }
        // Not probe or trail, it's falling debris.
        else
        {
          if(!explosion_found)
          {
            dist_explosion = sdf_sphere(pos, FALLING_EXPLOSION_POS, FALLING_EXPLOSION_RADIUS);
          }
          if(!trail_found)
          {
            dist_trail_2 = sdf_cylinder(pos, PROBE_START, FALLING_EXPLOSION_POS, PROBE_TRAIL_RADIUS);
            dist_debris_1 = sdf_cylinder(pos, DEBRIS_START_1, DEBRIS_END_1, DEBRIS_RADIUS);
            dist_debris_2 = sdf_cylinder(pos, DEBRIS_START_2, DEBRIS_END_2, DEBRIS_RADIUS);
            dist_debris_3 = sdf_cylinder(pos, DEBRIS_START_3, DEBRIS_END_3, DEBRIS_RADIUS);
            dist_trail = min(dist_trail_2, min(dist_debris_1, min(dist_debris_2, dist_debris_3)));
          }
        }
      }
    }
    else
    {
      dist_clouds = sdf_clouds(pos);
    }

    float dist = min(dist_clouds, min(dist_trail, dist_explosion));

    if(dist_clouds < 0.0)
    {
      float tunneling = 0.0017;
      float strength = abs(dist_clouds) * 0.0017;

      if(cloud_str.y <= 0.0)
      {
        first_pos = pos;
      }

      vec2 mag = vec2(1777.0, 0.0);
      vec3 grad = normalize(vec3(sdf_clouds(pos + mag.xyy), sdf_clouds(pos + mag.yxy), sdf_clouds(pos + mag.yyx)));
      float luminosity = dot(grad, light) * 0.5 + 0.5;

      cloud_str += vec2(luminosity, 1.0) * (tunneling + strength);

      if(cloud_str.y >= 1.0)
      {
        cloud_str /= cloud_str.y;
        break;
      }
    }
    else if(dist_explosion < 0.0)
    {
      if(scene_time < TRAIL_END_TIME)
      {
        trail_str += march_explosion(pos, dir);
      }
      else
      {
        g_explosion_pos = FALLING_EXPLOSION_POS;
        g_explosion_radius = FALLING_EXPLOSION_RADIUS;
        trail_str += march_explosion(pos, dir);
      }

      if(trail_str.y >= 1.0)
      {
        trail_str /= trail_str.y;
        break;
      }

      explosion_found = true;
    }
    else if(dist_trail < 0.0)
    {
      if(dist_trail_1 < 0.0)
      {
        trail_str += march_trail(pos, dir);
      }
      else if(dist_trail_2 < 0.0)
      {
        g_trail_dir = FALLING_EXPLOSION_TRAIL_DIR;
        trail_str += march_trail(pos, dir);
      }
      else
      {
        g_trail_step_add = (uniform_array[4].x - TRAIL_END_TIME) * 2;
        g_trail_step_start = 0.0 + g_trail_step_add;
        g_trail_step_end = 25000.0 + g_trail_step_add;

        if(dist_debris_1 < 0.0)
        {
          g_trail_dir = DEBRIS_DIR_1;
          g_trail_start = DEBRIS_START_1;
          trail_str += march_trail(pos, dir);
        }
        else if(dist_debris_2 < 0.0)
        {
          g_trail_dir = DEBRIS_DIR_2;
          g_trail_start = DEBRIS_START_2;
          trail_str += march_trail(pos, dir);
        }
        else if(dist_debris_3 < 0.0)
        {
          g_trail_dir = DEBRIS_DIR_3;
          g_trail_start = DEBRIS_START_3;
          trail_str += march_trail(pos, dir);
        }
      }

      if(trail_str.y >= 1.0)
      {
        trail_str /= trail_str.y;
        break;
      }

      trail_found = true;
    }

    pos_prev = pos;
    dist_prev = dist;
    float advance = (abs(dist) + SDF_DIST_OVERSHOOT);
    travelled += advance;
    pos += dir * advance;
  }

  travelled = min(travelled, FAR);

  vec3 fog_color = vec3(0.68, 0.69, 0.56);
  vec3 fog_color_dark = vec3(0.12, 0.12, 0.08);

  vec3 trail_dark_color = vec3(0.8, 0.6, 0.6);
  vec3 trail_bright_color = vec3(1.2, 1.15, 1.1);
  vec3 trail_color = mix(trail_dark_color, trail_bright_color, trail_str.x);
  vec4 trail_blend_color = vec4(trail_color * trail_str.x, sqrt(trail_str.y));

  vec3 cloud_bright_color = vec3(0.91, 0.71, 0.46);
  vec3 cloud_color = mix(fog_color, cloud_bright_color, cloud_str.x);
  vec4 cloud_blend_color = vec4(cloud_color * cloud_str.x, cloud_str.y);

  float graying = pow(travelled / FAR, 1.0);
  cloud_blend_color *= (1.0 - graying);

  blend_color = cloud_blend_color * (1.0 - trail_blend_color.a) + trail_blend_color;

  vec3 fade_color = vec3(0.87, 0.76, 0.55) * (dir.y * 0.3 + 0.7);
  float mm = dir.y * 0.5 + 0.5;
  vec4 bg_color = mix(vec4(fog_color, 1.0), vec4(0.0, 0.0, 0.0, 0.1), mm) * (0.98 + texture(noise_lq, dir * 0.8).r * 0.02);
  if(dir.y < -0.333333)
  {
    bg_color = mix(vec4(fog_color_dark, 1.0), bg_color, 3*mm);
  }
  bg_color.xyz = mix(fade_color, bg_color.xyz, min(travelled, NEAR) / NEAR);

  output_color = (bg_color * (1.0 - blend_color.a) + blend_color) * (1.0 - burn_color.a) + burn_color;

  // March rings if bg color is not completely opaque.
  if(output_color.a < 1.0)
  {
    vec3 pos = vec3(0.0);
    vec3 pos_prev = pos; // Must work if start inside object.
    float dist_prev = 0.0; // Must work if starting inside object.

    for(int ii = 0; (ii < 24); ++ii)
    {
      if(dot(pos, pos) > SATURN_RINGS_CUTOFF)
      {
        break;
      }

      float dist = sdf_rings(pos);

      if(dist < 0.0)
      {
        vec3 mid = sdf_bisect_rings(pos_prev, pos);
        float i_ring_tex_pos = (length(mid - CENTER_SATURN) - SATURN_RINGS_START) / (SATURN_RINGS_END - SATURN_RINGS_START);

        float deriv = (1.0 - abs(dot(dir, SATURN_RINGS_NORMAL))) / 444.0 + length(mid) / 2222222.0;
        vec4 new_blend_color = textureGrad(saturn_rings, vec2(i_ring_tex_pos, 0.0), vec2(deriv), vec2(deriv));

        output_color = new_blend_color * pow((1.0 - output_color.a), 2.4) + output_color;
        break;
      }

      pos_prev = pos;
      dist_prev = dist;
      pos += dir * (abs(dist) + SDF_DIST_OVERSHOOT);
    }
  }
}
