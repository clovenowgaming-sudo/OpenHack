#include "hacks.hpp"
#include "../openhack.hpp"
#include "hack-list.hpp"

namespace openhack::hacks {
    void ToggleComponent::onInit() {
        // Load the initial state from the configuration
        m_enabled = config::get<bool>("hack." + m_id, false);
        if (m_enabled) applyPatch();

        // Add keybinding callback
        menu::keybinds::setKeybindCallback(m_id, [this]() {
            this->m_enabled = !this->m_enabled;
            this->toggled();
        });
    }

    void ToggleComponent::onDraw() {
        if (m_hasWarning)
            ImGui::BeginDisabled();

        bool pressed = gui::checkbox(m_name.c_str(), &m_enabled);

        if (m_hasWarning) {
            ImGui::EndDisabled();
            gui::tooltip("Your game version is not supported, or you have conflicting mods.");
            return;
        } else if (!m_description.empty()) {
            gui::tooltip(m_description.c_str());
        }

        menu::keybinds::addMenuKeybind(m_id, m_name, [this]() {
            this->m_enabled = !this->m_enabled;
            this->toggled();
        });

        if (pressed) {
            toggled();
        }
    }

    static std::unordered_map<std::string, bool> s_flags;

    bool isOn(std::string_view id) {
        auto it = s_flags.find(std::string(id));
        return it != s_flags.end() && it->second;
    }

    bool ToggleComponent::applyPatch(bool enable) {
        if (m_hasWarning)
            return false;
        s_flags[m_id] = enable;
        return true;
    }

    void ToggleComponent::toggled() {
        applyPatch();
        config::set("hack." + m_id, m_enabled);
    }

    void TextComponent::onDraw() {
        gui::text(m_text.c_str());
    }

    static std::vector<gui::Window> windows;
    static std::vector<Component *> components;
    static std::vector<ToggleComponent *> hacks;

    void initialize() {
        auto hacksDir = utils::getModHacksDirectory();
        if (!std::filesystem::exists(hacksDir)) {
            L_ERROR("Hacks directory does not exist: {}", hacksDir.string());
            return;
        }

        // Add the embedded hacks
        std::vector<EmbeddedHack*> embeddedHacks = hacks::s_allHacks;

        for (const auto &entry: std::filesystem::directory_iterator(hacksDir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                // Load the JSON file
                std::ifstream file(entry.path());
                if (!file.is_open()) {
                    L_ERROR("Failed to open file: {}", entry.path().string());
                    continue;
                }

                std::stringstream buffer;
                buffer << file.rdbuf();
                file.close();

                std::vector<Component *> windowComponents;

                try {
                    nlohmann::json json = nlohmann::json::parse(buffer.str());
                    std::string title = json.at("title").get<std::string>();

                    L_BENCHMARK(title,
                    for (const auto &component: json.at("items")) {
                        if (component.contains("version")) {
                            auto version = component.at("version").get<std::string>();
                            if (!utils::compareVersion(version))
                                continue;
                        }

                        auto type = component.at("type").get<std::string>();
                        if (type == "text") {
                            auto text = component.at("text").get<std::string>();
                            windowComponents.push_back(new TextComponent(text));
                        } else if (type == "toggle") {
                            auto toggleTitle = component.at("title").get<std::string>();
                            auto id = component.at("id").get<std::string>();
                            auto toggle = new ToggleComponent(toggleTitle, id);

                            if (component.contains("description")) {
                                toggle->setDescription(component.at("description").get<std::string>());
                            }

                            if (component.contains("cheat")) {
                                toggle->setIsCheat(component.at("cheat").get<bool>());
                            }

                            windowComponents.push_back(toggle);
                            hacks.push_back(toggle);
                        } else if (type == "embedded") {
                            auto hack = component.at("hack").get<std::string>();
                            auto it = std::find_if(embeddedHacks.begin(), embeddedHacks.end(), [hack](const auto &h) {
                                return h->getId() == hack;
                            });

                            if (it != embeddedHacks.end()) {
                                windowComponents.push_back(*it);
                                embeddedHacks.erase(it);
                            } else {
                                L_WARN("Failed to find embedded hack: {}", hack);
                            }
                        }
                    }
                    );

                    // Sort the components
                    std::sort(windowComponents.begin(), windowComponents.end(), [](const auto &a, const auto &b) {
                        std::string aName = a->getName();
                        std::string bName = b->getName();

                        std::transform(aName.begin(), aName.end(), aName.begin(), ::tolower);
                        std::transform(bName.begin(), bName.end(), bName.begin(), ::tolower);

                        return aName < bName;
                    });

                    // Initialize the components
                    for (auto &component: windowComponents) {
                        component->onInit();
                    }

                    // Add the window
                    windows.emplace_back(title, [windowComponents]() {
                        for (auto &component: windowComponents) {
                            component->onDraw();
                        }
                    });

                } catch (const std::exception &e) {
                    L_ERROR("Failed to parse file: {}", entry.path().string());
                    L_ERROR("{}", e.what());
                    continue;
                }
            }
        }

        // Initialize the embedded hacks
        for (auto &hack: embeddedHacks) {
            hack->onInit();
        }
    }

    std::vector<gui::Window> &getWindows() {
        return windows;
    }

    std::vector<Component *> &getComponents() {
        return components;
    }

    std::vector<ToggleComponent *> &getHacks() {
        return hacks;
    }

    std::vector<EmbeddedHack *> &getEmbeddedHacks() {
        return s_allHacks;
    }

}
