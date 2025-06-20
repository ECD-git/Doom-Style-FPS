#include<cmath>
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

void draw_rectangle(std::vector<uint32_t> &image, const size_t imgW, const size_t imgH, const size_t x, const size_t y, const size_t W, const size_t H, const uint32_t color)
{
  assert(image.size() == imgW*imgH);
  // iterate over the dimensions of the rectangle, off set all points by the x and y
  for(size_t i = 0; i < W; i++)
  {
    for(size_t j = 0; j < H; j++)
    {
      // coords of point in rectangle in frame of image
      // x+i, y+j
      size_t cx = x+i;
      size_t cy = y+j;
      assert(cx < imgW && cy < imgH); // check point is in bounds
      image[cx + cy*imgW] = color;
    }
  }
}

int main() 
{
// constants
  const size_t winH = 512;
  const size_t winW = 512; // image dimensions
  std::vector<uint32_t> framebuffer(winW * winH, 255);
  // define as a linear vector of colors
  // we are storing the color of each pixel as a uinit32_t, a 4 byte integer with each byte
  // representing rgb and a. This is a vector of size width*height, initialised to red (255)
  
  const size_t mapH = 16;
  const size_t mapW = 16;
  const char map[] = "0000222222220000"\
                    "1              0"\
                    "1      11111   0"\
                    "1     0        0"\
                    "0     0  1110000"\
                    "0     3        0"\
                    "0   10000      0"\
                    "0   0   11100  0"\
                    "0   0   0      0"\
                    "0   0   1  00000"\
                    "0       1      0"\
                    "2       1      0"\
                    "0       0      0"\
                    "0 0000000      0"\
                    "0              0"\
                    "0002222222200000"; // this map is taken from the tutorial I cba
            
  // game map is saved as a char array, so spaces will be uneffected
  assert(sizeof(map) == mapH*mapW + 1); // +1 for the string termination character
                    
  // this generates the image without the map
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

  // this adds on the map by drawing rectangle on top of the existing fb
  // for each non space point on the map
  const size_t rectW = winW/mapW;
  const size_t rectH = winH/mapH;
  for(int j = 0; j < mapH; j++)
  {
    for(int i = 0; i < mapW; i++)
    {
      if(map[i+(j*mapW)] == ' ')
      {
        std::cout<<'e';
        continue; // skip spaces
      }
      std::cout<<'n';
      size_t rect_y = j*rectH;
      size_t rect_x = i*rectW;
      draw_rectangle(framebuffer, winW, winH, rect_x, rect_y, rectW, rectH, pack_color(0,255,255));
    }
  }

  // draw the plater, coords in map segments
  float player_x = 3;
  float player_y = 2;

  // generates the image from the framebuffer
  gen_ppm_image("./out.ppm", framebuffer, winW, winH);

  std::cout<<"EXITING"<<std::endl;
  return 0;
}