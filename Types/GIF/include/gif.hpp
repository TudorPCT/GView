#pragma once

#include "GView.hpp"
#include <gif_lib.h>

namespace GView
{
namespace Type
{
	namespace GIF
	{
#pragma pack(push, 2)

        const uint8_t IMAGE_DESCRIPTOR_MARKER = 0x2C;
        const uint8_t EXTENSION_MARKER = 0x21;
        const uint8_t GRAPHIC_CONTROL_EXTENSION = 0xF9;
        const uint8_t APPLICATION_EXTENSION = 0xFF;
        const uint8_t COMMENT_EXTENSION = 0xFE;
        const uint8_t PLAIN_TEXT_EXTENSION = 0x01;
        const uint8_t TRAILER = 0x3B;
        const uint8_t IMAGE_DATA_END_MARKER = 0x00;

        struct Header
        {
            char signature[3];
            char version[3];
        };

        struct LogicalScreenDescriptor
        {
            uint16_t width;
            uint16_t height;
            uint8_t packed;
            uint8_t backgroundColorIndex;
            uint8_t pixelAspectRatio;
        };

        struct ColorTable
        {
            uint8_t red;
            uint8_t green;
            uint8_t blue;
        };

        struct ImageDescriptor
        {
            uint16_t left;
            uint16_t top;
            uint16_t width;
            uint16_t height;
            uint8_t packed;
        };

        struct GraphicControlExtension
        {
            uint8_t blockSize;
            uint8_t packed;
            uint16_t delayTime;
            uint8_t transparentColorIndex;
            uint8_t terminator;
        };

        struct ApplicationExtension
        {
            uint8_t blockSize;
            char applicationIdentifier[8];
            char applicationAuthenticationCode[3];
        };

        struct CommentExtension
        {
            uint8_t blockSize;
        };

        struct PlainTextExtension
        {
            uint8_t blockSize;
            uint16_t left;
            uint16_t top;
            uint16_t width;
            uint16_t height;
            uint8_t cellWidth;
            uint8_t cellHeight;
            uint8_t foregroundColorIndex;
            uint8_t backgroundColorIndex;
        };

        struct ImageData
        {
            uint8_t lzwMinimumCodeSize;
        };

        struct Trailer
        {
            uint8_t terminator;
        };

#pragma pack(pop)

        class GIFFile : public TypeInterface, public View::ImageViewer::LoadImageInterface
        {
        public:
            GifFileType* gifFileLib;

            Header header;
            LogicalScreenDescriptor logicalScreenDescriptor;
            std::vector<ImageDescriptor> imageDescriptors;

            Reference<GView::Utils::SelectionZoneInterface> selectionZoneInterface;

        public:
            GIFFile();
            virtual ~GIFFile(){
                if(gifFileLib){
                    int errorCode;
                    DGifCloseFile(gifFileLib, &errorCode);
                    gifFileLib = nullptr;
                }
            }

            bool Update();

            std::string_view GetTypeName() override
            {
                return "GIF";
            }

            void RunCommand(std::string_view) override
            {
            }

            bool LoadImageToObject(Image& img, uint32 index) override;

            uint32 GetSelectionZonesCount() override
            {
                CHECK(selectionZoneInterface.IsValid(), 0, "");
                return selectionZoneInterface->GetSelectionZonesCount();
            }

            TypeInterface::SelectionZone GetSelectionZone(uint32 index) override
            {
                static auto d = TypeInterface::SelectionZone{ 0, 0 };
                CHECK(selectionZoneInterface.IsValid(), d, "");
                CHECK(index < selectionZoneInterface->GetSelectionZonesCount(), d, "");

                return selectionZoneInterface->GetSelectionZone(index);
            }

            bool UpdateKeys(KeyboardControlsInterface* interface) override
            {
                return true;
            }
        };

        namespace Panels
        {
            class Information : public AppCUI::Controls::TabPage
            {
                Reference<GView::Type::GIF::GIFFile> gif;
                Reference<AppCUI::Controls::ListView> general;
                Reference<AppCUI::Controls::ListView> issues;

                void UpdateGeneralInformation();
                void UpdateIssues();
                void RecomputePanelsPositions();

            public:
                Information(Reference<GView::Type::GIF::GIFFile> gif);

                void Update();
                virtual void OnAfterResize(int newWidth, int newHeight) override
                {
                    RecomputePanelsPositions();
                }
            };
        };
    }
 }
 }