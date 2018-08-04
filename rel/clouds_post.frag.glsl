layout(location=0) uniform vec3[5] uniform_array;
layout(location=5) uniform sampler2D fbo;
layout(location=7) uniform sampler2D noise_soft;
layout(location=9) uniform samplerCube space;

in vec3 direction;
in vec2 pixel_size;
in vec2 texcoord;
in vec2 texcoordNE;
in vec2 texcoordNW;
in vec2 texcoordSW;
in vec2 texcoordSE;

out vec4 output_color;

const float BLOOM_START_TIME = 14000.0;
const float HEAVY_BLOOM_END_TIME = 27000.0;
const float LIGHT_BLOOM_END_TIME = 21000.0;

// I don't get it, this should exist
mat2 mix_mat2(mat2 in1, mat2 in2, float frac)
{
  return (1-frac)*in1 + frac*in2;
}

// Fake bloom.
vec3 rbloom(sampler2D tex, vec2 pixel_size, vec2 texcoord, int negh, int posh, int negv, int posv, mat2 dir, float limit, float mul)
{
  vec3 sum = vec3(0.0);

  for(int ii = -negh; ii <= posh; ++ii)
  {
    float imul;
    if(ii <= 0.0)
    {
      imul = float(ii) / -negh;
    }
    else
    {
      imul = float(ii) / posh;
    }
    imul = 1.0 - imul;

    for(int jj = -negv; jj <= posv; ++jj)
    {
      float jmul;
      if(jj <= 0.0)
      {
        jmul = float(jj) / -negv;
      }
      else
      {
        jmul = float(jj) / posv;
      }
      jmul = (1.0 - jmul) * imul;

      vec2 stepping = vec2(pixel_size.x * ii, pixel_size.y * jj);
      stepping *= dir;
      vec3 col = texture(tex, texcoord + stepping).rgb;
      sum += max(col - limit, vec3(0.0)) * jmul * mul;
    }
  }

  return sum;
}

void main()
{
  vec3 dir = normalize(direction);
  vec3 bg = texture(space, dir).rgb * 0.5;

  // FXAA only on foreground.
  vec4 fg = fxaa(fbo, pixel_size, texcoord, texcoordNE, texcoordNW, texcoordSW, texcoordSE);

  float scene_frac = (uniform_array[4].x - LIGHT_BLOOM_END_TIME) / 6000;

  // Bloom after fg.
  if(uniform_array[4].x >= BLOOM_START_TIME)
  {
    if(uniform_array[4].x < LIGHT_BLOOM_END_TIME)
    {
      fg.rgb += rbloom(fbo, pixel_size, texcoord, 16, 16, 12, 12, mat2(1.0, 0.0, 0.0, 1.0), 0.5, 0.004);
    }
    else if(uniform_array[4].x < HEAVY_BLOOM_END_TIME)
    {
      fg.rgb += rbloom(fbo, pixel_size, texcoord, 4, int(mix(16, 64, scene_frac)), 16, 4, mix_mat2(mat2(0.906, 0.423, -0.423, 0.906), mat2(0.7071, 0.7071, -0.7071, 0.7071), scene_frac) * 2.0, 0.5, 0.03);
    }
  }

  output_color = vec4((1.0 - fg.a) * bg + fg.rgb, 1.0);
}
