#include <img_lib.h>
#include <jpeg_image.h>
#include <ppm_image.h>
#include <bmp_image.h>

#include <filesystem>
#include <string_view>
#include <iostream>

using namespace std;

enum class Format{
    UNKNOWN,
    JPEG,
    PPM,
    BMP
};

class ImageFormatInterface {
public:
    virtual bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const = 0;
    virtual img_lib::Image LoadImage(const img_lib::Path& file) const = 0;
};

class ImagePPMInterface: public ImageFormatInterface{
    bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override {
        return img_lib::SavePPM(file, image);
    }
    img_lib::Image LoadImage(const img_lib::Path& file) const override {
        return img_lib::LoadPPM(file);
    }
};

class ImageJPEGInterface: public ImageFormatInterface{
    bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override {
        return img_lib::SaveJPEG(file, image);
    }
    img_lib::Image LoadImage(const img_lib::Path& file) const override {
        return img_lib::LoadJPEG(file);
    }
};

class ImageBMPInterface: public ImageFormatInterface{
    bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override {
        return img_lib::SaveBMP(file, image);
    }
    img_lib::Image LoadImage(const img_lib::Path& file) const override {
        return img_lib::LoadBMP(file);
    }
};

Format GetFormatByExtension(const img_lib::Path& input_file) {
    const string ext = input_file.extension().string();
    if (ext == ".jpg"sv || ext == ".jpeg"sv) {
        return Format::JPEG;
    }

    if (ext == ".ppm"sv) {
        return Format::PPM;
    }

    if (ext == ".bmp"sv) {
        return Format::BMP;
    }

    return Format::UNKNOWN;
}

ImageFormatInterface* GetFormatInterface(const img_lib::Path& path){
    static ImagePPMInterface  PPMInterface;
    static ImageJPEGInterface JPEGInterface;
    static ImageBMPInterface BMPImterface;
    Format format = GetFormatByExtension(path);
    if(format == Format::PPM){ 
        return &PPMInterface;
    } else if(format == Format::JPEG){
        return &JPEGInterface;
    } else if(format == Format::BMP){
        return &BMPImterface;
    } else {
        return nullptr;
    }
}

int main(int argc, const char** argv) {
    if (argc != 3) {
        cerr << "Usage: "sv << argv[0] << " <in_file> <out_file>"sv << endl;
        return 1;
    }

    img_lib::Path in_path = argv[1];
    img_lib::Path out_path = argv[2];

    ImageFormatInterface* in_interface = GetFormatInterface(in_path);
    if(in_interface == nullptr){
        cout << "Unknown format of the input file\n";
        return 2;
    }
    ImageFormatInterface* out_interface = GetFormatInterface(out_path);
    if(out_interface == nullptr){
        cout << "Unknown format of the output file\n";
        return 3;
    }

    // img_lib::Image image = img_lib::LoadPPM(in_path);
    img_lib::Image image = in_interface->LoadImage(in_path);
    if (!image) {
        cerr << "Loading failed"sv << endl;
        return 4;
    }

    // if (!img_lib::SaveJPEG(out_path, image)) {
        if(!out_interface->SaveImage(out_path, image)) {
        cerr << "Saving failed"sv << endl;
        return 5;
    }

    cout << "Successfully converted"sv << endl;
}