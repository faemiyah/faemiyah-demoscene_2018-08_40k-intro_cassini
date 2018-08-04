layout(location=5) uniform sampler2D glyph;
layout(location=7) uniform vec3 glyph_color;
layout(location=8) uniform vec3 glyph_inverse_color;
layout(location=9) uniform float glyph_phase;
layout(location=10) uniform vec2[96] distorts;

in vec2 glyph_texcoord;

out vec4 output_color;

void main()
{
  float phase = sqrt(sqrt(sqrt(sqrt(sqrt(1.0 - abs(glyph_phase - 0.5) * 2.0)))));

  float subtract = 0.0;
  for(int ii = 0; (ii < 96); ++ii)
  {
    float diff = 1.0 - abs(distorts[ii].x - glyph_texcoord.y);
    subtract = max(smoothstep(1.0 - (1.0 - phase) * 0.5, 1.0, diff), subtract);
  }

  float outline = texture(glyph, glyph_texcoord).r;

  float intensity = max(outline - subtract, 0.0);

  vec3 gcol = mix(glyph_inverse_color * 2.0, glyph_color, intensity);

  float luma = color_to_luma(gcol);

  vec3 mixed = mix(vec3(1.0 - luma), gcol, outline);

  output_color = vec4(mixed * intensity, intensity);
}
