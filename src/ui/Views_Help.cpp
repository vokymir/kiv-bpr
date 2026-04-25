#include "ui/Layout.hpp"
#include "ui/Views.hpp"

namespace ssoc::ui::views {

void draw_welcome_help_window(bool &show) {
  layout::set_next_window(layout::Window::HELP);
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

  ImGui::TextWrapped(
      "This software is capable of simulating Abelian Sandpile "
      "Model on various graph topologies. "
      "It was developed for the needs of bachelor thesis. "
      "There are not many helper texts explaining what are the possibilities - "
      "for that, read the thesis itself. (See the github repository.)");

  ImGui::Spacing();

  ImGui::TextWrapped("You can close this window using shortcut: [CTRL-q]");

  ImGui::TextDisabled(
      "Tip: You can reopen this window from the main menu at [Windows].");

  ImGui::EndChild();
}

void draw_help_top_right() {
  ImGui::BeginChild("hook", ImVec2(0, 0), true);

  ImGui::Text("Abstract?");
  ImGui::Separator();

  ImGui::TextWrapped(
      "In everyday life, person's outburst is rarely caused by a single large "
      "event. "
      "Rather, the tension builds up from small frustrations over time. When "
      "certain "
      "treshold is reached, even a minor additional stress can trigger an "
      "outburst. "
      "This redistributes the tension to others in their social circle. This "
      "may in "
      "turn cause further reactions, leading to a cascade of reach and size "
      "which are "
      "difficult to predict. A local event may therefore propagate through a "
      "network, "
      "depending on individual thresholds and the shape of the network.\n\n"

      "...\n\n"

      "Systems in critical state are most responsive to input and behave "
      "unpredictably. This is the case for phase transition and forest fires. "
      "However, "
      "there is a significant difference. Criticality can be reached in many "
      "systems by "
      "fine-tuning certain parameters. In the case of phase transition the "
      "parameters "
      "are pressure and temperature. However, there exist systems which reach "
      "criticality independently of parameters.\n\n"

      "...\n\n"

      "In this thesis, we investigate the behaviour of the sandpile model on "
      "different "
      "graph structures. Specifically, on small-world networks which are a "
      "class of "
      "random networks. Before this generalization we introduce few necessary "
      "concepts "
      "from graph theory.");

  ImGui::TextDisabled("That was an excerpt from the thesis. To gain full "
                      "understanding read the thesis.");

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

  ImGui::TextWrapped("After closing this HELP window, you will be presented "
                     "with \"Simulation configuration\" window.");

  ImGui::Spacing();

  ImGui::TextWrapped("Once there, choose any graph model, dissipation rule and "
                     "visualization model - "
                     "adjust parameters and generate graph.");

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::TextWrapped("Generating graph will present you with new windows:");

  ImGui::Spacing();

  // == VISUALIZATION
  ImGui::BulletText("Visualization");
  ImGui::Indent();
  ImGui::TextWrapped("In the bottom left, you can see the generated graph. "
                     "Use left-click to move the view, mouse wheel to zoom, "
                     "or adjust vertex positions via right-click.");
  ImGui::Unindent();

  ImGui::Spacing();

  // == CONTROLS
  ImGui::BulletText("Controlling visualization");
  ImGui::Indent();
  ImGui::TextWrapped("In the top left corner, you can control visualization "
                     "and adjust zoom speed.");
  ImGui::Unindent();

  ImGui::Spacing();

  // == STATISTICS
  ImGui::BulletText("Statistics");
  ImGui::Indent();
  ImGui::TextWrapped("On the right side, you can inspect various statistics "
                     "for the current state of the model.");
  ImGui::Unindent();

  ImGui::Spacing();

  // == SIMULATION
  ImGui::BulletText("Controlling simulation");
  ImGui::Indent();

  ImGui::TextWrapped(
      "In the top center section you can control the simulation.");

  ImGui::Spacing();

  ImGui::BulletText("Simulation modes");
  ImGui::Indent();
  ImGui::TextWrapped(
      "Step In: drop one grain, see full avalanche step-by-step.\n"
      "Step Over: drop one grain, resolve avalanche automatically.\n"
      "Run Until Avalanche: drop grains until avalanche occurs.\n"
      "Run: continuously drop grains.");
  ImGui::Unindent();

  ImGui::Spacing();

  ImGui::BulletText("Configuration");
  ImGui::Indent();
  ImGui::TextWrapped("Render frequency: only render every Xth frame. Higher "
                     "values increase speed but reduce responsiveness.\n"
                     "Random sand: toggle deterministic vs random grain "
                     "placement. You may specify a vertex ID.\n");
  ImGui::Unindent();

  ImGui::BulletText("Graph");
  ImGui::Indent();
  ImGui::TextWrapped(
      "Regenerate: generate a new graph with the same parameters.\n"
      "New: return to simulation configuration window.");
  ImGui::Unindent();

  ImGui::EndChild();
}

void draw_help_bottom_right() {
  ImGui::BeginChild("about", ImVec2(0, 0), true);

  ImGui::Text("About");
  ImGui::Separator();

  ImGui::TextWrapped(
      "Created by Jakub Vokoun for the purposes of Bachelor Thesis led by Petr "
      "Stehlík.\n"
      "At Department of Informatics and Computer Technology,\n"
      "Faculty of Applied Sciences,\n"
      "University of West Bohemia,\n"
      "Anno Domini 2026.\n\n"
      "For the Thesis and/or Source Code visit the GitHub repository:");

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
