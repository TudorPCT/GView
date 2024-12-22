#include "gif.hpp"

using namespace AppCUI;
using namespace AppCUI::Utils;
using namespace AppCUI::Application;
using namespace AppCUI::Controls;
using namespace GView::Utils;
using namespace GView::Type;
using namespace GView;
using namespace GView::View;

extern "C"
{
    PLUGIN_EXPORT bool Validate(const AppCUI::Utils::BufferView& buf, const std::string_view& extension)
    {
        if (buf.GetLength() < sizeof(GIF::Header) + sizeof(GIF::LogicalScreenDescriptor)) {
            return false;
        }

        auto header = buf.GetObject<GIF::Header>();
        if (memcmp(header->signature, "GIF", 3) != 0 ||
            (memcmp(header->version, "87a", 3) != 0 && memcmp(header->version, "89a", 3) != 0)) {
            return false;
        }

        auto screenDesc = buf.GetObject<GView::Type::GIF::LogicalScreenDescriptor>(sizeof(GView::Type::GIF::Header));
        if (screenDesc->width == 0 || screenDesc->height == 0) {
            return false;
        }

        return true;
    }

    PLUGIN_EXPORT TypeInterface* CreateInstance()
    {
        return new GIF::GIFFile;
    }

    void CreateBufferView(Reference<GView::View::WindowInterface> win, Reference<GIF::GIFFile> gif)
    {
        auto findEndBlockOffset = [](GView::Utils::DataCache* data, uint64& offset, uint64 dataSize) -> void {
            while (offset < dataSize) {
                uint8 blockSize;

                if (!data->Copy<uint8>(offset, blockSize)) {
                    offset = static_cast<uint64>(-1); // Explicit cast for type safety
                    return;
                }

                if (blockSize == 0) {
                    break;
                }

                offset += blockSize + 1;
            }
        };

        BufferViewer::Settings settings;

        const std::vector<ColorPair> colors = { ColorPair{ Color::Teal, Color::DarkBlue }, ColorPair{ Color::Yellow, Color::DarkBlue } };

        auto& data            = gif->obj->GetData();
        const uint64 dataSize = data.GetSize();
        uint64 offset         = 0;
        uint64 frameIndex     = 0;

        bool hasGlobalColorTable = (gif->logicalScreenDescriptor.packed & 0x80) != 0;
        uint8_t colorResolution = (gif->logicalScreenDescriptor.packed & 0x70) >> 4;
        bool isSorted = (gif->logicalScreenDescriptor.packed & 0x08) != 0;
        uint8_t globalColorTableSize = (gif->logicalScreenDescriptor.packed & 0x07);
        uint16_t gctEntries = 1 << (globalColorTableSize + 1);

        settings.AddZone(0, sizeof(GIF::Header), ColorPair{ Color::Red, Color::DarkBlue }, "Header");
        offset += sizeof(GIF::Header);

        settings.AddZone(offset, sizeof(GIF::LogicalScreenDescriptor), ColorPair{ Color::Green, Color::DarkBlue }, "LogicalScreenDescriptor Segment");
        offset += sizeof(GIF::LogicalScreenDescriptor);

        settings.AddZone(offset, gctEntries * sizeof(GIF::ColorTable) - 1, ColorPair{ Color::Magenta, Color::DarkBlue }, "Global Color Table");
        offset += gctEntries * sizeof(GIF::ColorTable) - 1;

        while (offset < dataSize) {
            uint8 marker_type;
            uint8 extension_marker_type;

            if (!data.Copy<uint8>(offset, marker_type)) {
                return;
            }

            switch (marker_type) {
                case GIF::IMAGE_DESCRIPTOR_MARKER: {
                    uint8_t packedField;

                    settings.AddZone(offset, sizeof(GIF::ImageDescriptor), ColorPair{ Color::Green, Color::DarkBlue }, "Image Descriptor 1");
                    offset += sizeof(GIF::ImageDescriptor);

                    if (offset >= dataSize) {
                        break;
                    }

                    if (!data.Copy<uint8>(offset, packedField)) {
                        return;
                    }

                    bool hasLocalColorTable = packedField & 0x80;

                    if (hasLocalColorTable) {
                        uint8_t localColorTableSize = packedField & 0x07;
                        uint16_t lctEntries = 1 << (localColorTableSize + 1);

                        settings.AddZone(offset, lctEntries * sizeof(GIF::ColorTable), ColorPair{ Color::Magenta, Color::DarkBlue }, "Local Color Table");
                        offset += lctEntries * sizeof(GIF::ColorTable) - 1;
                    }

                    settings.AddZone(offset, 1, ColorPair{ Color::Yellow, Color::DarkBlue }, "LZW Minimum Code Size");
                    offset++;

                    uint64 beginImageData = offset - 1;

                    findEndBlockOffset(&data, offset, dataSize);

                    settings.AddZone(beginImageData, offset - beginImageData + 1, ColorPair{ Color::Red, Color::DarkBlue }, "Image " + std::to_string(frameIndex));
                    frameIndex++;

                    offset++;

                    break;
                }
                case GIF::EXTENSION_MARKER: {
                    if (!data.Copy<uint8>(offset + 1, extension_marker_type)) {
                        return;
                    }

                    switch (extension_marker_type) {
                        case GIF::GRAPHIC_CONTROL_EXTENSION: {
                            settings.AddZone(offset, sizeof(GIF::GraphicControlExtension) + 2, ColorPair{ Color::Pink, Color::DarkBlue }, "Graphic Control Extension");
                            offset += sizeof(GIF::GraphicControlExtension) + 2;
                            break;
                        }
                        case GIF::COMMENT_EXTENSION: {
                            auto beginBlock = offset;
                            offset+=2;

                            findEndBlockOffset(&data, offset, dataSize);

                            offset++;

                            settings.AddZone(beginBlock, offset - beginBlock, ColorPair{ Color::Yellow, Color::DarkBlue }, "Comment Extension");

                            break;
                        }
                        case GIF::APPLICATION_EXTENSION: {
                            auto beginBlock = offset;
                            uint8_t blockSize;
                            if (!data.Copy<uint8>(offset + 2, blockSize)) {
                                return;
                            }
                            offset += 3 + blockSize;

                            findEndBlockOffset(&data, offset, dataSize);

                            offset++;

                            settings.AddZone(beginBlock, offset - beginBlock, ColorPair{ Color::Green, Color::DarkBlue }, "Application Extension");

                            break;
                        }
                        case GIF::PLAIN_TEXT_EXTENSION: {
                            auto beginBlock = offset;
                            uint8_t blockSize;

                            if (!data.Copy<uint8>(offset + 2, blockSize)) {
                                return;
                            }

                            offset += 3 + blockSize;

                            findEndBlockOffset(&data, offset, dataSize);

                            offset++;

                            settings.AddZone(offset, offset - beginBlock, ColorPair{ Color::Yellow, Color::DarkBlue }, "Plain Text Extension");

                            break;
                        }
                        default: {
                            offset++;
                        }
                    }

                    break;
                }
                case GIF::TRAILER: {
                    settings.AddZone(offset, 1, ColorPair{ Color::Red, Color::DarkBlue }, "Trailer");
                    goto end;
                }
                default: {
                    offset++;
                }
            }

        }

        end:
        gif->selectionZoneInterface = win->GetSelectionZoneInterfaceFromViewerCreation(settings);
    }

    void CreateImageView(Reference<GView::View::WindowInterface> win, Reference<GIF::GIFFile> gif)
    {
        GView::View::ImageViewer::Settings settings;
        settings.SetLoadImageCallback(gif.ToBase<View::ImageViewer::LoadImageInterface>());
        int frameIndex = 0;

        for(frameIndex = 0; frameIndex < gif->gifFileLib->ImageCount; frameIndex++) {
            settings.AddImage(
                frameIndex,
                gif->gifFileLib->SavedImages[frameIndex].ImageDesc.Width * gif->gifFileLib->SavedImages[frameIndex].ImageDesc.Height * sizeof(Pixel)
            );
        }

        win->CreateViewer(settings);
    }

    PLUGIN_EXPORT bool PopulateWindow(Reference<GView::View::WindowInterface> win)
    {
        auto gif = win->GetObject()->GetContentType<GIF::GIFFile>();
        gif->Update();

        CreateImageView(win, gif);
        CreateBufferView(win, gif);

        win->AddPanel(Pointer<TabPage>(new GIF::Panels::Information(gif)), true);

        return true;
    }

    PLUGIN_EXPORT void UpdateSettings(IniSection sect)
    {
        sect["Pattern"]     = "magic:47 49 46";
        sect["Priority"]    = 1;
        sect["Description"] = "GIF image file (*.gif)";
    }

}

int main()
{
    return 0;
}