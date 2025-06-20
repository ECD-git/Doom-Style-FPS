#include<iostream>
#include<fstream>
#include<vector>
#include<stdint.h>
#include<assert.h>

uint32_t pack_color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a = 255)
{
  return (a<<24) + (b<<16) + (g<<8) + r;
  // packs as aaaaaaaabbbbbbbbggggggggrrrrrrrr in a uint23_t
}
void unpack_color(const uint32_t &color, uint8_t &r, uint8_t &g, uint8_t &b, uint8_t &a)
{
  r = (color >> 0) & 255;
  g = (color >> 8) & 255;
  b = (color >> 16) & 255;
  a = (color >> 24) & 255;
}

void gen_ppm_image(const std::string filename, const std::vector<uint32_t> &fb, const size_t W, const size_t H)
{
  assert(fb.size() == W*H);
  std::ofstream ofs(filename, std::ios::binary);
  ofs << "P6\n" << W << " " << H << "\n255\n";
  for (size_t i = 0; i < W*H; ++i)
  {
    uint8_t r,g,b,a;
    unpack_color(fb[i], r, g, b, a);
    // need  to unpack color here to pass to image pixel
    ofs << static_cast<char>(r) << static_cast<char>(g) << static_cast<char>(b);
  }
  ofs.close();
}

int main() 
{
  const size_t winH = 512;
  const size_t winW = 512; // image dimensions

  std::vector<uint32_t> framebuffer(winW * winH, 255);
  // define as a linear vector of colors
  // we are storing the color of each pixel as a uinit32_t, a 4 byte integer with each byte
  // representing rgb and a. This is a vector of size width*height, initialised to red (255)
  for (size_t irow = 0; irow<winH; irow++) // iterate over all rows
  {
    for (size_t ipixel = 0; ipixel<winW; ipixel++)
    {
      uint8_t r = 255*irow/float(winH); // red increases over height
      uint8_t g = 255*ipixel/float(winW); // increase green over width
      uint8_t b = 0; // no blue variation
      // pack the colors into a single uint32_t to be stored in vector
      framebuffer[ipixel+irow*winW] = pack_color(r,g,b);
    }
  }

  gen_ppm_image("./out.ppm", framebuffer, winW, winH);
  std::cout<<"EXITING"<<std::endl;
  return 0;
}