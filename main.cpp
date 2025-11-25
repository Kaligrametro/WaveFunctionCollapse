#include <iostream>
#include "WFC.hpp"
#include "BMP_reader.hpp"
#include "Timer.hpp"

int main()
{
    Timer timer("main.cpp Runtime: ");
    
    const std::string filename = "samples/sample(0).bmp";
    bmp::Bitmap bmp(filename);
    WFC<bmp::Pixel, 2, 3, 512> wfc(bmp.getData(), { bmp.height(), bmp.width() });
    wfc.run({ 10,10 });

    return 0;
}
