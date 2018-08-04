layout(location=0) uniform vec3[5] uniform_array;
layout(location=5) uniform sampler2D saturn_bands;
layout(location=6) uniform sampler2D saturn_rings;
layout(location=7) uniform sampler2D noise_soft;
layout(location=8) uniform sampler3D noise_hq;
layout(location=9) uniform sampler3D noise_lq;
layout(location=11) uniform samplerCube enceladus;
layout(location=13) uniform samplerCube tethys;

// For the space scenes, 1 unit = 10km.
const vec3 light = normalize(vec3(0.56, 0.35, 0.69));
const vec3 SATURN_RINGS_NORMAL = normalize(vec3(0.0, 1.0, 0.0));
const vec3 ENCELADUS_PLUME_NORMAL = normalize(vec3(-0.56, -0.1, -0.59));
const vec3 ENCELADUS_PLUME_DIR = normalize(vec3(-0.1, 0.8, 0.1));
const float SATURN_RINGS_START = 7400.0;
const float SATURN_RINGS_END = 14050.0;
const float SDF_DIST_OVERSHOOT = 8.0;
const float SDF_DIST_OVERSHOOT_SHADOW_MUL = 1.0;
const float SDF_MAX_RADIUS = 177777.0;
const float scene_time = uniform_array[4].x;

// Moons are in scale as opposed to simple version. This makes them hard to see.
const float RADIUS_SATURN = 5823.0;
const float RADIUS_DIONE = 56.2;
const float RADIUS_ENCELADUS = 25.2;
const float RADIUS_RHEA = 76.4;
const float RADIUS_TETHYS = 53.1;
const float RADIUS_TITAN = 257.5;

const float ORBIT_DIONE = 37740.0;
const float ORBIT_ENCELADUS = 23795.0;
const float ORBIT_IAPETUS = 356082.0;
const float ORBIT_RHEA = 52711.0;
const float ORBIT_TETHYS = 29462.0;
const float ORBIT_TITAN = 122187.0;

const float PERIOD_DIONE = 4.1;
const float PERIOD_ENCELADUS = 0.2;
const float PERIOD_RHEA = 3.1;
const float PERIOD_TETHYS = 1.7;
const float PERIOD_TITAN = 5.6;

in vec3 direction;

out vec4 output_color;

float get_advance(float dist_big, float dist_small)
{
  if(dist_big < dist_small)
  {
    return SDF_DIST_OVERSHOOT;
  }
  return SDF_DIST_OVERSHOOT;
}

float sdf_saturn(vec3 pos)
{
  // Saturn is in origo.
  return sdf_sphere(pos, vec3(0.0), RADIUS_SATURN);
}

float sdf_rings(vec3 pos)
{
  float dist_plane = sdf_plane_two_sided(pos, vec3(0.0), SATURN_RINGS_NORMAL, 9.0);
  float dist_shell = sdf_shell(pos, vec3(0.0), SATURN_RINGS_START, SATURN_RINGS_END);
  return max(dist_plane, dist_shell);
  //return sdf_plane(pos, vec3(0.0), vec3(0.0, 1.0, 0.0));
  //return sdf_shell(pos, vec3(0.0), SATURN_RINGS_START, SATURN_RINGS_END);
}

vec3 center_dione()
{
  return vec3(sin(PERIOD_DIONE), 0.0, cos(PERIOD_DIONE)) * ORBIT_DIONE;
}
const float g_dist_dione = length(uniform_array[0] - center_dione());

vec3 center_enceladus()
{
  return vec3(sin(PERIOD_ENCELADUS), 0.0, cos(PERIOD_ENCELADUS)) * ORBIT_ENCELADUS;
}
const float g_dist_enceladus = length(uniform_array[0] - center_enceladus());

vec3 center_rhea()
{
  return vec3(sin(PERIOD_RHEA), 0.0, cos(PERIOD_RHEA)) * ORBIT_RHEA;
}
const float g_dist_rhea = length(uniform_array[0] - center_rhea());

vec3 center_tethys()
{
  return vec3(sin(PERIOD_TETHYS), 0.0, cos(PERIOD_TETHYS)) * ORBIT_TETHYS;
}
const float g_dist_tethys = length(uniform_array[0] - center_tethys());

vec3 center_titan()
{
  return vec3(sin(PERIOD_TITAN), 0.0, cos(PERIOD_TITAN)) * ORBIT_TITAN;
}
const float g_dist_titan = length(uniform_array[0] - center_titan());

float sdf_dione(vec3 pos)
{
  return sdf_sphere(pos, center_dione(), RADIUS_DIONE);
}

float sdf_enceladus(vec3 pos)
{
  const float nc = 0.091;
  const float tc = 0.4;
  vec3 cen = pos - center_enceladus();
  float len_cen = length(cen);
  vec3 nor = cen / len_cen;
  float heighted = 0.0;
  float noised = 0.0;
  if(len_cen <= RADIUS_ENCELADUS + (tc + nc) * 1.2)
  {
    if(g_dist_enceladus < RADIUS_ENCELADUS * 18.0)
    {
      noised = sample_noise(noise_hq, nor * 1.19) * nc *
        smoothstep(-RADIUS_ENCELADUS * 18.0, -RADIUS_ENCELADUS * 11.0, -g_dist_enceladus);
    }
    if(g_dist_enceladus < RADIUS_ENCELADUS * 48.0)
    {
      heighted = (texture(enceladus, nor).a - 0.5) * tc *
        smoothstep(-RADIUS_ENCELADUS * 48.0, -RADIUS_ENCELADUS * 18.0, -g_dist_enceladus);
    }
  }
  float ht = heighted + noised;
  float surf = RADIUS_ENCELADUS + ht;
  return len_cen - surf;
}

float sdf_enceladus_plume(vec3 pos)
{
  return sdf_plane(pos, center_enceladus(), ENCELADUS_PLUME_NORMAL);
}

vec4 color_plume(vec3 pos)
{
  vec3 diff = pos - center_enceladus();
  vec3 dir = normalize(diff);
  float dist = max(length(diff) - RADIUS_ENCELADUS, 0.0);
  float angle1 = dot(dir, ENCELADUS_PLUME_DIR);
  float amplify = smoothstep(0.8, 1.0, angle1);

  if(amplify > 0.0)
  {
    angle1 *= 1.33;
    float angle2 = dot(dir, light) * 1.33;
    float angle3 = dot(dir, SATURN_RINGS_NORMAL) * 1.33;

    float ns = smoothstep(0.11, 1.8, (sample_noise(noise_hq, vec3(-angle1 - scene_time * 0.000003, dist * 0.0022, angle2 - scene_time * 0.000008)) + sample_noise(noise_hq, vec3(-angle2 - scene_time * 0.000008, dist * 0.0033, angle3))));
    float mul = pow(smoothstep(-14.0, 0.0, -dist), 2.0);
    return vec4(0.95, 0.95, 1.0, 1.0) * mul * amplify * ns;
  }
  return vec4(0.0);
}

float sdf_rhea(vec3 pos)
{
  return sdf_sphere(pos, center_rhea(), RADIUS_RHEA);
}

float sdf_tethys(vec3 pos)
{
  const float nc = 0.17;
  const float tc = 0.8;
  vec3 cen = pos - center_tethys();
  float len_cen = length(cen);
  vec3 nor = cen / len_cen;
  float heighted = 0.0;
  float noised = 0.0;
  if(len_cen <= RADIUS_TETHYS + (tc + nc) * 1.2)
  {
    if(g_dist_tethys < RADIUS_TETHYS * 18.0)
    {
      noised = sample_noise(noise_hq, nor * 2.29) * nc *
        smoothstep(-RADIUS_TETHYS * 18.0, -RADIUS_TETHYS * 11.0, -g_dist_tethys);
    }
    if(g_dist_tethys < RADIUS_TETHYS * 48.0)
    {
      heighted = (texture(tethys, nor).a - 0.5) * tc *
        smoothstep(-RADIUS_TETHYS * 48.0, -RADIUS_TETHYS * 18.0, -g_dist_tethys);
    }
  }
  float ht = heighted + noised;
  float surf = RADIUS_TETHYS + ht;
  return len_cen - surf;
}

float sdf_titan(vec3 pos)
{
  return sdf_sphere(pos, center_titan(), RADIUS_TITAN);
}

vec4 color_titan(vec3 pos, vec3 norm, vec3 dir)
{
  float ns = sqrt(smoothstep(-0.7, 0.3, sample_noise(noise_hq, pos * 0.0008)));
  vec4 col = vec4(mix(vec3(0.3, 0.75, 0.9), vec3(0.84, 0.7, 0.3), ns), 1.0);
  float dot_mix = dot(-dir, norm);
  vec4 ret = mix(vec4(0.6, 0.65, 0.7, 1.0), col, smoothstep(0.04, 1.0, dot_mix));
  return ret * smoothstep(0.0, 0.3, dot_mix);
}

#define SDF_BISECT_FUNC(bisect_func, sdf_func) vec3 bisect_func(vec3 prev, vec3 next) { vec3 mid = mix(prev, next, 0.5); for(int ii = 0; (ii < 9); ++ii) { float dist = sdf_func(mid); if(dist < 0.0) { next = mid; } else { prev = mid; } mid = (prev + next) * 0.5; } return mid; }
SDF_BISECT_FUNC(sdf_bisect_saturn, sdf_saturn)
SDF_BISECT_FUNC(sdf_bisect_rings, sdf_rings)
SDF_BISECT_FUNC(sdf_bisect_dione, sdf_dione)
SDF_BISECT_FUNC(sdf_bisect_enceladus, sdf_enceladus)
SDF_BISECT_FUNC(sdf_bisect_enceladus_plume, sdf_enceladus_plume)
SDF_BISECT_FUNC(sdf_bisect_rhea, sdf_rhea)
SDF_BISECT_FUNC(sdf_bisect_tethys, sdf_tethys)
SDF_BISECT_FUNC(sdf_bisect_titan, sdf_titan)

float shadow_saturn(vec3 pos)
{
  vec3 pos_prev;
  float ret = 1.0;
  bool rings_found = false;

  for(int ii = 0; (ii < 77); ++ii)
  {
    float dist_saturn = sdf_saturn(pos);
    float dist_rings = 0.0;
    float dist = dist_saturn;

    if(!rings_found)
    {
      dist_rings = sdf_rings(pos);
      dist = min(dist, dist_rings);
    }

    if(ii > 0)
    {
      if(dist_saturn < 0.0)
      {
        return 0.0;
      }
      if((dist_rings < 0.0) && !rings_found)
      {
        vec3 mid = sdf_bisect_rings(pos_prev, pos);
        float i_ring_tex_pos = (length(mid) - SATURN_RINGS_START) / (SATURN_RINGS_END - SATURN_RINGS_START);

        float deriv = (1.0 - abs(dot(light, SATURN_RINGS_NORMAL))) / 222.0 + length(mid - uniform_array[0]) / 777777.0;
        ret = min(1.0 - textureGrad(saturn_rings, vec2(i_ring_tex_pos, 0.0), vec2(deriv), vec2(deriv)).a, ret);
        rings_found = true;
      }
    }

    if(dot(pos, pos) > SDF_MAX_RADIUS * SDF_MAX_RADIUS)
    {
      break;
    }
 
    pos_prev = pos;
    pos += light * (abs(dist) + SDF_DIST_OVERSHOOT * SDF_DIST_OVERSHOOT_SHADOW_MUL);
  }

  return ret;
}

float shadow_others(vec3 pos)
{
  vec3 pos_prev;

  for(int ii = 0; (ii < 22); ++ii)
  {
    float dist = min(sdf_dione(pos), sdf_rhea(pos));

    if(ii > 0)
    {
      if(dist < 0.0)
      {
        return 0.0;
      }
    }

    if(dot(pos, pos) > SDF_MAX_RADIUS * SDF_MAX_RADIUS)
    {
      break;
    }
 
    pos_prev = pos;
    pos += light * (abs(dist) + SDF_DIST_OVERSHOOT * SDF_DIST_OVERSHOOT_SHADOW_MUL);
  }

  return 1.0;
}

float shadow_enceladus(vec3 pos)
{
  vec3 pos_prev;

  for(int ii = 0; (ii < 22); ++ii)
  {
    float dist = sdf_enceladus(pos);

    if(ii > 0)
    {
      if(dist < 0.0)
      {
        return 0.0;
      }
    }

    if(dot(pos, pos) > SDF_MAX_RADIUS * SDF_MAX_RADIUS)
    {
      break;
    }
 
    pos_prev = pos;
    pos += light * (abs(dist) + SDF_DIST_OVERSHOOT * SDF_DIST_OVERSHOOT_SHADOW_MUL);
  }

  return 1.0;
}

float shadow_tethys(vec3 pos)
{
  vec3 pos_prev;

  for(int ii = 0; (ii < 22); ++ii)
  {
    float dist = sdf_tethys(pos);

    if(ii > 0)
    {
      if(dist < 0.0)
      {
        return 0.0;
      }
    }

    if(dot(pos, pos) > SDF_MAX_RADIUS * SDF_MAX_RADIUS)
    {
      break;
    }
 
    pos_prev = pos;
    pos += light * (abs(dist) + SDF_DIST_OVERSHOOT * SDF_DIST_OVERSHOOT_SHADOW_MUL);
  }

  return 1.0;
}

float shadow_titan(vec3 pos)
{
  vec3 pos_prev;

  for(int ii = 0; (ii < 22); ++ii)
  {
    float dist = sdf_titan(pos);

    if(ii > 0)
    {
      if(dist < 0.0)
      {
        return 0.0;
      }
    }

    if(dot(pos, pos) > SDF_MAX_RADIUS * SDF_MAX_RADIUS)
    {
      break;
    }
 
    pos_prev = pos;
    pos += light * (abs(dist) + SDF_DIST_OVERSHOOT * SDF_DIST_OVERSHOOT_SHADOW_MUL);
  }

  return 1.0;
}

void main()
{
  vec4 solid_color = vec4(0.0);
  vec4 blend_color = vec4(0.0);
  vec3 dir = normalize(direction);
  vec3 pos = uniform_array[0];
  vec3 pos_prev = pos; // Must work if start inside object.
  float dist_prev = 0.0; // Must work if starting inside object.
  float solid_shadow = 1.0;
  float blend_shadow = 1.0;
  bool plume_found = false;
  bool rings_found = false;

  if(scene_time >= 9000.0 && scene_time < 17000.0)
  {
    const vec3 cassini_pos = vec3(-4000.0, 900.0, 8000.0);
    const float PROBE_RADIUS = 180.0;
    vec4 bounding_pos = intersect_sphere(pos, dir, cassini_pos, PROBE_RADIUS);
    if(bounding_pos.a > 0.0)
    {
      //mat3 probe_rot = mat3(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
      mat3 probe_rot = mat3(0.97, 0.11, -0.2, -0.23, 0.52, -0.82, 0.01, 0.85, 0.53);
      vec3 probe_pos = (bounding_pos.xyz - cassini_pos) / PROBE_RADIUS;

      vec4 closest;
      vec4 cassini_color = march_probe(probe_pos, dir, light, probe_rot, 0.5, noise_lq, closest);

      if(cassini_color.a > 0.0)
      {
        output_color = cassini_color;
        return;
      }
    }
  }

  for(int ii = 0; (ii < 111); ++ii)
  {
    float dist_saturn = sdf_saturn(pos);
    float dist_rings = SDF_MAX_RADIUS;
    float dist_plume = SDF_MAX_RADIUS;
    float dist_dione = sdf_dione(pos);
    float dist_enceladus = sdf_enceladus(pos);
    float dist_rhea = sdf_rhea(pos);
    float dist_tethys = sdf_tethys(pos);
    float dist_titan = sdf_titan(pos);
    float dist = min(min(min(min(min(dist_dione, dist_enceladus), dist_rhea), dist_tethys), dist_titan), dist_saturn);

    if(!rings_found)
    {
      dist_rings = sdf_rings(pos);
      dist = min(dist, dist_rings);
    }
    if(!plume_found && (scene_time >= 47000.0) && (scene_time < 53000.0))
    {
      dist_plume = sdf_enceladus_plume(pos);
      dist = min(dist, dist_plume);
    }

    if(dist_saturn < 0.0)
    {
      pos_prev = sdf_nearify(pos, pos_prev, dir, dist_prev, RADIUS_SATURN);
      pos = sdf_bisect_saturn(pos_prev, pos);

      // Rotate surface point to the opposite direction of SATURN_RINGS_NORMAL.
      mat2 surf_rot = mat2(SATURN_RINGS_NORMAL.y, -SATURN_RINGS_NORMAL.z, SATURN_RINGS_NORMAL.z, SATURN_RINGS_NORMAL.y);
      vec3 surf = normalize(vec3(pos.x, surf_rot * pos.yz));
      float lat = (surf.y * 0.5) + 0.5;
      float lon = (atan(surf.z, surf.x) + i_PI) / (i_PI * 2.0);

      float luminosity = dot(normalize(pos), light);
      solid_shadow = luminosity * shadow_saturn(pos);

#if 0
      vec3 noise = texture(noise_soft, vec2(lon, lat)).rgb - 0.5;
      solid_color = vec4(texture(saturn_bands, vec2(lat, 0.0) + noise.rg * 0.01).rgb + noise.b * 0.01, 1.0);
#else
      float deriv = length(pos - uniform_array[0]) / 5555555.0;
      solid_color = vec4(textureGrad(saturn_bands, vec2(lat, lon), vec2(deriv), vec2(deriv)).rgb, 1.0) * vec4(vec3(1.5), 1.0);
#endif
      break;
    }
    else if(dist_dione < 0.0)
    {
      //pos = sdf_bisect_dione(pos_prev, pos);

      float luminosity = dot(normalize(pos - center_dione()), light);
      solid_shadow = luminosity * shadow_others(pos);

      solid_color = vec4(vec3(0.8), 1.0);
      break;
    }
    else if(dist_enceladus < 0.0)
    {
      pos_prev = sdf_nearify(pos, pos_prev, dir, dist_prev, RADIUS_ENCELADUS);
      pos = sdf_bisect_enceladus(pos_prev, pos);
      vec3 surf = normalize(pos - center_enceladus());

      vec4 surf_color = texture(enceladus, surf) * 1.1;

      vec2 mag = vec2(0.11, 0.0);
      vec3 grad = normalize(vec3(sdf_enceladus(pos + mag.xyy), sdf_enceladus(pos + mag.yxy), sdf_enceladus(pos + mag.yyx))- sdf_enceladus(pos));

      float luminosity = dot(grad, light);
      solid_shadow = luminosity * shadow_enceladus(pos);

      solid_color = vec4(surf_color.rgb, 1.0);
      break;
    }
    else if(dist_rhea < 0.0)
    {
      //pos = sdf_bisect_rhea(pos_prev, pos);

      float luminosity = dot(normalize(pos - center_rhea()), light);
      solid_shadow = luminosity * shadow_others(pos);

      solid_color = vec4(vec3(0.7), 1.0);
      break;
    }
    else if(dist_tethys < 0.0)
    {
      pos_prev = sdf_nearify(pos, pos_prev, dir, dist_prev, RADIUS_TETHYS);
      pos = sdf_bisect_tethys(pos_prev, pos);
      vec3 surf = normalize(pos - center_tethys());

      vec2 mag = vec2(0.11, 0.0);
      vec3 grad = normalize(vec3(sdf_tethys(pos + mag.xyy), sdf_tethys(pos + mag.yxy), sdf_tethys(pos + mag.yyx)) - sdf_tethys(pos));

      float luminosity = dot(grad, light);
      solid_shadow = luminosity * shadow_tethys(pos);

      solid_color = vec4(texture(tethys, surf).rgb, 1.0);
      break;
    }
    else if(dist_titan < 0.0)
    {
      pos = sdf_bisect_titan(pos_prev, pos);

      vec3 norm = normalize(pos - center_titan());

      float luminosity = dot(norm, light);
      blend_shadow = luminosity * shadow_titan(pos);

      blend_color = color_titan(pos, norm, dir);
      break;
    }
    // Elements that do not stop the iteration should be handled last.
    else if(dist_rings < 0.0)
    {
      vec3 mid = sdf_bisect_rings(pos_prev, pos);
      float i_ring_tex_pos = (length(mid) - SATURN_RINGS_START) / (SATURN_RINGS_END - SATURN_RINGS_START);

      blend_shadow = shadow_saturn(mid);

      float deriv = (1.0 - abs(dot(dir, SATURN_RINGS_NORMAL))) / 444.0 + length(mid - uniform_array[0]) / 2222222.0;
      blend_color = textureGrad(saturn_rings, vec2(i_ring_tex_pos, 0.0), vec2(deriv), vec2(deriv)) * smoothstep(0.01, 0.02, abs(dir.y));

      rings_found = true;
    }
    else if(dist_plume < 0.0)
    {
      vec3 mid = sdf_bisect_enceladus_plume(pos_prev, pos);

      blend_color = color_plume(mid);
      
      plume_found = true;
    }

    if(dot(pos, pos) > SDF_MAX_RADIUS * SDF_MAX_RADIUS)
    {
      break;
    }

    pos_prev = pos;
    dist_prev = dist;
    pos += dir * (abs(dist) + SDF_DIST_OVERSHOOT);
  }

  // Draw some kind of weak halo over saturn to simulate the famous shot.
  if(scene_time >= 17000.0 && scene_time < 25000.0)
  {
    float phase = pow(1.0 - abs(21000.0 - scene_time) / 4000.0, 1.6);
    float dist_origo = length(cross(-uniform_array[0], -(dir + uniform_array[0])));
    vec3 closest = sqrt(pow(length(uniform_array[0]), 2.0) + dist_origo * dist_origo) * dir + uniform_array[0];
    float arg = dot(normalize(vec3(0.0, 1.0, scene_time * 0.00006)), normalize(closest));

    float str = pow(smoothstep(3000.0, 15000.0, dist_origo) * smoothstep(-36000.0, -15000.0, -dist_origo) * (1.0 - abs(closest.y * 0.00002)), 2.0) * abs(sample_noise(noise_hq, closest * 0.000011 - arg * 1.4) + sample_noise(noise_hq, -closest * 0.000009 + arg * 1.3));

    vec4 col = vec4(0.6, 0.7, 1.0, 1.0) * str * phase * 0.8;

    blend_color = (1.0 - blend_color.a) * col + blend_color;

    solid_shadow = mix(solid_shadow, 0.15 + solid_shadow * 0.85, phase);
    blend_shadow = mix(blend_shadow, 0.15 + blend_shadow * 0.85, phase);
  }

  solid_color.rgb *= solid_shadow;
  blend_color.rgb *= blend_shadow;

  output_color = solid_color * (1.0 - blend_color.a) + blend_color;
}
