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

void main()
{
  vec3 dir = normalize(direction);
  vec4 bg;

  float pixelation = 1.0;

  if(uniform_array[4].x < 6000.0)
  {
    pixelation = ceil(mix(14.0, 3.0, uniform_array[4].x / 6000.0));
    float psize = mix(pixel_size.x, pixel_size.y, 0.5);
    bg = vec4(texture(space, floor(direction / psize / pixelation) * pixelation * psize).rgb * 0.5, 1.0);
  }
  else
  {
    bg = vec4(texture(space, dir).rgb * 0.5, 1.0);
  }

  vec2 tx = floor(texcoord / pixel_size / pixelation) * pixelation * pixel_size;
  vec2 txNE = floor(texcoordNE / pixel_size / pixelation) * pixelation * pixel_size;
  vec2 txNW = floor(texcoordNW / pixel_size / pixelation) * pixelation * pixel_size;
  vec2 txSW = floor(texcoordSW / pixel_size / pixelation) * pixelation * pixel_size;
  vec2 txSE = floor(texcoordSE / pixel_size / pixelation) * pixelation * pixel_size;

  // FXAA only on foreground.
  vec4 fg = fxaa(fbo, pixel_size, tx, txNE, txNW, txSW, txSE);
  
  //float bmul = floor(mod(texcoord.y / pixel_size.y, 2.0)) * 0.06;
  vec4 bloom = min(vhbloom(fbo, vec2(0.005, 0.005), texcoord)*0.03, vec4(1.0));
  float time_bloomer_mixer = max(1-abs(uniform_array[4].x - 21000.0)/4000.0, 0.0);
  fg.rgb += mix(vec3(0.0), bloom.rgb, time_bloomer_mixer);

  output_color = (1.0 - fg.a) * bg + fg;

  // Splitter.
  float splitter = smoothstep(-0.01, 0.0, -abs(texcoord.x - uniform_array[4].z));
  if(splitter > 0)
  {
    output_color -= splitter * mod(int(splitter * 8.0), 2);
  }
}
