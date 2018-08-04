layout(location=0) uniform vec3[5] uniform_array;
layout(location=5) uniform sampler2D fbo;
layout(location=7) uniform sampler2D noise_soft;
layout(location=10) uniform vec2[96] distorts;

in vec2 pixel_size;
in vec2 screen_coord;
in vec2 texcoord;
in vec2 texcoordNE;
in vec2 texcoordNW;
in vec2 texcoordSW;
in vec2 texcoordSE;

out vec4 output_color;

vec2 mapTexcoordDistort(vec2 tx, vec2 center)
{
  float cc = center.x * 77.0;
  float lt = cc - 0.012;
  float rt = cc + 0.012;
  if((tx.x > lt) && (tx.x < rt))
  {
    float phase = (tx.x - lt) / (rt - lt);
    float amp = 1.0 - (phase * 2.0 - 1.0);
    float component = phase * i_PI * 5.0;
    return vec2(tx.x, tx.y + (sin(component) * center.y + sin(component + i_PI) * (1.0 - center.y)) * amp * 0.4);
  }
  return tx;
}

void main()
{
  vec2 tx = texcoord;
  vec2 txNE = texcoordNE;
  vec2 txNW = texcoordNW;
  vec2 txSW = texcoordSW;
  vec2 txSE = texcoordSE;

  for(int ii = 0; ii < 96; ++ii)
  {
    vec2 center = distorts[ii];
    tx = mapTexcoordDistort(tx, center);
    txNE = mapTexcoordDistort(txNE, center);
    txNW = mapTexcoordDistort(txNW, center);
    txSW = mapTexcoordDistort(txSW, center);
    txSE = mapTexcoordDistort(txSE, center);
  }

  vec4 fg = fxaa(fbo, pixel_size, tx, txNE, txNW, txSW, txSE);

  float luma = color_to_luma(fg.rgb);

  float intensity = smoothstep(-2.0, 1.5, sample_noise(noise_soft, screen_coord.xy * 0.00006 + uniform_array[4].z * 0.5));
  intensity *= intensity * intensity * intensity * intensity;

  float figure = uniform_array[4].z * 777.0 + uniform_array[4].y * 0.02;

  vec2 mapped_coord = (mod(screen_coord * (sin((screen_coord.x + figure) * 0.002) * 0.25 + 0.75) * (cos((screen_coord.y - figure) * 0.002) * 0.25 + 0.75), 12.0) - 6.0) / 6.0;
  intensity *= 1.0 - dot(mapped_coord, mapped_coord);

  output_color = vec4(mix(fg.rgb, 1.0 - fg.rgb, intensity), 1.0);
}
