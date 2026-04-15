#include "ui/Views.hpp"

namespace ssoc::ui::views {

void draw_welcome_help_window(bool &show) {
  ImGui::SetNextWindowPos(ImVec2(150, 50), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(1000, 700), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowBgAlpha(1.0f);

  ImGui::Begin("Welcome | Help", &show, ImGuiWindowFlags_NoCollapse);

  // shortcuts
  if (ImGui::IsKeyPressed(ImGuiKey_Q) || (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) &&
                                          ImGui::IsKeyPressed(ImGuiKey_Q))) {
    show = false;
  }

  _detail::HelpLayoutState state;

  _detail::draw_help_title();

  ImVec2 a = ImGui::GetContentRegionAvail();

  // -20 is just to prevent not enough region
  float top_h = a.y * state.split_y - 20.0f;
  float bot_h = a.y - top_h - 20.0f;

  draw_help_top(state, top_h, a.x);

  ImGui::InvisibleButton("split_h", ImVec2(-1, 6));
  if (ImGui::IsItemActive())
    state.split_y = std::clamp(
        state.split_y + ImGui::GetIO().MouseDelta.y / a.y, 0.2f, 0.8f);

  draw_help_bottom(state, bot_h, a.x);

  ImGui::End();
}

namespace _detail {

void draw_help_title() {
  const char *title = "Sandpile Simulation (SOC)";

  float tw = ImGui::CalcTextSize(title).x;
  ImGui::SetCursorPosX((ImGui::GetWindowWidth() - tw) * 0.5f);

  ImGui::Text("%s", title);
  ImGui::Separator();
  ImGui::Spacing();
}

void draw_help_top(HelpLayoutState &s, float h, float w) {
  ImGui::BeginChild("top", ImVec2(0, h));

  float left_w = w * s.split_x;

  _detail::draw_help_top_left(left_w);

  ImGui::SameLine();

  ImGui::InvisibleButton("split_top", ImVec2(6, h));
  if (ImGui::IsItemActive())
    s.split_x =
        std::clamp(s.split_x + ImGui::GetIO().MouseDelta.x / w, 0.2f, 0.8f);

  ImGui::SameLine();

  _detail::draw_help_top_right();

  ImGui::EndChild();
}

void draw_help_top_left(float left_w) {
  ImGui::BeginChild("welcome", ImVec2(left_w, 0), true);

  ImGui::Text("Welcome");
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::TextWrapped("TODO: here will be some short onboarding, info for who "
                     "this app is.\n");

  ImGui::Spacing();

  ImGui::TextWrapped("You can close this window using shortcut: [CTRL-q]");

  ImGui::TextDisabled(
      "Tip: You can reopen this window from the main menu at [Windows].");

  ImGui::EndChild();
}

void draw_help_top_right() {
  ImGui::BeginChild("soc", ImVec2(0, 0), true);

  ImGui::Text("What is SOC?");
  ImGui::Separator();

  ImGui::TextWrapped(
      "TODO: here short description of SOC & its relation to sandpile");

  ImGui::EndChild();
}

void draw_help_bottom(HelpLayoutState &s, float h, float w) {
  ImGui::BeginChild("bottom", ImVec2(0, h));

  float left_w = w * s.split_x;

  _detail::draw_help_bottom_left(left_w);

  ImGui::SameLine();

  ImGui::InvisibleButton("split_bottom", ImVec2(6, h));
  if (ImGui::IsItemActive())
    s.split_x =
        std::clamp(s.split_x + ImGui::GetIO().MouseDelta.x / w, 0.2f, 0.8f);

  ImGui::SameLine();

  _detail::draw_help_bottom_right();

  ImGui::EndChild();
}

void draw_help_bottom_left(float left_w) {
  ImGui::BeginChild("usage", ImVec2(left_w, 0), true);

  ImGui::Text("How to use");
  ImGui::Separator();

  ImGui::TextWrapped(
      "TODO: detail explanation of individual functionalities in app");

  ImGui::EndChild();
}

void draw_help_bottom_right() {
  ImGui::BeginChild("about", ImVec2(0, 0), true);

  ImGui::Text("About");
  ImGui::Separator();

  ImGui::TextWrapped("TODO: Why this app was created. all info on GH:");

  ImGui::Spacing();

  auto open_url = [&](const std::string &url) {
#ifdef _WIN32
    std::system(("start " + url).c_str());
#elif __APPLE__
    std::system(("open " + url).c_str());
#else
    std::system(("xdg-open " + url).c_str());
#endif
  };

  if (ImGui::Button("Open GitHub repository")) {
    open_url("https://github.com/vokymir/kiv-bpr");
  }

  ImGui::EndChild();
}

} // namespace _detail

} // namespace ssoc::ui::views
