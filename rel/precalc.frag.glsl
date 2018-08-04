layout(location=0) uniform int control;
layout(location=5) uniform sampler2D tex;

const int COMPOSITE = 1;
const int DYE = 2;
const int PRESSURE = 3;
const int VX = 4;
const int VY = 5;

in vec2 texcoord;

out vec4 output_color;

void main()
{
  if(control == COMPOSITE)
  {
    output_color = texture(tex, texcoord) * 0.5 + 0.5;
  }
  else if(control == DYE)
  {
    float output_color_density = sqrt(texture(tex, texcoord).a);
    output_color = vec4(vec3(output_color_density), 1.0);
  }
  else if(control == PRESSURE)
  {
    float pressure = texture(tex, texcoord).b*4;
    output_color = vec4(pressure, -pressure, 0.0, 1.0);
  }
  else if(control==VX)
  {
    float vx = texture(tex, texcoord).r;
    output_color = vec4(vx, -vx, 0.0, 1.0);
  }
  else if(control==VY)
  {
    float vy = texture(tex, texcoord).g;
    output_color = vec4(vy, -vy, 0.0, 1.0);
  }
  // Show texture as-is.
  else
  {
    output_color = texture(tex, texcoord);
  }
}
