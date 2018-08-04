layout(location=0) uniform vec3[5] uniform_array;
layout(location=5) uniform sampler2D saturn_bands;
layout(location=6) uniform sampler2D saturn_rings;
layout(location=7) uniform sampler2D noise_soft;
layout(location=8) uniform sampler3D noise_volume;
layout(location=14) uniform samplerCube trail;

// For the space scenes, 1 unit = 10km.
const vec3 light = normalize(vec3(0.56, 0.35, 0.69));
const vec3 LINJA = normalize(cross(light, vec3(0.0, 1.0, 0.0)));
const vec3 SOLAR_PLANE = cross(light, LINJA);
const vec3 SATURN_RINGS_NORMAL = normalize(vec3(0.0, 1.0, 0.0));

const vec3 COLOR_SATURN = vec3(0.8, 0.7, 0.5);
const vec3 COLOR_RINGS = vec3(0.9, 0.7, 0.5);
const vec3 COLOR_DIONE = vec3(0.6, 0.7, 0.9);
const vec3 COLOR_ENCELADUS = vec3(0.8, 0.6, 0.9);
const vec3 COLOR_IAPETUS = vec3(0.6, 0.9, 0.7);
const vec3 COLOR_RHEA = vec3(0.9, 0.5, 0.7);
const vec3 COLOR_TETHYS = vec3(0.5, 0.9, 0.6);
const vec3 COLOR_TITAN = vec3(0.9, 0.8, 0.5);

const float SDF_DIST_OVERSHOOT = 8.0;
const float SDF_DIST_OVERSHOOT_SHADOW_MUL = 1.0;
const float SDF_CUTOFF = 133333333333.0;

const float SATURN_RINGS_START = 7400.0;
const float SATURN_RINGS_END = 14050.0;
const float SATURN_RINGS_THICKNESS = 9.0;
const float ORBITAL_THICKNESS = 16.0;

// Moons are 10x the actual size so that they are actually visible.
const float RADIUS_SATURN = 5823.0;
const float RADIUS_DIONE = 562.0;
const float RADIUS_ENCELADUS = 252.0;
const float RADIUS_IAPETUS = 734.5;
const float RADIUS_RHEA = 764.0;
const float RADIUS_TETHYS = 531.0;
const float RADIUS_TITAN = 2575.0;

const float ORBIT_DIONE = 37740.0;
const float ORBIT_ENCELADUS = 23795.0;
const float ORBIT_IAPETUS = 356082.0;
const float ORBIT_RHEA = 52711.0;
const float ORBIT_TETHYS = 29462.0;
const float ORBIT_TITAN = 122187.0;

const float PERIOD_DIONE = 4.1;
const float PERIOD_ENCELADUS = 0.2;
const float PERIOD_IAPETUS = 0.0;
const float PERIOD_RHEA = 3.1;
const float PERIOD_TETHYS = 1.7;
const float PERIOD_TITAN = 5.6;

// Orbital speeds are 1 / days.
const float SPEED_DIONE = 1.0 / 2.74 * i_PI * 2.0;
const float SPEED_ENCELADUS = 1.0 / 1.37 * i_PI * 2.0;
const float SPEED_IAPETUS = 1.0 / 79.3 * i_PI * 2.0;
const float SPEED_RHEA = 1.0 / 4.52 * i_PI * 2.0;
const float SPEED_TETHYS = 1.0 / 1.89 * i_PI * 2.0;
const float SPEED_TITAN = 1.0 / 15.9 * i_PI * 2.0;

in vec3 direction;

out vec4 output_color;

float sdf_saturn(vec3 pos)
{
  // Saturn is in origo.
  return sdf_sphere(pos, vec3(0.0), RADIUS_SATURN);
}

float sdf_trail(vec3 pos)
{
  // Trail orbits saturn.
  return sdf_sphere(pos, vec3(0.0), RADIUS_SATURN * 3.0);
}

float sdf_rings(vec3 pos)
{
  float dist_plane = sdf_plane_two_sided(pos, vec3(0.0), SATURN_RINGS_NORMAL, SATURN_RINGS_THICKNESS);
  float dist_shell = sdf_shell(pos, vec3(0.0), SATURN_RINGS_START, SATURN_RINGS_END);
  return max(dist_plane, dist_shell);
}

float planetary_time()
{
  return uniform_array[4].x * 0.0002;
}

vec3 center_dione()
{
  float period = PERIOD_DIONE + planetary_time() * SPEED_DIONE;
  return vec3(sin(period), 0.0, cos(period)) * ORBIT_DIONE;
}

vec3 center_enceladus()
{
  float period = PERIOD_ENCELADUS + planetary_time() * SPEED_ENCELADUS;
  return vec3(sin(period), 0.0, cos(period)) * ORBIT_ENCELADUS;
}

vec3 center_iapetus()
{
  float period = PERIOD_IAPETUS + planetary_time() * SPEED_IAPETUS;
  return vec3(sin(period), 0.0, cos(period)) * ORBIT_IAPETUS;
}

vec3 center_rhea()
{
  float period = PERIOD_RHEA + planetary_time() * SPEED_RHEA;
  return vec3(sin(period), 0.0, cos(period)) * ORBIT_RHEA;
}

vec3 center_tethys()
{
  float period = PERIOD_TETHYS + planetary_time() * SPEED_TETHYS;
  return vec3(sin(period), 0.0, cos(period)) * ORBIT_TETHYS;
}

vec3 center_titan()
{
  float period = PERIOD_TITAN + planetary_time() * SPEED_TITAN;
  return vec3(sin(period), 0.0, cos(period)) * ORBIT_TITAN;
}

float sdf_dione(vec3 pos)
{
  return sdf_sphere(pos, center_dione(), RADIUS_DIONE);
}

float sdf_enceladus(vec3 pos)
{
  return sdf_sphere(pos, center_enceladus(), RADIUS_ENCELADUS);
}

float sdf_iapetus(vec3 pos)
{
  return sdf_sphere(pos, center_iapetus(), RADIUS_IAPETUS);
}

float sdf_rhea(vec3 pos)
{
  return sdf_sphere(pos, center_rhea(), RADIUS_RHEA);
}

float sdf_tethys(vec3 pos)
{
  return sdf_sphere(pos, center_tethys(), RADIUS_TETHYS);
}

float sdf_titan(vec3 pos)
{
  return sdf_sphere(pos, center_titan(), RADIUS_TITAN);
}

float sdf_orbital(vec3 pos)
{
  return sdf_plane_two_sided(pos, vec3(0.0), vec3(0.0, 1.0, 0.0), ORBITAL_THICKNESS);
}

float sdf_solarplane(vec3 pos)
{
  return sdf_plane_two_sided(pos, vec3(0.0), SOLAR_PLANE, ORBITAL_THICKNESS);
}

float sdf_path(vec3 pos)
{
  return sdf_plane_two_sided(pos, vec3(0.0), light, ORBITAL_THICKNESS);
}

vec3 simple_color(vec3 color, float luminance)
{
  return (luminance * 0.2 + 0.8) * color;
}

vec4 color_orbit(vec3 pos, float period, float speed, vec3 color)
{
  const float SPEED_MUL = 1.9;
  const float OPACITY = 0.34;
  float adjusted_speed = speed * SPEED_MUL;
  float curr = (congr(atan(pos.x, pos.z) - period, i_PI * 2.0) - i_PI * 2 + adjusted_speed) / adjusted_speed;
  return max(vec4(color * OPACITY, OPACITY) * curr, vec4(0.0));
}

vec4 color_orbital(vec3 pos)
{
  float len = length(pos.xz);

  float diff = (len - ORBIT_DIONE) / RADIUS_DIONE;
  if(abs(diff) <= 1.0)
  {
    float period = PERIOD_DIONE + planetary_time() * SPEED_DIONE;
    return color_orbit(pos, period, SPEED_DIONE, COLOR_DIONE);
  }

  diff = (len - ORBIT_ENCELADUS) / RADIUS_ENCELADUS;
  if(abs(diff) <= 1.0)
  {
    float period = PERIOD_ENCELADUS + planetary_time() * SPEED_ENCELADUS;
    return color_orbit(pos, period, SPEED_ENCELADUS, COLOR_ENCELADUS);
  }

  diff = (len - ORBIT_IAPETUS) / RADIUS_IAPETUS;
  if(abs(diff) <= 1.0)
  {
    float period = PERIOD_IAPETUS + planetary_time() * SPEED_IAPETUS;
    return color_orbit(pos, period, SPEED_IAPETUS, COLOR_IAPETUS);
  }

  diff = (len - ORBIT_RHEA) / RADIUS_RHEA;
  if(abs(diff) <= 1.0)
  {
    float period = PERIOD_RHEA + planetary_time() * SPEED_RHEA;
    return color_orbit(pos, period, SPEED_RHEA, COLOR_RHEA);
  }

  diff = (len - ORBIT_TETHYS) / RADIUS_TETHYS;
  if(abs(diff) <= 1.0)
  {
    float period = PERIOD_TETHYS + planetary_time() * SPEED_TETHYS;
    return color_orbit(pos, period, SPEED_TETHYS, COLOR_TETHYS);
  }

  diff = (len - ORBIT_TITAN) / RADIUS_TITAN;
  if(abs(diff) <= 1.0)
  {
    float period = PERIOD_TITAN + planetary_time() * SPEED_TITAN;
    return color_orbit(pos, period, SPEED_TITAN, COLOR_TITAN);
  }

  return vec4(0);
}

vec4 color_measurement(vec3 pos, float dist, float offset)
{
  float OPACITY = 0.48;
  vec4 COLOR_PATH = vec4(0.1, 0.6, 0.7, 1.0);
  float STEPPING = 19000.0;
  float VSTEPPING = 1000.0;

  float mag = abs(dist);
  mag = mag * mag * min(mag, 333.0) * 0.00000008;
  dist = sign(dist) * mag;

  float str = smoothstep(0.97, 1.0, max(1.0 - abs(offset / RADIUS_SATURN), 0.0));

  float hstr = 1.0 - abs(congr(dist + planetary_time() * 33333.0, STEPPING) - STEPPING * 0.5) / (STEPPING * 0.5);
  float str2 = smoothstep(0.99 - min(hstr * 0.05, 0.05), 1.0, hstr);

  float vstr = abs(congr(offset, VSTEPPING) - VSTEPPING * 0.5) / (VSTEPPING * 0.5);
  float str3 = smoothstep(0.66, 1.0, vstr) * smoothstep(0.4 + abs(offset * 0.000012), 1.0, hstr);

  str2 = min(max(str2, str3), 1.0 - abs(offset / (STEPPING * 2.0)));

  return COLOR_PATH * OPACITY * max(str, str2) * (1.0 - smoothstep(22222.0, 666666.0, abs(dist)));
}

vec4 color_solarplane(vec3 pos)
{
  float dist = dot(pos, LINJA);

  return color_measurement(pos, dist, pos.y * 2.0);
}

vec4 color_path(vec3 pos)
{
  float dist = dot(pos, LINJA);

  return color_measurement(pos, dist, pos.y);
}

#define SDF_BISECT_FUNC(bisect_func, sdf_func) vec3 bisect_func(vec3 prev, vec3 next) { vec3 mid = mix(prev, next, 0.5); for(int ii = 0; (ii < 13); ++ii) { float dist = sdf_func(mid); if(dist < 0.0) { next = mid; } else { prev = mid; } mid = (prev + next) * 0.5; } return mid; }
SDF_BISECT_FUNC(sdf_bisect_saturn, sdf_saturn)
SDF_BISECT_FUNC(sdf_bisect_rings, sdf_rings)
SDF_BISECT_FUNC(sdf_bisect_orbital, sdf_orbital)
SDF_BISECT_FUNC(sdf_bisect_solarplane, sdf_solarplane)
SDF_BISECT_FUNC(sdf_bisect_path, sdf_path)
SDF_BISECT_FUNC(sdf_bisect_trail, sdf_trail)
SDF_BISECT_FUNC(sdf_bisect_dione, sdf_dione)
SDF_BISECT_FUNC(sdf_bisect_enceladus, sdf_enceladus)
SDF_BISECT_FUNC(sdf_bisect_iapetus, sdf_iapetus)
SDF_BISECT_FUNC(sdf_bisect_rhea, sdf_rhea)
SDF_BISECT_FUNC(sdf_bisect_tethys, sdf_tethys)
SDF_BISECT_FUNC(sdf_bisect_titan, sdf_titan)

void main()
{
  vec3 dir = normalize(direction);
  vec3 pos = uniform_array[0];
  vec3 pos_prev = pos; // Must work if start inside object.
  float dist_prev = 0.0; // Must work if starting inside object.
  vec4 solid_color = vec4(0.0);
  vec4 blend_color = vec4(0.0);
  int trail_found = 0;
  bool rings_found = false;
  bool orbital_found = false;
  bool solarplane_found = false;
  bool path_found = false;

  for(int ii = 0; (ii < 222); ++ii)
  {
    float dist_saturn = sdf_saturn(pos);
    float dist_rings = 0.0;
    float dist_orbital = 0.0;
    float dist_solarplane = 0.0;
    float dist_path = 0.0;
    float dist_trail = 0.0;
    float dist_dione = sdf_dione(pos);
    float dist_enceladus = sdf_enceladus(pos);
    float dist_iapetus = sdf_iapetus(pos);
    float dist_rhea = sdf_rhea(pos);
    float dist_tethys = sdf_tethys(pos);
    float dist_titan = sdf_titan(pos);
    float dist = min(min(min(min(min(min(dist_dione, dist_enceladus), dist_iapetus), dist_rhea), dist_tethys), dist_titan), dist_saturn);

    if(!rings_found)
    {
      dist_rings = sdf_rings(pos);
      dist = min(dist, dist_rings);
    }
    if(!orbital_found)
    {
      dist_orbital = sdf_orbital(pos);
      dist = min(dist, dist_orbital);
    }
    if(!solarplane_found)
    {
      dist_solarplane = sdf_solarplane(pos);
      dist = min(dist, dist_solarplane);
    }
    if(!path_found)
    {
      dist_path = sdf_path(pos);
      dist = min(dist, dist_path);
    }
    if(trail_found == 0)
    {
      dist_trail = sdf_trail(pos);
      dist = min(dist, dist_trail);
    }
    if(trail_found == 1)
    {
      dist_trail = -sdf_trail(pos);
      dist = min(dist, dist_trail);
    }

    if(dist > SDF_CUTOFF)
    {
      break;
    }

    if(dist_saturn < 0.0)
    {
      pos_prev = sdf_nearify(pos, pos_prev, dir, dist_prev, RADIUS_SATURN);
      pos = sdf_bisect_saturn(pos_prev, pos);

      float luminosity = dot(normalize(pos), light);

      solid_color = vec4(simple_color(COLOR_SATURN, luminosity), 1.0);
      break;
    }
    else if((dist_orbital < 0.0) && !orbital_found)
    {
      pos = sdf_bisect_orbital(pos_prev, pos);

      vec4 col = color_orbital(pos);
      blend_color = (1.0 - blend_color.a) * col + blend_color;

      orbital_found = true;
    }
    else if((dist_solarplane < 0.0) && !solarplane_found)
    {
      pos = sdf_bisect_solarplane(pos_prev, pos);

      vec4 col = color_solarplane(pos);
      blend_color = (1.0 - blend_color.a) * col + blend_color;

      solarplane_found = true;
    }
    else if((dist_path < 0.0) && !path_found)
    {
      pos = sdf_bisect_path(pos_prev, pos);

      vec4 col = color_path(pos);
      blend_color = (1.0 - blend_color.a) * col + blend_color;

      path_found = true;
    }
    else if((dist_trail < 0.0) && (trail_found < 2))
    {
      pos = sdf_bisect_trail(pos_prev, pos);

      float ctime = (uniform_array[4].x + 1111.0) * 0.0000555;
      float str = texture(trail, pos).r;
      str = smoothstep(ctime - 0.06, ctime, str) * smoothstep(-ctime - 0.008, -ctime, -str) * floor(mod(str * 666.0, 2.0));
      vec4 col = vec4(vec3(0.9, 0.4, 0.3) * str * 8.0, str);

      blend_color = (1.0 - blend_color.a) * col + blend_color;

      ++trail_found;
    }
    else if((dist_rings < 0.0) && !rings_found)
    {
      vec3 mid = sdf_bisect_rings(pos_prev, pos);
      float i_ring_tex_pos = (length(mid) - SATURN_RINGS_START) / (SATURN_RINGS_END - SATURN_RINGS_START);

      float deriv = (1.0 - abs(dot(dir, SATURN_RINGS_NORMAL))) / 444.0 + length(mid - uniform_array[0]) / 2222222.0;
      vec4 ring_color = textureGrad(saturn_rings, vec2(i_ring_tex_pos, 0.0), vec2(deriv), vec2(deriv));
      ring_color.rgb = vec3(color_to_luma(ring_color.rgb) * COLOR_RINGS);

      blend_color = (1.0 - blend_color.a) * ring_color + blend_color;

      rings_found = true;
    }
    else if(dist_dione < 0.0)
    {
      pos_prev = sdf_nearify(pos, pos_prev, dir, dist_prev, RADIUS_DIONE);
      pos = sdf_bisect_dione(pos_prev, pos);

      float luminosity = dot(normalize(pos - center_dione()), light);

      solid_color = vec4(simple_color(COLOR_DIONE, luminosity), 1.0);
      break;
    }
    else if(dist_enceladus < 0.0)
    {
      pos_prev = sdf_nearify(pos, pos_prev, dir, dist_prev, RADIUS_ENCELADUS);
      pos = sdf_bisect_enceladus(pos_prev, pos);

      float luminosity = dot(normalize(pos - center_enceladus()), light);

      solid_color = vec4(simple_color(COLOR_ENCELADUS, luminosity), 1.0);
      break;
    }
    else if(dist_iapetus < 0.0)
    {
      pos_prev = sdf_nearify(pos, pos_prev, dir, dist_prev, RADIUS_IAPETUS);
      pos = sdf_bisect_iapetus(pos_prev, pos);

      float luminosity = dot(normalize(pos - center_iapetus()), light);

      solid_color = vec4(simple_color(COLOR_IAPETUS, luminosity), 1.0);
      break;
    }
    else if(dist_rhea < 0.0)
    {
      pos_prev = sdf_nearify(pos, pos_prev, dir, dist_prev, RADIUS_RHEA);
      pos = sdf_bisect_rhea(pos_prev, pos);

      float luminosity = dot(normalize(pos - center_rhea()), light);

      solid_color = vec4(simple_color(COLOR_RHEA, luminosity), 1.0);
      break;
    }
    else if(dist_tethys < 0.0)
    {
      pos_prev = sdf_nearify(pos, pos_prev, dir, dist_prev, RADIUS_TETHYS);
      pos = sdf_bisect_tethys(pos_prev, pos);

      float luminosity = dot(normalize(pos - center_tethys()), light);

      solid_color = vec4(simple_color(COLOR_TETHYS, luminosity), 1.0);
      break;
    }
    else if(dist_titan < 0.0)
    {
      pos_prev = sdf_nearify(pos, pos_prev, dir, dist_prev, RADIUS_TITAN);
      pos = sdf_bisect_titan(pos_prev, pos);

      float luminosity = dot(normalize(pos - center_titan()), light);

      solid_color = vec4(simple_color(COLOR_TITAN, luminosity), 1.0);
      break;
    }

    pos_prev = pos;
    dist_prev = dist;
    pos += dir * (abs(dist) + SDF_DIST_OVERSHOOT);
  }

#if 0
  float ns = floor((texture(noise_volume, dir * 0.55).r - 0.5) * 8.0) / 8.0;
  solid_color.rgb *= 1.0 + ns * 0.4;
#endif

  output_color = solid_color * (1.0 - blend_color.a) + blend_color;
}
