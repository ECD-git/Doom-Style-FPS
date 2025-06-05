#include<iostream>
#include<fstream>
#include<vector>
#include<stdint.h>

int main() 
{
  const size_t winH = 512;
  const size_t winW = 512; // image dimensions

  std::vector<uint32_t> framebuffer(winW * winH, 255);
  // we are storing the color of each pixel as a uinit32_t, a 4 byte integer with each byte
  // representing rgb and a. This is a vector of size width*height, initialised to red (255)
  for (size_t irow = 0; irow<winH; irow++) // iterate over all rows
  {
    for (size_t ipixel = 0; ipixel<winW; ipixel++)
    {
      break;
    }
  }
}