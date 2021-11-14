#include "gui/debugger/stack_view.hpp"
#include "gui/imgui_helpers.hpp"
#include <fmt/format.h>

namespace GUI {

    StackView::StackView(core::EmuWrapper& e, float fs) : DbgComponent(e, fs) {}

    void StackView::draw_window() {

        ImGui::SetNextWindowSize({ 300, 200 }, ImGuiCond_FirstUseEver);

        ImGui::Begin("Stack view", &window_state);
        {
            ImGui::BeginChild("stacks");
            auto& stack = emu.get_stack();
            if (emu.is_readable()) {

                for (auto i = 15; i >= 0; --i) {
                    // if we actually are at a point we can print somethin

                    if (i + 1 <= stack.size()) {
                        ImGui::Selectable(fmt::format("{0:03X}", stack[i]).c_str());
                    }
                    else {
                        helpers::disabled_centered_text("???");
                    }
                }
            }
            else {
                for (auto i = 15; i >= 0; --i) {
                    helpers::disabled_centered_text("???");
                }
            }

            ImGui::EndChild();
        }
        ImGui::End();
    }

    void StackView::process_dbg_msg(DbgMessage m) { return; }

} // namespace GUI