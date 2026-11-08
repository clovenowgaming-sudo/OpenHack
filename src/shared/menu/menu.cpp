#include "menu.hpp"
#include "../openhack.hpp"
#include "../config.hpp"
#include "../hacks/hacks.hpp"
// #include "blur.hpp"

namespace openhack::menu {
    bool isOpened = false;
    bool isInitialized = false;
    std::vector<gui::Window> windows;
    int selectedWindow = 0;

    void toggle() {
        isOpened = !isOpened;

        if (!isOpened) {
            config::set("windows", windows);
            config::save();
        }
    }

    bool isOpen() { return isOpened; }

    void setVisibility(bool value) { isOpened = value; }

    bool isVisible() {
        return isOpened;
    }

    void createOpenURLPopup(const std::string &url) {
        gui::Modal::create("Open URL", [url](gui::Modal *popup) {
            ImGui::TextWrapped("You are about to open\n%s in your browser.", url.c_str());
            ImGui::TextWrapped("Do you want to continue?");
            if (gui::button("Yes", {0.5f, 0.f})) {
                utils::openURL(url.c_str());
                popup->close();
            }
            ImGui::SameLine(0, 2);
            if (gui::button("No")) {
                popup->close();
            }
        });
    }

    void init() {
        // Make sure to initialize ImGui
        gui::init();

        // Then initialize the windows
        if (isInitialized)
            return;

        addWindow("OpenHack", []() {
            gui::text("Version: " OPENHACK_VERSION);
            gui::text("Build: " __DATE__ " " __TIME__);
            gui::text("Game version: %s", utils::getGameVersion().c_str());

            if (gui::button("GitHub", ImVec2(0.5, 0)))
                createOpenURLPopup(OPENHACK_HOME_URL);

            auto searchValue = config::getGlobal<std::string>("searchValue", "");
            gui::widthF(1.0);
            gui::inputText("##Search", &searchValue, 64, "Search");
            gui::width();
            config::setGlobal("searchValue", searchValue);
        });

        addWindow("Interface", []() {
            gui::width(120);
            if (gui::combo("Theme", "menu.theme", gui::THEME_NAMES, gui::THEME_COUNT)) {
                gui::setTheme(config::get<gui::Themes>("menu.theme"));
                gui::loadPalette();
            }
            gui::width();

            gui::popupSettings("Sizes", []() {
                gui::width(70);
                gui::inputFloat("UI Scale", "menu.uiScale", 0.5f, 3.0f, "%.3f");
                gui::inputFloat("Border Size", "menu.borderSize", 0.0f, 10.0f, "%.3f");
                gui::inputFloat("Window Rounding", "menu.windowRounding", 0.0f, 10.0f, "%.3f");
                gui::inputFloat("Frame Rounding", "menu.frameRounding", 0.0f, 10.0f, "%.3f");
                gui::width();
            });

            gui::popupSettings("Fonts", []() {
                gui::width(120);
                auto fonts = gui::getFonts();
                std::string fontsLine; // Create null-delimited string with all font names
                for (auto &font: fonts)
                    fontsLine += font.name + '\0';
                fontsLine += '\0';
                int32_t currentFont = gui::getFontIndex();
                if (gui::combo("Font", &currentFont, fontsLine.c_str())) {
                    auto selected = fonts[currentFont].name;
                    gui::setFont(selected);
                }
                gui::inputFloat("Font Size", "menu.fontSize", 8.0f, 32.0f, "%.1f px");
                gui::width();

                if (gui::button("Reload Fonts")) {
                    config::setGlobal("resetNextFrame", true);
                    return;
                }
                gui::tooltip("Reloads the UI to apply changes which require a restart");
            });

            gui::popupSettings("Colors", []() {
                gui::width(120);
                gui::colorEdit("Background", "menu.color.background");
                gui::colorEdit("Accent", "menu.color.accent");
                gui::colorEdit("Primary", "menu.color.primary");
                gui::colorEdit("Secondary", "menu.color.secondary");
                gui::colorEdit("Border", "menu.color.border");
                gui::colorEdit("Hovered", "menu.color.hovered");
                gui::colorEdit("Clicked", "menu.color.clicked");
                gui::colorEdit("Text", "menu.color.text");
                gui::colorEdit("Text Disabled", "menu.color.textDisabled");
                gui::width();

                if (gui::button("Reset")) {
                    gui::loadPalette();
                }
            });

            // TODO: Implement blur properly
            // if (gui::combo("Blur", "menu.blur", "Off\0Windows\0Screen\0\0")) {
            //     blur::setState(config::get<blur::State>("menu.blur"));
            // }


            gui::callback([]() {
                gui::tooltip("Makes the title bar change colors.");
            });
            gui::toggleSetting(
                    "Rainbow Menu",
                    "menu.rainbow.enabled",
                    []() {
                        gui::width(110);
                        gui::inputFloat("Speed", "menu.rainbow.speed");
                        gui::inputFloat("Saturation", "menu.rainbow.saturation", 0.0f, 100.0f);
                        gui::inputFloat("Value", "menu.rainbow.value", 0.0f, 100.0f);
                        gui::width();
                    });

            gui::inputFloat("Button Size", "menu.button.size", 0.5f, 3.0f, "%.2fx");
            gui::tooltip("Size of the round button which opens the menu.");

        });

        // Get windows for hacks
        auto hacks = hacks::getWindows();
        for (auto &hack: hacks) {
            windows.push_back(hack);
        }

        // Call late init for embedded hacks
        for (auto &hack: hacks::getEmbeddedHacks()) {
            hack->onLateInit();
        }

        // Restore window positions from config
        if (config::has("windows")) {
            auto loaded = config::get<std::vector<gui::Window>>("windows");
            for (auto &window: windows) {
                auto it = std::find_if(loaded.begin(), loaded.end(),
                                       [&window](const gui::Window &w) { return w.getTitle() == window.getTitle(); });
                if (it != loaded.end()) {
                    auto opened = it->isOpen();
                    auto position = it->getPosition();
                    window.setOpen(opened);
                    window.setPosition(position);
                    window.setDrawPosition(position);
                }
            }
        }

        // Sort windows by title
        std::sort(windows.begin(), windows.end(), [](const gui::Window &a, const gui::Window &b) {
            return a.getTitle() < b.getTitle();
        });

        isInitialized = true;
    }

    void updateColors() {
        // Rainbow menu
        bool rainbowEnabled = config::get<bool>("menu.rainbow.enabled");
        gui::Color accentOrig;
        if (rainbowEnabled) {
            accentOrig = config::get<gui::Color>("menu.color.accent");

            // Calculate new colors
            auto speed = config::get<float>("menu.rainbow.speed");
            auto saturation = config::get<float>("menu.rainbow.saturation");
            auto value = config::get<float>("menu.rainbow.value");

            float r, g, b;
            ImGui::ColorConvertHSVtoRGB(
                    (float) ImGui::GetTime() * speed,
                    saturation / 100.0f,
                    value / 100.0f,
                    r, g, b);

            gui::Color primary = {r, g, b, accentOrig.a};
            config::set("menu.color.accent", primary);
        }

        // Update theme
        gui::setStyles();

        // Revert rainbow menu colors
        if (rainbowEnabled) {
            config::set("menu.color.accent", accentOrig);
        }
    }

    static void drawFloatingButton() {
        auto &io = ImGui::GetIO();
        const float scale = config::getGlobal<float>("UIScale", 1.f);
        const float btnScale = std::clamp(config::get<float>("menu.button.size", 1.0f), 0.5f, 3.0f);
        const float radius = 22.f * scale * btnScale;
        const float diameter = radius * 2.f;

        static bool s_posInit = false;
        static ImVec2 s_pos;
        static bool s_dragged = false;
        if (!s_posInit) {
            float x = config::get<float>("menu.button.x", -1.f);
            float y = config::get<float>("menu.button.y", -1.f);
            if (x < 0.f || y < 0.f) {
                x = 12.f * scale;
                y = io.DisplaySize.y * 0.35f;
            }
            s_pos = ImVec2(x, y);
            s_posInit = true;
        }

        s_pos.x = std::clamp(s_pos.x, 0.f, std::max(0.f, io.DisplaySize.x - diameter));
        s_pos.y = std::clamp(s_pos.y, 0.f, std::max(0.f, io.DisplaySize.y - diameter));

        ImGui::SetNextWindowPos(s_pos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(diameter, diameter), ImGuiCond_Always);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, radius);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));

        const ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                       ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                                       ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoSavedSettings |
                                       ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav |
                                       ImGuiWindowFlags_NoBringToFrontOnFocus;

        if (ImGui::Begin("##openhack_button", nullptr, flags)) {
            ImGui::InvisibleButton("##openhack_button_hit", ImVec2(diameter, diameter));

            if (ImGui::IsItemActivated())
                s_dragged = false;

            if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 8.f * scale)) {
                s_dragged = true;
                s_pos.x += io.MouseDelta.x;
                s_pos.y += io.MouseDelta.y;
            }

            if (ImGui::IsItemDeactivated()) {
                if (s_dragged) {
                    config::setGlobal("buttonX", s_pos.x);
                    config::setGlobal("buttonY", s_pos.y);
                    config::set("menu.button.x", s_pos.x);
                    config::set("menu.button.y", s_pos.y);
                    config::save();
                } else {
                    toggle();
                }
            }

            auto *dl = ImGui::GetWindowDrawList();
            ImVec2 center(s_pos.x + radius, s_pos.y + radius);
            auto accent = config::get<gui::Color>("menu.color.accent");
            ImU32 fill = ImGui::GetColorU32(ImVec4(accent.r, accent.g, accent.b, isOpened ? 0.95f : 0.75f));
            dl->AddCircleFilled(center, radius, fill, 48);
            dl->AddCircle(center, radius, IM_COL32(255, 255, 255, 200), 48, 2.0f * scale);

            const char *label = "OH";
            ImVec2 textSize = ImGui::CalcTextSize(label);
            dl->AddText(ImVec2(center.x - textSize.x * 0.5f, center.y - textSize.y * 0.5f),
                        IM_COL32(255, 255, 255, 255), label);
        }
        ImGui::End();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(3);
    }

    static void drawPanel() {
        auto &io = ImGui::GetIO();
        const float scale = config::getGlobal<float>("UIScale", 1.f);
        const ImVec2 screen = io.DisplaySize;

        ImVec2 size(std::min(screen.x * 0.94f, 1250.f * scale), screen.y * 0.9f);
        ImGui::SetNextWindowPos(ImVec2(screen.x * 0.5f, screen.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(size, ImGuiCond_Always);

        ImGui::GetStyle().ScrollbarSize = 22.f * scale;

        const ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                       ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                                       ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar |
                                       ImGuiWindowFlags_NoScrollWithMouse;

        if (ImGui::Begin("OpenHack 2.7##panel", nullptr, flags)) {
            // Header
            ImGui::TextUnformatted("OpenHack " OPENHACK_VERSION);
            ImGui::SameLine();
            float closeW = 110.f * scale;
            ImGui::SetCursorPosX(ImGui::GetWindowWidth() - closeW - ImGui::GetStyle().WindowPadding.x);
            if (ImGui::Button("Close", ImVec2(closeW, 0)))
                toggle();
            ImGui::Separator();

            if (selectedWindow < 0 || selectedWindow >= (int) windows.size())
                selectedWindow = 0;

            const float sideW = 230.f * scale;
            auto avail = ImGui::GetContentRegionAvail();

            // Sidebar with the list of categories
            ImGui::BeginChild("##oh_side", ImVec2(sideW, avail.y), ImGuiChildFlags_Borders);
            for (int i = 0; i < (int) windows.size(); i++) {
                if (ImGui::Selectable(windows[i].getTitle().c_str(), i == selectedWindow,
                                      ImGuiSelectableFlags_None, ImVec2(0, 44.f * scale))) {
                    selectedWindow = i;
                }
            }
            ImGui::EndChild();

            ImGui::SameLine();

            // Content of the selected category
            ImGui::BeginChild("##oh_content", ImVec2(0, avail.y), ImGuiChildFlags_Borders);
            if (!windows.empty())
                windows[selectedWindow].drawContent();

            // Touch scrolling: drag vertically anywhere inside the content area
            static bool s_scrolling = false;
            const float threshold = 14.f * scale;
            if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) &&
                ImGui::IsMouseDown(ImGuiMouseButton_Left) && !s_scrolling) {
                ImVec2 d(io.MousePos.x - io.MouseClickedPos[0].x, io.MousePos.y - io.MouseClickedPos[0].y);
                if (std::fabs(d.y) > threshold && std::fabs(d.y) > std::fabs(d.x) * 1.5f) {
                    s_scrolling = true;
                    ImGui::ClearActiveID();
                }
            }
            if (s_scrolling) {
                if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
                    ImGui::SetScrollY(ImGui::GetScrollY() - io.MouseDelta.y);
                else
                    s_scrolling = false;
            }
            ImGui::EndChild();
        }
        ImGui::End();
    }

    void draw() {
        // Calculate relative UI scale against 1080p
        auto resW = ImGui::GetIO().DisplaySize.x;
        auto ratio = resW / 1920.0f;
        auto currentScale = config::get<float>("menu.uiScale");
        config::setGlobal("UIScale", ratio * currentScale);

        utils::resetKeyStates();
        keybinds::update();

        // Update embedded hacks
        for (auto &hack: hacks::getEmbeddedHacks()) {
            hack->update();
        }

        updateColors();
        drawFloatingButton();

        if (isOpened)
            drawPanel();

        gui::drawPopups();

        // Reset UI if requested
        if (config::getGlobal<bool>("resetNextFrame", false)) {
            config::setGlobal("resetNextFrame", false);
            utils::resetUI();
        }
    }

    void addWindow(std::string_view title, const std::function<void()> &onDraw) {
        windows.emplace_back(title, onDraw);
    }
}
