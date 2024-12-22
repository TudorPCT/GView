#include "gif.hpp"
#include <codecvt>
using namespace GView::Type::GIF;

static std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;

GIFFile::GIFFile()
{
}

bool GIFFile::Update()
{
    int errorCode;
    auto filePath = convert.to_bytes(obj->GetPath().data(), obj->GetPath().data() + obj->GetPath().size());
    GifFileType* gifFile = DGifOpenFileName(filePath.c_str(), &errorCode);

    if (!gifFile) {
        return false;
    }

    if (DGifSlurp(gifFile) != GIF_OK) {
        DGifCloseFile(gifFile, &errorCode);
        return false;
    }

    this->gifFileLib = gifFile;

    memset(&header, 0, sizeof(header));
    memset(&logicalScreenDescriptor, 0, sizeof(logicalScreenDescriptor));

    auto& data = this->obj->GetData();

    CHECK(data.Copy<Header>(0, header), false, "");
    CHECK(data.Copy<LogicalScreenDescriptor>(sizeof(Header), logicalScreenDescriptor), false, "");

    return true;
}


bool GIFFile::LoadImageToObject(Image& img, uint32_t index) {

    auto gifFileLib = this->gifFileLib;

    int errorCode;

    if (index >= gifFileLib->ImageCount) {
        DGifCloseFile(gifFileLib, &errorCode);
        return false;
    }

    SavedImage& frame = gifFileLib->SavedImages[index];
    const GifImageDesc& imageDesc = frame.ImageDesc;

    int width = imageDesc.Width;
    int height = imageDesc.Height;

    if (!img.Create(width, height)) {
        DGifCloseFile(gifFileLib, &errorCode);
        return false;
    }

    auto* outputBuffer = img.GetPixelsBuffer();
    const ColorMapObject* colorMap = (imageDesc.ColorMap) ? imageDesc.ColorMap : gifFileLib->SColorMap;

    if (!colorMap) {
        DGifCloseFile(gifFileLib, &errorCode);
        return false;
    }

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int pixelIndex = frame.RasterBits[y * width + x];
            if (pixelIndex < colorMap->ColorCount) {
                GifColorType color = colorMap->Colors[pixelIndex];
                size_t pixelOffset = y * width + x;

                outputBuffer[pixelOffset] = Pixel(color.Red, color.Green, color.Blue);
            }
        }
    }

    return true;
}