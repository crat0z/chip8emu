#ifndef GUICOMPONENT_HPP
#define GUICOMPONENT_HPP

#include "gui/guimessage.hpp"
#include <optional>

namespace GUI {
    class GUIComponent {
    protected:
        std::optional<GUIMessage> message;

        bool  window_state;
        float font_size;

        GUIComponent() = delete;

    public:
        GUIComponent(float fs, bool start = true)
                : message{ std::nullopt }, window_state{ start }, font_size{ fs } {}

        virtual ~GUIComponent() {}

        bool& is_enabled() { return window_state; }

        virtual void draw_window() = 0;

        std::optional<GUIMessage> get_message() {
            auto ret = std::move(message);
            message.reset();
            return ret;
        }

        virtual void process_message([[maybe_unused]] GUIMessage msg) {}
    };
} // namespace GUI

#endif