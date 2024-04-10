#include "bmp_image.h"
#include "pack_defines.h"

#include <array>
#include <fstream>
#include <string_view>

#include <iostream>

using namespace std;

namespace img_lib {

PACKED_STRUCT_BEGIN BitmapFileHeader {
    // поля заголовка Bitmap File Header
    char sign[2] = {'B', 'M'};
    uint32_t total_size;
    uint32_t reserved = 0;
    uint32_t data_shift = 54;
}
PACKED_STRUCT_END

PACKED_STRUCT_BEGIN BitmapInfoHeader {
    // поля заголовка Bitmap Info Header
    uint32_t header_size = 40;
    int width;
    int height;
    uint16_t plates = 1;
    uint16_t bits_per_pixel = 24;
    uint32_t compression_type = 0;
    uint32_t data_in_bytes;
    int h_resolution = 11811;
    int v_resolution = 11811;
    int colors_used = 0;
    int colors_significant = 0x1000000;
}
PACKED_STRUCT_END

// функция вычисления отступа по ширине
static int GetBMPStride(int w) {
    return 4 * ((w * 3 + 3) / 4);
}

// напишите эту функцию
bool SaveBMP(const Path& file, const Image& image){
    const int w = image.GetWidth();
    const int h = image.GetHeight();
    BitmapInfoHeader infoHeader;
    infoHeader.width = w;
    infoHeader.height = h;
    infoHeader.data_in_bytes = GetBMPStride(w) * h;
    BitmapFileHeader fileHeader;
    fileHeader.total_size = sizeof(BitmapFileHeader) 
                            + sizeof(BitmapInfoHeader)
                            + infoHeader.data_in_bytes;
    try{
        ofstream out(file, ios::binary | ios::trunc);
        out.write((const char *)&fileHeader, sizeof(BitmapFileHeader));
        out.write((const char *)&infoHeader, sizeof(BitmapInfoHeader));
        vector<char> data(infoHeader.data_in_bytes);
        char* buf = data.data();
        int stride_size = GetBMPStride(w);
        for(int y = 0; y < h; ++y){
            byte* stride = (byte *)buf + y*stride_size;
            const Color* line = image.GetLine(h - y - 1);
            for(int x = 0; x < w ; ++x){
                stride[x*3] = line[x].b;
                stride[x*3 + 1] = line[x].g;
                stride[x*3 + 2] = line[x].r;
            }
        }
        out.write(buf, infoHeader.data_in_bytes);
    } catch (std::exception& e){
        cerr << e.what() << endl;
    }

    return true;
}

// напишите эту функцию
Image LoadBMP(const Path& file){

    BitmapFileHeader fileHeader;
    BitmapInfoHeader infoHeader;
    vector<char> data;
    char* buf = nullptr;
    try{
        ifstream ifs(file, ios::binary);
        ifs.read((char *)&fileHeader,sizeof(BitmapFileHeader));
        ifs.read((char *)&infoHeader,sizeof(BitmapInfoHeader));
        data.resize(infoHeader.data_in_bytes);
        buf = data.data();
        ifs.read(buf, infoHeader.data_in_bytes);
    } catch(std::exception& e){
        cerr << e.what() << endl;
    }
    const int h = infoHeader.height;
    const int w = infoHeader.width;
    Image result{w, h, Color::Black()};
    int stride_size = GetBMPStride(w);
    for(int y = 0; y < h; ++y){
        byte* stride = (byte *)buf + y*stride_size;
        Color* line = result.GetLine(h - y - 1);
        for(int x = 0; x < w ; ++x){
            line[x].b = stride[x*3];
            line[x].g = stride[x*3 + 1];
            line[x].r = stride[x*3 + 2];
            line[x].a = (byte)255;
        }
    }

    return result;
}

}  // namespace img_lib
