layout(location=5) uniform sampler2D fbo;
layout(location=7) uniform sampler2D noise_soft;
layout(location=8) uniform sampler3D noise_volume;
layout(location=9) uniform samplerCube space;
layout(location=10) uniform vec2[96] distorts;

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
  vec4 bg = vec4(texture(space, dir).rgb * 0.4, 1.0);

  float pixelation = 1.0;

  for(int ii = 0; ii < 96; ++ii)
  {
    float dist = pow(smoothstep(0.95, 1.0, 1.0 - abs(texcoord.x - distorts[ii].x * 122.0)), 0.1);
    pixelation = max(pixelation, dist * 22.0);
  }

  vec2 tx = floor(texcoord / pixel_size / pixelation) * pixelation * pixel_size;
  vec2 txNE = floor(texcoordNE / pixel_size / pixelation) * pixelation * pixel_size;
  vec2 txNW = floor(texcoordNW / pixel_size / pixelation) * pixelation * pixel_size;
  vec2 txSW = floor(texcoordSW / pixel_size / pixelation) * pixelation * pixel_size;
  vec2 txSE = floor(texcoordSE / pixel_size / pixelation) * pixelation * pixel_size;

  // FXAA only on foreground.
  vec4 fg = fxaa(fbo, pixel_size, tx, txNE, txNW, txSW, txSE);

  float bmul = floor(mod(texcoord.y / pixel_size.y, 2.0)) * 0.06;
  fg += min(vhbloom(fbo, pixel_size, texcoord) * bmul, vec4(0.4));

  float ns = texture(noise_volume, dir * 8.0).r;
  vec4 mg = vec4(ns * 0.2 * (length(texcoord - 0.5)));

  vec4 cg = (1.0 - mg.a) * bg + mg;

  output_color = (1.0 - fg.a) * cg + fg;
}
