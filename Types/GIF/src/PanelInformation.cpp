#include "gif.hpp"

using namespace GView::Type::GIF;
using namespace AppCUI::Controls;

Panels::Information::Information(Reference<GView::Type::GIF::GIFFile> _gif) : TabPage("&Information")
{
    gif     = _gif;
    general = Factory::ListView::Create(this, "x:0,y:0,w:100%,h:10", { "n:Field,w:25", "n:Value,w:100" }, ListViewFlags::None);

    issues = Factory::ListView::Create(this, "x:0,y:21,w:100%,h:10", { "n:Info,w:200" }, ListViewFlags::HideColumns);

    this->Update();
}

void Panels::Information::UpdateGeneralInformation()
{
    LocalString<256> tempStr;
    NumericFormatter n;

    general->DeleteAllItems();
    general->AddItem("File");
    general->AddItem({ "Size", tempStr.Format("%s bytes", n.ToString(gif->obj->GetData().GetSize(), { NumericFormatFlags::None, 10, 3, ',' }).data()) });

    general->AddItem("Logical Screen Descriptor");
    const auto width  = Endian::LittleToNative(gif->logicalScreenDescriptor.width);
    const auto height = Endian::LittleToNative(gif->logicalScreenDescriptor.height);
    general->AddItem({ "Size", tempStr.Format("%u x %u", width, height) });

    bool hasGlobalColorTable = (gif->logicalScreenDescriptor.packed & 0x80) != 0;
    uint8_t colorResolution = (gif->logicalScreenDescriptor.packed & 0x70) >> 4;
    bool isSorted = (gif->logicalScreenDescriptor.packed & 0x08) != 0;
    uint8_t globalColorTableSize = (gif->logicalScreenDescriptor.packed & 0x07);
    uint16_t gctEntries = 1 << (globalColorTableSize + 1);

    general->AddItem({ "Global Color Table", hasGlobalColorTable ? "Yes" : "No" });

    general->AddItem({ "Color Resolution", tempStr.Format("%u bits", colorResolution + 1) });

    general->AddItem({ "GCT Sorted", isSorted ? "Yes" : "No" });

    general->AddItem({ "GCT Entries", tempStr.Format("%u", gctEntries) });

    general->AddItem({ "Background Color Index", tempStr.Format("%u", gif->logicalScreenDescriptor.backgroundColorIndex) });

    general->AddItem({ "Pixel Aspect Ratio", tempStr.Format("%u", gif->logicalScreenDescriptor.pixelAspectRatio) });

}


void Panels::Information::UpdateIssues()
{
}

void Panels::Information::RecomputePanelsPositions()
{
    int py = 0;
    int w  = this->GetWidth();
    int h  = this->GetHeight();

    if ((!general.IsValid()) || (!issues.IsValid())){
        return;
    }

    issues->SetVisible(false);
    this->general->Resize(w, h);
}

void Panels::Information::Update()
{
    UpdateGeneralInformation();
    UpdateIssues();
    RecomputePanelsPositions();
}
