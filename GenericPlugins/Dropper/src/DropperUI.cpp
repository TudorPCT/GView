#pragma once

#include "DropperUI.hpp"

constexpr std::string_view BINARY_PAGE_NAME             = "Binary";
constexpr std::string_view OBJECTS_PAGE_NAME            = "Objects";
constexpr std::string_view STRINGS_PAGE_NAME            = "Strings";
constexpr std::string_view FORMAT_INFORMATION_PAGE_NAME = "Format Info";

constexpr int32 BUTTON_ID_CANCEL = 1;
constexpr int32 BUTTON_ID_RUN    = 2;

constexpr int32 RADIO_GROUP_COMPUTATION = 1;
constexpr int32 RADIO_ID_FILE           = 1;
constexpr int32 RADIO_ID_SELECTION      = 2;

constexpr int32 CHECKBOX_ID_OPEN_DROPPED_FILE = 1;
constexpr int32 CHECKBOX_ID_OVERWRITE_FILE    = 2;
constexpr int32 CHECKBOX_ID_APPEND_TO_FILE    = 3;

using namespace AppCUI::Controls;

namespace GView::GenericPlugins::Droppper
{
DropperUI::DropperUI(Reference<GView::Object> object) : Window("Dropper", "d:c,w:100,h:30", WindowFlags::None)
{
    this->object = object;
    if (!this->instance.Init(object)) {
        Dialogs::MessageBox::ShowError("Dropper", "Failed to initialize dropper!");
        this->Exit(Dialogs::Result::Cancel);
    }

    this->tab = Factory::Tab::Create(this, "l:1,t:1,r:1,b:6", TabFlags::TopTabs | TabFlags::TabsBar);
    auto tpb  = Factory::TabPage::Create(this->tab, BINARY_PAGE_NAME);
    auto tpo  = Factory::TabPage::Create(this->tab, OBJECTS_PAGE_NAME);
    auto tps  = Factory::TabPage::Create(this->tab, STRINGS_PAGE_NAME);
    auto tpf  = Factory::TabPage::Create(this->tab, FORMAT_INFORMATION_PAGE_NAME);

    LocalUnicodeStringBuilder<1024> lusb;

    /* init binary tab page area */

    lusb.Set(object->GetName());
    lusb.Add(".drop");

    Factory::Label::Create(tpb, "Description: drop selection(s) to a file (overwrite or append)", "x:2%,y:1,w:97%");

    Factory::Label::Create(tpb, "Filename", "x:2%,y:3,w:13%");
    this->binaryFilename = Factory::TextField::Create(tpb, lusb, "x:15%,y:3,w:84%");

    Factory::Label::Create(tpb, "CharSet to include (a-z,\\x01-\\x05)", "x:2%,y:5,w:97%");
    this->includedCharset = Factory::TextField::Create(tpb, "\\x00-\\xff", "x:2%,y:6,w:97%");

    Factory::Label::Create(tpb, "CharSet to exclude (a-z,\\x01-\\x05)", "x:2%,y:8,w:97%");
    this->excludedCharset = Factory::TextField::Create(tpb, "", "x:2%,y:9,w:97%");

    this->checkboxOpenDroppedFile = Factory::CheckBox::Create(tpb, "Open &dropped file", "x:2%,y:11,w:96%", CHECKBOX_ID_OPEN_DROPPED_FILE);
    this->checkboxOverwriteFile   = Factory::CheckBox::Create(tpb, "Over&write file", "x:2%,y:13,w:96%", CHECKBOX_ID_OVERWRITE_FILE);
    this->checkboxAppendToFile    = Factory::CheckBox::Create(tpb, "&Append to file", "x:2%,y:15,w:96%", CHECKBOX_ID_APPEND_TO_FILE);

    this->checkboxOverwriteFile->SetChecked(true);

    /* end binary tab page area */

    computeForFile      = Factory::RadioBox::Create(this, "Compute for the &entire file", "x:1,y:23,w:31", RADIO_GROUP_COMPUTATION, RADIO_ID_FILE);
    computeForSelection = Factory::RadioBox::Create(this, "Compute for the &selection", "x:1,y:24,w:31", RADIO_GROUP_COMPUTATION, RADIO_ID_SELECTION);

    if (this->instance.IsComputingFile()) {
        computeForFile->SetChecked(true);
        computeForSelection->SetEnabled(false);
    } else {
        computeForSelection->SetChecked(true);
    }

    Factory::Button::Create(this, "&Cancel", "x:40%,y:28,a:b,w:12", BUTTON_ID_CANCEL);
    Factory::Button::Create(this, "&Run", "x:60%,y:28,a:b,w:12", BUTTON_ID_RUN);
}

bool DropperUI::OnEvent(Reference<Control> control, Event eventType, int32 ID)
{
    if (Window::OnEvent(control, eventType, ID)) {
        return true;
    }

    switch (eventType) {
    case AppCUI::Controls::Event::WindowClose:
        this->Exit(Dialogs::Result::Cancel);
        return true;
    case AppCUI::Controls::Event::ButtonClicked:
        if (ID == BUTTON_ID_CANCEL) {
            this->Exit(Dialogs::Result::Cancel);
            return true;
        }
        if (ID == BUTTON_ID_RUN) {
            if (instance.Process()) {
                Dialogs::MessageBox::ShowNotification("Dropper", "Objects extracted.");
            } else {
                Dialogs::MessageBox::ShowError("Dropper", "Failed extracting objects!");
            }

            return true;
        }
        break;
    case AppCUI::Controls::Event::CheckedStatusChanged:
        if (ID == RADIO_ID_FILE) {
            this->instance.SetComputingFile(true);
            return true;
        }
        if (ID == RADIO_ID_SELECTION) {
            this->instance.SetComputingFile(false);
            return true;
        }
        break;
    default:
        break;
    }

    return false;
}
} // namespace GView::GenericPlugins::Droppper
