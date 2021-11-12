#include "gui/debugger/register_view.hpp"
#include "gui/imgui_helpers.hpp"
#include <fmt/format.h>

namespace GUI {

    RegisterView::RegisterView(core::EmuWrapper& e, float fs) : DbgComponent(e, fs) {}

    void RegisterView::draw_window() {
        static const float onesize    = ImGui::CalcTextSize("A").x;
        static const float lhs_width  = ImGui::CalcTextSize("V0").x;
        static const float rhs_width  = ImGui::CalcTextSize("FF").x;
        static const float threesize  = ImGui::CalcTextSize("FFF").x;
        static const float pair_width = lhs_width + rhs_width + ((lhs_width + rhs_width) / 2.0f);

        auto draw_I = [&]() {
            ImGui::TableNextColumn();
            helpers::center_text("I");

            if (emu.is_readable()) {
                ImGui::SameLine();
                ImGui::Selectable("###I", false, ImGuiSelectableFlags_SpanAllColumns);

                if (ImGui::BeginPopupContextItem()) {
                    uint16_t value = emu.get_I() & 0xFFF;
                    if (ImGui::Selectable(
                                fmt::format("Follow to {0:03X} in disassembly", value).c_str())) {
                        message = DbgMessage{ dbg_component::disassembly_view, dbg_action::update,
                                              ScrollMessage{ value, true } };
                        ImGui::CloseCurrentPopup();
                    }
                    if (ImGui::Selectable(
                                fmt::format("View {0:03X} in memory viewer", value).c_str())) {
                        message = DbgMessage{ dbg_component::memory_view, dbg_action::scroll,
                                              ScrollMessage{ value } };
                    }
                    ImGui::EndPopup();
                }

                ImGui::TableNextColumn();
                helpers::colored_centered_text_known({ 255, 0, 0, 255 }, emu.I_change, threesize,
                                                     "%03X", emu.get_I());
            }
            else {
                ImGui::TableNextColumn();
                // fix this
                helpers::center_text("???");
            }
        };

        auto draw_PC = [&]() {
            ImGui::TableNextColumn();
            helpers::center_text("PC");

            if (emu.is_readable()) {
                ImGui::SameLine();
                ImGui::Selectable("###PC", false, ImGuiSelectableFlags_SpanAllColumns);

                if (ImGui::BeginPopupContextItem()) {
                    uint16_t value = emu.get_PC() & 0xFFF;
                    if (ImGui::Selectable(
                                fmt::format("Follow to {0:03X} in disassembly", value).c_str())) {
                        message = DbgMessage(dbg_component::disassembly_view, dbg_action::scroll,
                                             ScrollMessage{ value });
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }

                ImGui::TableNextColumn();
                helpers::colored_centered_text_known({ 255, 0, 0, 255 }, emu.PC_change, threesize,
                                                     "%03X", emu.get_PC());
            }
            else {
                ImGui::TableNextColumn();
                helpers::center_text("???");
            }
        };

        auto draw_D = [&]() {
            ImGui::TableNextColumn();
            helpers::center_text("D");
            ImGui::TableNextColumn();
            if (emu.is_readable()) {
                helpers::colored_centered_text_known({ 255, 0, 0, 255 }, emu.dt_change, rhs_width,
                                                     "%02X", emu.get_DT());
            }
            else {
                helpers::center_text("??");
            }
        };

        auto draw_S = [&]() {
            ImGui::TableNextColumn();
            helpers::center_text("S");
            ImGui::TableNextColumn();

            if (emu.is_readable()) {
                helpers::colored_centered_text_known({ 255, 0, 0, 255 }, emu.st_change, rhs_width,
                                                     "%02X", emu.get_ST());
            }
            else {
                helpers::center_text("??");
            }
        };

        // we check one time to calculate how much space we will need for our pair of register/values
        ImGui::Begin("Registers", &window_state);

        helpers::center_text("Registers");
        // calculate how many pairs we can draw horizontally
        auto avail = ImGui::GetContentRegionAvail();
        int  count = static_cast<int>(std::floor(avail.x / pair_width));

        if (count == 0) {
            count = 1;
        }

        ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, { 0.5f, 0.0f });

        // draw outer table
        if (ImGui::BeginTable("allregisters", count,
                              ImGuiTableFlags_NoHostExtendX | ImGuiTableFlags_SizingStretchSame |
                                      ImGuiTableFlags_Borders | ImGuiTableFlags_NoPadInnerX)) {
            for (auto index = 0; index < 16; ++index) {
                ImGui::TableNextColumn();

                helpers::center_cursor(pair_width);
                // draw each pair
                if (ImGui::BeginTable(
                            "pair", 2,
                            ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_NoHostExtendX |
                                    ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_NoPadOuterX,
                            { pair_width, 0.0f })) {
                    ImGui::TableNextColumn();
                    helpers::center_text_known("V%01x", lhs_width, index);
                    ImGui::TableNextColumn();

                    if (emu.is_readable()) {
                        helpers::colored_centered_text_known({ 255, 0, 0, 255 },
                                                             emu.reg_changes[index], rhs_width,
                                                             "%02X", emu.get_V(index));
                    }
                    else {
                        helpers::center_text("??");
                    }

                    ImGui::EndTable();
                }
            }
            ImGui::EndTable();
        }
        // three cases, we draw vertically, horizontally, or in a table if we're square
        // vertical case
        if (count == 1) {
            if (ImGui::BeginTable("special registers", 2,
                                  ImGuiTableFlags_NoHostExtendX |
                                          ImGuiTableFlags_SizingStretchSame |
                                          ImGuiTableFlags_Borders | ImGuiTableFlags_NoPadInnerX)) {
                draw_I();
                draw_PC();
                draw_D();
                draw_S();
                ImGui::EndTable();
            }
        }
        // horizontal case
        else if (count > 7) {
            if (ImGui::BeginTable("special registers", 8,
                                  ImGuiTableFlags_NoHostExtendX |
                                          ImGuiTableFlags_SizingStretchSame |
                                          ImGuiTableFlags_Borders | ImGuiTableFlags_NoPadInnerX)) {
                draw_I();
                draw_PC();
                draw_D();
                draw_S();
                ImGui::EndTable();
            }
        }
        // square
        else {
            if (ImGui::BeginTable("special registers", 2,
                                  ImGuiTableFlags_NoHostExtendX |
                                          ImGuiTableFlags_SizingStretchSame |
                                          ImGuiTableFlags_Borders | ImGuiTableFlags_NoPadInnerX)) {
                ImGui::TableNextColumn();
                helpers::center_text("Special");
                ImGui::TableNextColumn();
                helpers::center_text("Timers");

                ImGui::TableNextRow();
                ImGui::TableNextColumn();

                if (ImGui::BeginTable(
                            "Special", 2,
                            ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_NoHostExtendX |
                                    ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_NoPadOuterX)) {

                    draw_I();

                    draw_PC();

                    ImGui::EndTable();
                }

                ImGui::TableNextColumn();

                if (ImGui::BeginTable(
                            "Timers", 2,
                            ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_NoHostExtendX |
                                    ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_NoPadOuterX)) {

                    draw_D();
                    draw_S();

                    ImGui::EndTable();
                }

                ImGui::EndTable();
            }
        }

        ImGui::PopStyleVar();

        ImGui::End();
    }

    // dont do shit at all bruh
    void RegisterView::process_dbg_msg(DbgMessage m) { return; }

} // namespace GUI