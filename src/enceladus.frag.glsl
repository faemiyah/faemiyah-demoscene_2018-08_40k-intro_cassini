layout(location=0) uniform vec3[5] uniform_array;
layout(location=5) uniform sampler2D saturn_bands;
layout(location=6) uniform sampler2D saturn_rings;
layout(location=7) uniform sampler2D noise_soft;
layout(location=8) uniform sampler3D noise_hq;
layout(location=11) uniform sampler2D enceladus;

// 1 unit = 10km for background Saturn, otherwise completely arbitrary.
const vec3 light = normalize(vec3(0.26, 0.4, 0.87));
const vec3 SATURN_RINGS_NORMAL = normalize(vec3(-0.1, 0.9, 0.4));

const float SDF_DIST_OVERSHOOT = 9.0;
const float SDF_DIST_OVERSHOOT_SHADOW_MUL = 1.0;

const float SATURN_RINGS_START = 7400.0;
const float SATURN_RINGS_END = 14050.0;
const float RADIUS_SATURN = 5823.0;

const float i_RADIUS_BURST = 7000.0;

in vec3 direction;

out vec4 output_color;

vec3 center_saturn()
{
  return normalize(vec3(0.8, 0.1, -0.1)) * 23795.0;
}

float sdf_saturn(vec3 pos)
{
  return sdf_sphere(pos, center_saturn(), RADIUS_SATURN);
}

float sdf_rings(vec3 pos)
{
  float dist_plane = sdf_plane_two_sided(pos, center_saturn(), SATURN_RINGS_NORMAL, 9.0);
  float dist_shell = sdf_shell(pos, center_saturn(), SATURN_RINGS_START, SATURN_RINGS_END);
  return max(dist_plane, dist_shell);
}

float terrainL(vec2 pos)
{
  mat2 rot = mat2(0.79, -0.69, 0.6, 0.81);
  pos = rot * pos;
  vec2 der = vec2(0.0010);
  float fmul = 2.69;
  float vmul = -0.39;
  float scale = 1.0;
  float ret = 0.0;
  for(int ii = 0; ii < 4; ++ii)
  {
    //ret += iqnoise(pos) * scale;
    ret += textureGrad(noise_soft, pos, der, der).r * scale;
    scale *= vmul;
    pos = rot * pos * fmul;
  }
  return ret;
}

float crater(float op)
{
  const float RIMPOINT = 0.25;
  const float WINDOW_SHARPNESS = 20.0;

  float stepfunction = 0.5 + tanh(WINDOW_SHARPNESS * (op - RIMPOINT)) * 0.5;
  float y1 = pow(3.0 * op, 2.0) - 1.0;
  float y2 = 0.25 * (sin((op - RIMPOINT) * (i_PI / (1.0 - RIMPOINT)) + i_PI * 0.5) + 1.0);
  return stepfunction * y2 + (1.0 - stepfunction) * y1;
}

float sdf_enceladus(vec3 pos)
{
  float planar = sdf_plane(pos, vec3(0.0), vec3(0.0, 1.0, 0.0));
  float ht_scale = 500.0;
  float ht = ht_scale * 0.5;
  if(planar < ht_scale * 0.5)
  {
    float cr = 0.0;
    float rad = min(length(pos.xz * 0.00011), length((pos.xz + vec2(-100.0, 27500.0)) * 0.00006));
    if(rad < 1.0)
    {
      cr = crater(rad) * 1.4;
    }
    float gorge = (textureGrad(enceladus, pos.xz * 0.00003 + vec2(-0.1, -0.1), vec2(0.0), vec2(0.0)).r - 1.0) * 1.3;
    float base = terrainL(pos.xz * vec2(0.0000041, 0.0000061) + vec2(0.1)) / max(pow(abs(cr) * 17.0, 0.61), 2.0) * (1.0 - smoothstep(0.3, 0.9, rad) * 0.55 + smoothstep(0.9, 2.7, rad) * 0.5) * (1.0 + abs(gorge) * 1.1);
    ht = (cr + base + gorge) * ht_scale - ht_scale;
  }
  return planar - ht;
}

vec3 center_burst()
{
  return vec3(7000.0, -5000.0, 6000.0) + smoothstep(19500.0, 24000.0, uniform_array[4].x) * vec3(2000, 0.0, -2000.0) + smoothstep(16000.0, 19000.0, uniform_array[4].x) * vec3(0.0, 4500.0, 0.0);
}

float color_burst(vec3 pos)
{
#if 0
  float ret = sample_noise(noise_hq, (pos + vec3(77.0 - uniform_array[4].x * 0.11, uniform_array[4].x * -7.0, 0.0)) * vec3(0.0003, 0.000007, 0.0003)) + 0.4;
  ret *= sample_noise(noise_hq, (pos + vec3(0.0, uniform_array[4].x * -4.0, 222.0 -uniform_array[4].x * 0.22)) * vec3(0.0004, 0.000003, 0.0004)) + 0.4;
  return max(ret * ret * ret, 0.0);
#else
  vec3 dir = pos - center_burst() - vec3(0.0, -33333.0, 0.0);
  float dist = length(dir);
  dir /= dist;

  float angle1 = dot(dir, vec3(0.0, 1.0, 0.0)) * 1.33;
  float angle2 = dot(dir, light) * 1.33;
  float angle3 = dot(dir, SATURN_RINGS_NORMAL) * 1.33;
  float scene_time = uniform_array[4].x;

  float ns = sample_noise(noise_hq, vec3(-angle1 - scene_time * 0.000015, dist * 0.0000014, angle2 + scene_time * 0.00001)) + sample_noise(noise_hq, vec3(-angle2 + scene_time * 0.000011, dist * 0.0000016, angle3));
  float ns2 = sample_noise(noise_hq, vec3(-angle1 - scene_time * 0.0002, dist * 0.000011, angle2 + scene_time * 0.000008));
  return pow(abs(smoothstep(0.3, 1.6, ns) - smoothstep(-0.3, 0.9, -ns)) + 0.1, 2.0) + ns2 * 0.5;
#endif
}

float radius_burst()
{
  return i_RADIUS_BURST * (1.0 + smoothstep(22000, 25000, uniform_array[4].x) * 0.9);
}

float strength_burst(vec3 pos)
{
  return max(0.0, radius_burst() - length(pos * vec3(1.0, 0.2, 1.0) - center_burst())) / radius_burst() * (smoothstep(13500.0, 17000.0, uniform_array[4].x));
}

float sdf_burst(vec3 pos)
{
  return sdf_plane(pos, center_burst(), vec3(-0.64, 0.0, -0.9));
}

#define SDF_BISECT_FUNC(bisect_func, sdf_func) vec3 bisect_func(vec3 prev, vec3 next) { vec3 mid = mix(prev, next, 0.2); for(int ii = 0; (ii < 11); ++ii) { float dist = sdf_func(mid); if(dist < 0.0) { next = mid; } else { prev = mid; } mid = (prev + next) * 0.5; } return mid; }
SDF_BISECT_FUNC(sdf_bisect_saturn, sdf_saturn)
SDF_BISECT_FUNC(sdf_bisect_rings, sdf_rings)
SDF_BISECT_FUNC(sdf_bisect_enceladus, sdf_enceladus)
SDF_BISECT_FUNC(sdf_bisect_burst, sdf_burst)

float shadow_saturn(vec3 pos)
{
  vec3 pos_prev;
  float ret = 0.0;
  bool rings_found = false;

  for(int ii = 0; (ii < 77); ++ii)
  {
    float dist_saturn = sdf_saturn(pos);
    float dist = dist_saturn;

    if(ii > 0)
    {
      if(dist_saturn < 0.0)
      {
        return 1.0;
      }
      if(!rings_found)
      {
        float dist_rings = sdf_rings(pos);
        dist = min(dist_rings, dist_saturn);

        if(dist_rings < 0.0)
        {
          vec3 mid = sdf_bisect_rings(pos_prev, pos);
          float i_ring_tex_pos = (length(mid) - SATURN_RINGS_START) / (SATURN_RINGS_END - SATURN_RINGS_START);

          float deriv = (1.0 - abs(dot(light, SATURN_RINGS_NORMAL))) / 222.0 + length(mid - uniform_array[0]) / 777777.0;
          ret = max(textureGrad(saturn_rings, vec2(i_ring_tex_pos, 0.0), vec2(deriv), vec2(deriv)).a, ret);
          //ret = max(texture(saturn_rings, vec2(i_ring_tex_pos, 0.0)).a, ret);
          rings_found = true;
        }
      }
    }

    pos_prev = pos;
    pos += light * (abs(dist) + SDF_DIST_OVERSHOOT * SDF_DIST_OVERSHOOT_SHADOW_MUL);
  }

  return ret;
}

float shadow(vec3 pos)
{
  vec3 pos_prev;
  float ret = 0.0;
  bool burst_found = false;

  for(int ii = 0; (ii < 77); ++ii)
  {
    float dist_enceladus = 9999.0;
    if(pos.y < -100.0)
    {
      dist_enceladus = sdf_enceladus(pos);
    }

    float dist = dist_enceladus;

    if(ii > 0)
    {
      if(dist_enceladus < 0.0)
      {
        ret -= dist_enceladus * 0.2;
        if(ret >= 1.0)
        {
          return 1.0;
        }
      }
    }
    if(!burst_found)
    {
      float dist_burst = sdf_burst(pos);
      dist = min(dist_burst, dist_enceladus);

      if(dist_burst < 0.0)
      {
        pos = sdf_bisect_burst(pos, pos_prev);

        float str = strength_burst(pos);
        if(str > 0.0)
        {
          ret += color_burst(pos) * sqrt(str);
        }
        burst_found = true;
      }
    }
 
    pos_prev = pos;
    pos += light * (abs(dist) + SDF_DIST_OVERSHOOT * SDF_DIST_OVERSHOOT_SHADOW_MUL);
  }

  return ret;
}

void main()
{
  vec3 dir = normalize(direction);
  vec3 pos = uniform_array[0];
  vec3 pos_prev = pos; // Must work if start inside object.
  float dist_prev = 0.0; // Must work if starting inside object.
  float advance = 0.0;
  vec4 solid_color = vec4(0.0);
  vec4 blend_color = vec4(0.0);
  bool burst_found = false;
  bool rings_found = false;

  float FAR = 25000.0;
  float FADE_FAR = 19000.0;
  float FADE_NEAR = 5000.0;

  for(int ii = 0; (ii < 140); ++ii)
  {
    if(advance >= FAR)
    {
      break;
    }

    float dist_enceladus = 9999.0;
    if((pos.y < -100.0) || (dir.y <= 0.0))
    {
      dist_enceladus = sdf_enceladus(pos);
    }

    float dist_burst = sdf_burst(pos);
    float dist = dist_enceladus;

    if(dist_enceladus < 0.0)
    {
      pos = sdf_bisect_enceladus(pos_prev, pos);

      vec2 mag = vec2(27.0 * (1.0 + smoothstep(0.0, FAR, length(uniform_array[0] - pos)) * 3.0), 0.0);
      vec3 grad = normalize(vec3(sdf_enceladus(pos + mag.xyy), sdf_enceladus(pos + mag.yxy), sdf_enceladus(pos + mag.yyx)) - sdf_enceladus(pos)/*dist_enceladus*/);

      float luminosity = smoothstep(-0.3, 1.0, dot(grad, light)) * ((1.4 + sample_noise(noise_soft, pos.xz * 0.00001) * 1.4));

      solid_color = vec4(luminosity, luminosity, luminosity, 1.0);
      //solid_color = vec4(advance, 0.0, 0.0, 1.0);
      //solid_color = vec4(texture(enceladus, pos.xz * 0.00004).a, 0.0, 0.0, 1.0);
      //output_color = solid_color;
      //return;
      break;
    }
    else if(!burst_found)
    {
      dist = min(dist_burst, dist_enceladus);
      //dist = dist_burst;

      if(dist_burst < 0.0)
      {
        pos = sdf_bisect_burst(pos_prev, pos);

        float str = strength_burst(pos);
        if(str > 0.0)
        {
          blend_color = vec4(color_burst(pos) * str) * vec4(0.93, 0.93, 1.0, 1.0);
        }
        burst_found = true;
      }
    }

    pos_prev = pos;
    dist_prev = dist;
    float increment = abs(dist) + SDF_DIST_OVERSHOOT;
    advance += increment;
    pos += dir * increment;
  }

  // Saturn.
  if(solid_color.a <= 0.0)
  {
    vec3 pos = vec3(0.0);
    vec3 pos_prev = pos; // Must work if start inside object.
    float dist_prev = 0.0; // Must work if starting inside object.

    for(int ii = 0; (ii < 166); ++ii)
    {
      float dist_saturn = sdf_saturn(pos);
      float dist = dist_saturn;

      if(dist_saturn < 0.0)
      {
        pos_prev = sdf_nearify(pos, pos_prev, dir, dist_prev, RADIUS_SATURN);
        pos = sdf_bisect_saturn(pos_prev, pos);

        // Rotate surface point to the opposite direction of SATURN_RINGS_NORMAL.
        mat2 surf_rot = mat2(SATURN_RINGS_NORMAL.y, -SATURN_RINGS_NORMAL.z, SATURN_RINGS_NORMAL.z, SATURN_RINGS_NORMAL.y);
        vec3 nor = pos - center_saturn();
        vec3 surf = normalize(vec3(nor.x, surf_rot * nor.yz));
        float lat = (surf.y * 0.5) + 0.5;
        float lon = (atan(surf.z, surf.x) + i_PI) / (i_PI * 2.0);

        float luminosity = dot(normalize(nor), light);

        vec3 noise = texture(noise_soft, vec2(lon, lat)).rgb - 0.5;

        solid_color = vec4((texture(saturn_bands, vec2(lat, 0.0) + noise.rg * 0.01).rgb + noise.b * 0.01) * luminosity, 1.0);
        break;
      }
      if(!rings_found)
      {
        float dist_rings = sdf_rings(pos);
        dist = min(dist_rings, dist_saturn);
        if(dist_rings < 0.0)
        {
          vec3 mid = sdf_bisect_rings(pos_prev, pos);
          float i_ring_tex_pos = (length(mid - center_saturn()) - SATURN_RINGS_START) / (SATURN_RINGS_END - SATURN_RINGS_START);

          float deriv = (1.0 - abs(dot(dir, SATURN_RINGS_NORMAL))) / 444.0 + length(mid - uniform_array[0]) / 2222222.0;
          vec4 new_blend_color = textureGrad(saturn_rings, vec2(i_ring_tex_pos, 0.0), vec2(deriv), vec2(deriv));
          //blend_color = texture(saturn_rings, vec2(i_ring_tex_pos, 0.0));

          float shade = shadow_saturn(mid);
          new_blend_color.rgb *= 1.0 - shade;

          blend_color = new_blend_color * (1.0 - blend_color.a) + blend_color;

          rings_found = true;
        }
      }

      pos_prev = pos;
      dist_prev = dist;
      pos += dir * (abs(dist) + SDF_DIST_OVERSHOOT);
    }

    if(solid_color.a >= 1.0)
    {
      float shade = shadow_saturn(pos);
      solid_color.rgb *= 1.0 - shade;
    }
  }
  else
  {
    float fade = smoothstep(FADE_NEAR, FADE_FAR, advance);

    solid_color.rgb = mix(solid_color.rgb, mix(vec3(color_to_luma(solid_color.rgb)), vec3(0.1), pow(fade * 0.98, 9.0)), fade);// 1.0 - fade * );

    if(solid_color.a >= 1.0)
    {
      float shade = shadow(pos);
      solid_color.rgb *= 1.0 - shade * (1.0 - fade);
    }
  }

  output_color = solid_color * (1.0 - blend_color.a) + blend_color;
  //output_color = blend_color;
}
