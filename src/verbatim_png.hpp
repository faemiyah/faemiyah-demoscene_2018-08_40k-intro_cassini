#ifndef VERBATIM_PNG_HPP
#define VERBATIM_PNG_HPP

#include "verbatim_image_2d_rgba.hpp"
#include "verbatim_uarr.hpp"

#include "png.h"

/// Pointer to data to read.
static const uint8_t* g_png_read_ptr;

/// PNG data reader function.
/// \param data Data to write to.
/// \param length Length to read.
static void png_read_data(png_structp /*png_ptr*/, png_bytep data, png_size_t length)
{
  for(; length; --length)
  {
    *data++ = *g_png_read_ptr++;
  }
}

/// Read a PNG file from memory.
/// \param data 
static Image2DRGBAUptr png_read(const void* data)
{
  png_structp png_ptr = dnload_png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
#if defined(USE_LD)
  if(!png_ptr)
  {
    BOOST_THROW_EXCEPTION(std::runtime_error(std::string("could not create a PNG read struct")));
  }
#endif

  png_infop info_ptr = dnload_png_create_info_struct(png_ptr);
#if defined(USE_LD)
  if(!info_ptr)
  {
    BOOST_THROW_EXCEPTION(std::runtime_error(std::string("could not create a PNG info struct")));
  }
#endif

  // Set read settings before reading info.
  g_png_read_ptr = static_cast<const uint8_t*>(data);
  dnload_png_set_read_fn(png_ptr, NULL, png_read_data);
  dnload_png_set_expand(png_ptr);
  dnload_png_set_tRNS_to_alpha(png_ptr);

  dnload_png_read_info(png_ptr, info_ptr);
  png_uint_32 width;
  png_uint_32 height;
  int color_type;
  png_uint_32 err = dnload_png_get_IHDR(png_ptr, info_ptr, &width, &height, NULL, &color_type, NULL, NULL, NULL);
#if defined(USE_LD)
  if(1 != err)
  {
    std::ostringstream sstr;
    sstr << "png_get_IHDR returned " << err;
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }
#else
  (void)err;
#endif

  unsigned bpp = 3;
  if((color_type == PNG_COLOR_TYPE_GRAY_ALPHA) || (color_type == PNG_COLOR_TYPE_RGB_ALPHA) ||
      dnload_png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
  {
    bpp = 4;
  }

  Image2DRGBAUptr image(new Image2DRGBA(width, height));
  uarr<uint8_t> row(width * bpp);

  for(unsigned ii = 0; (ii < height); ++ii)
  {
    dnload_png_read_row(png_ptr, row.get(), NULL);

    for(unsigned jj = 0; (jj < width); ++jj)
    {
      uint8_t* row_ptr = row.get() + (jj * bpp);
      float rr = static_cast<float>(row_ptr[0]) / 255.0f;
      float gg = static_cast<float>(row_ptr[1]) / 255.0f;
      float bb = static_cast<float>(row_ptr[2]) / 255.0f;
      float aa = (bpp == 4) ? (static_cast<float>(row_ptr[3]) / 255.0f) : 1.0f;

      image->setPixel(jj, height - ii - 1, rr, gg, bb, aa);
    }
  }

#if defined(USE_LD)
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
#endif

  return image;  
}

#endif
