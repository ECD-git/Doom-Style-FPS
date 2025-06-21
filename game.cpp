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

void gen_ppm_image(const std::string filename, const std::vector<uint32_t> &fb, const size_t W, const size_t H, const size_t VW)
{
  assert(fb.size() == (W+VW)*H);
  std::ofstream ofs(filename, std::ios::binary);
  ofs << "P6\n" << W+VW << " " << H << "\n255\n";
  for (size_t i = 0; i < (W+VW)*H; ++i)
  {
    uint8_t r,g,b,a;
    unpack_color(fb[i], r, g, b, a);
    // need  to unpack color here to pass to image pixel
    ofs << static_cast<char>(r) << static_cast<char>(g) << static_cast<char>(b);
  }
  ofs.close();
}

void draw_rectangle(std::vector<uint32_t> &image, const size_t imgW, const size_t viewW, const size_t imgH, const size_t x, const size_t y, const size_t W, const size_t H, const uint32_t color)
{
  assert(image.size() == (imgW+viewW)*imgH);
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
      image[cx + cy*(imgW+viewW)] = color;
    }
  }
}

void draw_player(std::vector<uint32_t> &image, const size_t imgW, const size_t viewW,const size_t imgH, const size_t rectW, const size_t rectH, const float x, const float y, const size_t W, const size_t H, const uint32_t color)
{
  assert(image.size() == (imgW+viewW)*imgH);
  // iterate over dimensions of player
  for (size_t i = 0; i < W; i++)
  {
    for (size_t j = 0; j < H; j++)
    {
      size_t cx = i+(x*rectW); // TODO: maybe add 1 here and make sure in boundaries of wall
      size_t cy = j+(y*rectH);
      assert(cx < imgW && cy < imgH);
      image[cx + cy*(imgW+viewW)] = color;
    }
  }
}

void ray_cast(std::vector<uint32_t> &image, std::vector<float> &casts, const char map[], const size_t imgW, const size_t viewW, const size_t imgH, const size_t rectW, const size_t rectH, const size_t mapW, const size_t mapH, const float x, const float y, const float a, const float fov, const uint32_t color)
{
  assert(image.size() == (imgW+viewW)*imgH);

  int intervals = viewW - 1;
  for(float ang = a-(fov/2); ang<a+(fov/2); ang+=(fov/intervals))
  {
    float c = 0;
    for(float c = 0; c<20; c+=0.05)
    {
      float new_x = x + c*cos(ang);
      float new_y = y + c*sin(ang);
      //std::cout<<map[int(new_x)+ int(new_y)*mapW];
      if (map[int(new_x)+ int(new_y)*mapW] != ' ')
      {
        casts.push_back(c);
        break;
      }
      size_t cx = new_x*rectW;
      size_t cy = new_y*rectH;
      //assert(cx < imgW && cy < imgH);
      image[cx + cy*(imgW + viewW)] = color;
    }
  }

  float bounds[2] = {a-(fov/2), a+(fov/2)};
  for(float ang : bounds)
  {
    float c = 0;
    for(float c = 0; c<2; c+=0.05)
    {
      float new_x = x + c*cos(ang);
      float new_y = y + c*sin(ang);
      //std::cout<<map[int(new_x)+ int(new_y)*mapW];
      if (map[int(new_x)+ int(new_y)*mapW] != ' ')
      {
        break;
      }
      size_t cx = new_x*rectW;
      size_t cy = new_y*rectH;
      //assert(cx < imgW && cy < imgH);
      image[cx + cy*(imgW + viewW)] = pack_color(255,0,0);
    }
  }
}

void draw_view(std::vector<uint32_t> &image, std::vector<float> casts, const size_t winW, const size_t winH, const size_t viewW, const size_t viewH)
{
  assert(casts.size() == viewW);
  for(int j = 0; j < viewW; j++)
  {
    int height = int(viewH/casts[j]);
    std::cout<<height<<std::endl;
    int space = int((viewH-height)/2);
    std::cout<<space<<std::endl;
    std::cout<<height + space*2<<std::endl;
    //assert(height + space*2 == viewH);
    for(int i = 0; i < viewH; i++)
    {
      if(i > space && i < space+height)
      {
        image[i*(winW+viewW) + j + winW] = pack_color(0, 255, 255);
      }
      else
      {
        image[i*(winW+viewW) + j + winW] = pack_color(255, 255, 255);
      }
    }
  }
}

int main() 
{
// constants
  const size_t winH = 512;
  const size_t winW = 512; // map dimensions

  const size_t viewH = 512;
  const size_t viewW = 512;

  std::vector<uint32_t> framebuffer((winW+viewW) * winH, 255);
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
                    "0    0000      0"\
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
                    
  // this generates the mapbackground without the map
  for (size_t irow = 0; irow<winH; irow++) // iterate over all rows
  {
    for (size_t ipixel = 0; ipixel<winW; ipixel++)
    {
      uint8_t r = 255*irow/float(winH); // red increases over height
      uint8_t g = 255*ipixel/float(winW); // increase green over width
      uint8_t b = 0; // no blue variation
      // pack the colors into a single uint32_t to be stored in vector
      framebuffer[ipixel+irow*(winW+viewW)] = pack_color(r,g,b);
    }
  }
  // this generates the background of the view port
  for (size_t i = 0; i<viewH; i++)
  {
    for (size_t j = 0; j<viewW; j++)
    {
      framebuffer[j+winW+i*(winW+viewW)] = pack_color(255,255,255);
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
        //std::cout<<'e';
        continue; // skip spaces
      }
      //std::cout<<'n';
      size_t rect_y = j*rectH;
      size_t rect_x = i*rectW;
      draw_rectangle(framebuffer, winW, viewW, winH, rect_x, rect_y, rectW, rectH, pack_color(0,255,255));
    }
  }

  // draw the plater, coords in num of map segments
  float player_x = 3;
  float player_y = 2;
  float player_a = 1.5; // angle of view to the positive x axis
  const float player_fov = 1; 
  const size_t playerW = 5;
  const size_t playerH = 5;

  draw_player(framebuffer, winW, viewW, winH, rectW, rectH, player_x, player_y, playerW, playerH ,pack_color(255,255,255));
  std::vector<float> casts;
  ray_cast(framebuffer, casts, map, winW, viewW, winH, rectW, rectH, mapW, mapH, player_x, player_y, player_a, player_fov, pack_color(255,255,255));

  std::cout<<casts.size()<<std::endl;

  draw_view(framebuffer, casts, winW, winH, viewW, viewH);

  // generates the image from the framebuffer
  gen_ppm_image("./out.ppm", framebuffer, winW, winH, viewW);

  std::cout<<"EXITING"<<std::endl;
  return 0;
}