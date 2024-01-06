#pragma once

#include <AppCUI/include/AppCUI.hpp>
#include <array>
using AppCUI::uint32;
constexpr uint32 COMMAND_ADD_NEW_TYPE           = 100;
constexpr uint32 COMMAND_ADD_SHOW_FILE_CONTENT  = 101;
constexpr uint32 COMMAND_EXPORT_ASM_FILE        = 102;
constexpr uint32 COMMAND_JUMP_BACK              = 103;
constexpr uint32 COMMAND_JUMP_FORWARD           = 104;
constexpr uint32 COMMAND_DISSAM_GOTO_ENTRYPOINT = 105;

using AppCUI::int32;
// TODO: reenable
constexpr int32 RIGHT_CLICK_MENU_CMD_NEW    = 0;
constexpr int32 RIGHT_CLICK_MENU_CMD_EDIT   = 1;
constexpr int32 RIGHT_CLICK_MENU_CMD_DELETE = 2;

constexpr int32 RIGHT_CLICK_MENU_CMD_COLLAPSE   = 3;
constexpr int32 RIGHT_CLICK_ADD_COMMENT         = 4;
constexpr int32 RIGHT_CLICK_REMOVE_COMMENT      = 5;
constexpr int32 RIGHT_CLICK_DISSASM_ADD_ZONE    = 6;
constexpr int32 RIGHT_CLICK_DISSASM_REMOVE_ZONE = 7;

namespace GView
{
namespace View
{
    namespace DissasmViewer
    {
        using namespace AppCUI;

        struct Config {
            struct {
                Graphics::ColorPair Normal;
                Graphics::ColorPair Highlight;
                Graphics::ColorPair HighlightCursorLine;
                Graphics::ColorPair Inactive;
                Graphics::ColorPair Cursor;
                Graphics::ColorPair Line;
                Graphics::ColorPair Selection;
                Graphics::ColorPair OutsideZone;
                Graphics::ColorPair StructureColor;
                Graphics::ColorPair DataTypeColor;
                Graphics::ColorPair AsmOffsetColor;                // 0xsomthing
                Graphics::ColorPair AsmIrrelevantInstructionColor; // int3
                Graphics::ColorPair AsmWorkRegisterColor;          // eax, ebx,ecx, edx
                Graphics::ColorPair AsmStackRegisterColor;         // ebp, edi, esi
                Graphics::ColorPair AsmCompareInstructionColor;    // test, cmp
                Graphics::ColorPair AsmFunctionColor;              // ret call
                Graphics::ColorPair AsmLocationInstruction;        // dword ptr[ ]
                Graphics::ColorPair AsmJumpInstruction;            // jmp
                Graphics::ColorPair AsmComment;                    // comments added by user
                Graphics::ColorPair AsmDefaultColor;               // rest of things
                Graphics::ColorPair AsmTitleColor;
                Graphics::ColorPair AsmTitleColumnColor;
            } Colors;

            struct DissasmCommand {
                Input::Key Key;
                const char* Caption;
                const char* Explanation;
                uint32 CommandId;
            };

            inline static DissasmCommand AddNewTypeCommand            = { Input::Key::F6, "AddNewType", "Add new data type", COMMAND_ADD_NEW_TYPE };
            inline static DissasmCommand ShowOrHideFileContentCommand = {
                Input::Key::F9, "ShowOrHideFileContent", "Show or hide file content", COMMAND_ADD_SHOW_FILE_CONTENT
            };
            inline static DissasmCommand AsmExportFileContentCommand = {
                Input::Key::F8, "AsmExportToFile", "Export ASM content to file", COMMAND_EXPORT_ASM_FILE
            };
            inline static DissasmCommand JumpBackCommand    = { Input::Key::Ctrl | Input::Key::Q, "JumpBack", "Jump to previous location", COMMAND_JUMP_BACK };
            inline static DissasmCommand JumpForwardCommand = {
                Input::Key::Ctrl | Input::Key::E, "JumpForward", "Jump to a forward location", COMMAND_JUMP_FORWARD
            };
            inline static DissasmCommand GotoEntrypointCommand = {
                Input::Key::F2, "GoToEntrypoint", "Go to the entry point of the dissasm zone", COMMAND_DISSAM_GOTO_ENTRYPOINT
            };
            inline static DissasmCommand AsmExportFileContent = { Input::Key::F6, "AddNewType", "Add new data type", COMMAND_DISSAM_GOTO_ENTRYPOINT };

            inline static std::array<std::reference_wrapper<DissasmCommand>, 6> CommandBarCommands = {
                AddNewTypeCommand, ShowOrHideFileContentCommand, AsmExportFileContentCommand, JumpBackCommand, JumpForwardCommand, GotoEntrypointCommand
            };
            inline static std::array<std::reference_wrapper<DissasmCommand>, 6> AllKeyboardCommands = {
                AddNewTypeCommand, ShowOrHideFileContentCommand, AsmExportFileContentCommand, JumpBackCommand, JumpForwardCommand, GotoEntrypointCommand
            };
            bool Loaded;

            bool ShowFileContent;
            bool EnableDeepScanDissasmOnStart;
            static void Update(AppCUI::Utils::IniSection sect);
            void Initialize();
        };
    } // namespace DissasmViewer
} // namespace View
} // namespace GView