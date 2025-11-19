#include <iostream>
#include "WFC.hpp"
#include "BMP_reader.hpp"

int main()
{
    const std::string filename = "samples/sample(0).bmp";
    bmp::Bitmap bmp(filename);

    WFC<bmp::Pixel, 2, 7> wfc(bmp.getData(), { bmp.height(), bmp.width() });
    //wfc.run({ 100,100 });

    std::cout << "Compilation success!" << std::endl;
    return 0;
}