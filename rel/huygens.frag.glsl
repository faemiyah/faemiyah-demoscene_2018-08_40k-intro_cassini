layout(location=0) uniform vec3[5] uniform_array;

in vec2 pixel_size;
in vec3 screen_coord;

out vec4 output_color;

void main()
{
  float dist = 0.0;
  int figure = int(uniform_array[4].z + 0.1);

  if(figure <= 0)
  {
    float dist_left = sdf_sphere(screen_coord, vec3(-0.6, 0.0, 0.0), 0.15);
    float dist_right = sdf_sphere(screen_coord, vec3(0.6, 0.0, 0.0), 0.15);
    dist = min(sdf_sphere(screen_coord, vec3(0.0), 0.4), min(dist_left, dist_right));
  }
  else if(figure <= 1)
  {
    float dist_left = sdf_sphere(screen_coord, vec3(-0.6, 0.0, 0.0), 0.15);
    float dist_right = sdf_sphere(screen_coord, vec3(0.6, 0.0, 0.0), 0.15);
    dist = min(sdf_sphere(screen_coord, vec3(0.0), 0.4), min(dist_left, dist_right));
  }
  else if(figure <= 2)
  {
    float dist_left = sdf_sphere(screen_coord, vec3(-0.55, 0.0, 0.0), 0.12);
    float dist_right = sdf_sphere(screen_coord, vec3(0.55, 0.0, 0.0), 0.12);
    float dist_center = sdf_sphere(screen_coord, vec3(0.0), 0.4);
    float meta_left = sdf_smin(dist_left, dist_center, 20.0);
    float meta_right = sdf_smin(dist_right, dist_center, 20.0);
    dist = min(meta_right, meta_left);
  }
  else if(figure <= 3)
  {
    float dist_ell = sdf_ellipsoid(screen_coord, vec3(0.0), vec3(0.8, 0.0, 0.0), vec3(0.0, 0.2, 0.0), vec3(0.0, 0.0, 1.0));
    float dist_center = sdf_sphere(screen_coord, vec3(0.0), 0.4);
    float dist_shell1 = sdf_shell(screen_coord, vec3(0.0), 0.4, 0.5);
    float dist_shell2 = sdf_shell(screen_coord, vec3(0.8, 0.0, 0.0), 0.3, 0.4);
    float dist_shell3 = sdf_shell(screen_coord, vec3(-0.8, 0.0, 0.0), 0.3, 0.4);
    dist = min(sdf_subtract(sdf_subtract(sdf_subtract(dist_ell, dist_shell1), dist_shell2), dist_shell3), dist_center);
  }
  else if(figure <= 4)
  {
    float dist_left = sdf_sphere(screen_coord, vec3(-0.5, 0.0, 0.0), 0.15);
    float dist_right = sdf_sphere(screen_coord, vec3(0.5, 0.0, 0.0), 0.15);
    float dist_center = sdf_sphere(screen_coord, vec3(0.0), 0.4);
    float dist_shell = sdf_shell(screen_coord, vec3(0.0), 0.4, 0.48);
    dist = min(dist_center, min(sdf_subtract(dist_left, dist_shell), sdf_subtract(dist_right, dist_shell)));
  }
  else if(figure <= 5)
  {
    float dist_ell = sdf_ellipsoid(screen_coord, vec3(0.0), vec3(0.75, 0.0, 0.0), vec3(0.0, 0.26, 0.0), vec3(0.0, 0.0, 1.0));
    float dist_center = sdf_sphere(screen_coord, vec3(0.0), 0.4);
    float dist_sub = sdf_shell(screen_coord, vec3(0.0), 0.4, 0.5);
    dist = min(sdf_subtract(dist_ell, dist_sub), dist_center);
  }
  else if(figure <= 6)
  {
    float dist_ell = sdf_ellipsoid(screen_coord, vec3(0.0), vec3(0.75, 0.0, 0.0), vec3(0.0, 0.3, 0.0), vec3(0.0, 0.0, 1.0));
    float dist_center = sdf_sphere(screen_coord, vec3(0.0), 0.4);
    float dist_sub = sdf_ellipsoid(screen_coord, vec3(0.0), vec3(0.55, 0.0, 0.0), vec3(0.0, 0.35, 0.0), vec3(0.0, 0.0, 1.0));
    dist = min(sdf_subtract(dist_ell, dist_sub), dist_center);
  }
  else if(figure <= 7)
  {
    float dist_ell = sdf_ellipsoid(screen_coord, vec3(0.0), vec3(0.85, 0.0, 0.0), vec3(0.0, 0.4, 0.0), vec3(0.0, 0.0, 1.0));
    float dist_center = sdf_ellipsoid(screen_coord, vec3(0.0), vec3(0.45, 0.0, 0.0), vec3(0.0, 0.4, 0.0), vec3(0.0, 0.0, 1.0));
    float dist_sub = sdf_ellipsoid(screen_coord, vec3(0.0), vec3(0.6, 0.0, 0.0), vec3(0.0, 0.45, 0.0), vec3(0.0, 0.0, 1.0));
    dist = min(sdf_subtract(dist_ell, dist_sub), dist_center);
  }
  else if(figure <= 8)
  {
    float dist_ell = sdf_ellipsoid(screen_coord, vec3(0.0), vec3(0.82, 0.0, 0.0), vec3(0.0, 0.4, 0.0), vec3(0.0, 0.0, 1.0));
    float dist_center = sdf_sphere(screen_coord, vec3(0.0), 0.4);
    float dist_sub = sdf_ellipsoid(screen_coord, vec3(0.0), vec3(0.62, 0.0, 0.0), vec3(0.0, 0.38, 0.0), vec3(0.0, 0.0, 1.0));
    dist = min(sdf_subtract(dist_ell, dist_sub), dist_center);
  }
  else if(figure <= 9)
  {
    float dist_ell = sdf_ellipsoid(screen_coord, vec3(0.0), vec3(0.8, 0.0, 0.0), vec3(0.0, 0.4, 0.0), vec3(0.0, 0.0, 1.0));
    float dist_center = sdf_sphere(screen_coord, vec3(0.0), 0.4);
    float dist_sub = sdf_ellipsoid(screen_coord, vec3(0.0), vec3(0.57, 0.0, 0.0), vec3(0.0, 0.37, 0.0), vec3(0.0, 0.0, 1.0));
    dist_ell = sdf_subtract(dist_ell, dist_sub);
    dist = min(dist_ell, dist_center);
  }
  else if(figure <= 10)
  {
    float dist_ell = sdf_ellipsoid(screen_coord, vec3(0.0), vec3(0.85, 0.0, 0.0), vec3(0.0, 0.38, 0.0), vec3(0.0, 0.0, 1.0));
    float dist_center = sdf_sphere(screen_coord, vec3(0.0), 0.4);
    float dist_sub = sdf_ellipsoid(screen_coord, vec3(0.0), vec3(0.75, 0.0, 0.0), vec3(0.0, 0.35, 0.0), vec3(0.0, 0.0, 1.0));
    dist = min(sdf_subtract(dist_ell, dist_sub), dist_center);

    // Blur edges.
    if((dist > -0.015) && (dist <= 0.0))
    {
      dist += mod((screen_coord.x + screen_coord.y) * 4.0, min(pixel_size.x, pixel_size.y)) * 25.0;
    }
  }
  else if(figure <= 11)
  {
    float dist_left_pos = sdf_sphere(screen_coord, vec3(-0.4, 0.0, 0.0), 0.47);
    float dist_left_neg = sdf_sphere(screen_coord, vec3(-0.33, 0.0, 0.0), 0.41);
    float dist_right_pos = sdf_sphere(screen_coord, vec3(0.4, 0.0, 0.0), 0.47);
    float dist_right_neg = sdf_sphere(screen_coord, vec3(0.33, 0.0, 0.0), 0.41);
    float dist_left = sdf_subtract(dist_left_pos, dist_left_neg);
    float dist_right = sdf_subtract(dist_right_pos, dist_right_neg);

    dist = min(sdf_sphere(screen_coord, vec3(0.0), 0.4), min(dist_left, dist_right));
  }
  else if(figure <= 12)
  {
    float dist_ell = sdf_ellipsoid(screen_coord, vec3(0.0), vec3(0.8, 0.0, 0.0), vec3(0.0, 0.36, 0.0), vec3(0.0, 0.0, 1.0));
    float dist_left = sdf_sphere(screen_coord, vec3(-0.55, 0.0, 0.0), 0.15);
    float dist_right = sdf_sphere(screen_coord, vec3(0.55, 0.0, 0.0), 0.15);
    dist = sdf_subtract(dist_ell, min(dist_left, dist_right));
  }
  else if(figure <= 13)
  {
    float dist_ell_pos = sdf_ellipsoid(screen_coord, vec3(0.0), vec3(0.81, 0.0, 0.0), vec3(0.0, 0.4, 0.0), vec3(0.0, 0.0, 1.0));
    float dist_ell_neg = sdf_ellipsoid(screen_coord, vec3(0.0), vec3(0.76, 0.0, 0.0), vec3(0.0, 0.36, 0.0), vec3(0.0, 0.0, 1.0));
    float dist_ell = sdf_subtract(dist_ell_pos, dist_ell_neg);
    float dist_left = sdf_sphere(screen_coord, vec3(-0.81, 0.0, 0.0), 0.14);
    float dist_right = sdf_sphere(screen_coord, vec3(0.81, 0.0, 0.0), 0.14);
    dist = min(sdf_sphere(screen_coord, vec3(0.0), 0.4), min(dist_ell, min(dist_left, dist_right)));
  }
  else if(figure <= 14)
  {
    float dist_ell_pos = sdf_ellipsoid(screen_coord, vec3(0.0), vec3(0.9, 0.0, 0.0), vec3(0.0, 0.41, 0.0), vec3(0.0, 0.0, 1.0));
    float dist_ell_neg = sdf_ellipsoid(screen_coord, vec3(0.0), vec3(0.74, 0.0, 0.0), vec3(0.0, 0.32, 0.0), vec3(0.0, 0.0, 1.0));    
    dist = min(sdf_sphere(screen_coord, vec3(0.0), 0.46), sdf_subtract(dist_ell_pos, dist_ell_neg));

    // Add 'in' cross-stitch from Cassini.
    if(dist_ell_neg < 0.0)
    {
      dist = min(dist, 0.01) - mod((screen_coord.x + screen_coord.y) * 4.0, min(pixel_size.x, pixel_size.y)) * 20.0;
    }
  }
  else
  {
    output_color = vec4(0.0);
    return;
  }

  output_color = vec4(float(dist < 0.0));
}
