#include "Ui.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <cstddef>
#include <format>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>
#include <stdexcept>
#include <type_traits>
#include <variant>

namespace ssoc::ui {

void UI::init() {
  // Setup SDL
  // [If using SDL_MAIN_USE_CALLBACKS: all code below until the main loop starts
  // would likely be your SDL_AppInit() function]
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    throw std::runtime_error(
        std::format("Error: SDL_Init(): {}\n", SDL_GetError()));
  }

  // Decide GL+GLSL versions
  // GL 3.0 + GLSL 130
  const char *glsl_version = "#version 130";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

  // Create window with graphics context
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
  SDL_WindowFlags window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
                                 SDL_WINDOW_HIDDEN |
                                 SDL_WINDOW_HIGH_PIXEL_DENSITY;
  SDL_Window *window = SDL_CreateWindow("Dear ImGui SDL3+OpenGL3 example",
                                        (int)(1280 * main_scale),
                                        (int)(800 * main_scale), window_flags);
  if (window == nullptr) {
    throw std::runtime_error(
        std::format("Error: SDL_CreateWindow(): {}\n", SDL_GetError()));
  }
  SDL_GLContext gl_context = SDL_GL_CreateContext(window);
  if (gl_context == nullptr) {
    throw std::runtime_error(
        std::format("Error: SDL_CreateContext(): {}\n", SDL_GetError()));
  }

  SDL_GL_MakeCurrent(window, gl_context);
  SDL_GL_SetSwapInterval(1); // Enable vsync
  SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
  SDL_ShowWindow(window);

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  // Setup scaling
  ImGuiStyle &style = ImGui::GetStyle();
  style.ScaleAllSizes(
      main_scale); // Bake a fixed style scale. (until we have a solution for
                   // dynamic style scaling, changing this requires resetting
                   // Style + calling this again)
  style.FontScaleDpi =
      main_scale; // Set initial font scale. (in docking branch: using
                  // io.ConfigDpiScaleFonts=true automatically overrides this
                  // for every window depending on the current monitor)

  // Setup Platform/Renderer backends
  ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
  ImGui_ImplOpenGL3_Init(glsl_version);

  // Load Fonts
  // - If fonts are not explicitly loaded, Dear ImGui will call AddFontDefault()
  // to select an embedded font: either AddFontDefaultVector() or
  // AddFontDefaultBitmap().
  //   This selection is based on (style.FontSizeBase * style.FontScaleMain *
  //   style.FontScaleDpi) reaching a small threshold.
  // - You can load multiple fonts and use ImGui::PushFont()/PopFont() to select
  // them.
  // - If a file cannot be loaded, AddFont functions will return a nullptr.
  // Please handle those errors in your code (e.g. use an assertion, display an
  // error and quit).
  // - Read 'docs/FONTS.md' for more instructions and details.
  // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use FreeType
  // for higher quality font rendering.
  // - Remember that in C/C++ if you want to include a backslash \ in a string
  // literal you need to write a double backslash \\ !
  // - Our Emscripten build process allows embedding fonts to be accessible at
  // runtime from the "fonts/" folder. See Makefile.emscripten for details.
  // style.FontSizeBase = 20.0f;
  // io.Fonts->AddFontDefaultVector();
  // io.Fonts->AddFontDefaultBitmap();
  // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
  // ImFont* font =
  // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
  // IM_ASSERT(font != nullptr);

  // SAVE GLOBAL STATE
  window_ = window;
  gl_context_ = gl_context;
}

void UI::pollevs(bool &should_end) {
  // Poll and handle events (inputs, window resize, etc.)
  // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to
  // tell if dear imgui wants to use your inputs.
  // - When io.WantCaptureMouse is true, do not dispatch mouse input data to
  // your main application, or clear/overwrite your copy of the mouse data.
  // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input
  // data to your main application, or clear/overwrite your copy of the
  // keyboard data. Generally you may always pass all inputs to dear imgui,
  // and hide them from your application based on those two flags. [If using
  // SDL_MAIN_USE_CALLBACKS: call ImGui_ImplSDL3_ProcessEvent() from your
  // SDL_AppEvent() function]
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    ImGui_ImplSDL3_ProcessEvent(&event);
    if (event.type == SDL_EVENT_QUIT)
      should_end = true;
    if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
        event.window.windowID == SDL_GetWindowID(window_))
      should_end = true;
  }
};

void UI::begin_frame() {

  // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your
  // SDL_AppIterate() function]
  if (SDL_GetWindowFlags(window_) & SDL_WINDOW_MINIMIZED) {
    SDL_Delay(10);
    return;
  }

  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();
};

void UI::end_frame() {

  ImGuiIO &io = ImGui::GetIO();
  (void)io;

  // Rendering
  ImGui::Render();
  glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
  glClearColor(bg_clr_.x * bg_clr_.w, bg_clr_.y * bg_clr_.w,
               bg_clr_.z * bg_clr_.w, bg_clr_.w);
  glClear(GL_COLOR_BUFFER_BIT);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  SDL_GL_SwapWindow(window_);
};

void UI::clear() {
  // Cleanup
  // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your
  // SDL_AppQuit() function]
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DestroyContext(gl_context_);
  SDL_DestroyWindow(window_);
  window_ = nullptr;
  SDL_Quit();
}

void UI::draw_graph(const ssoc::graph::Graph &g) {
  if (show_vis_) {
    ImGui::SetNextWindowPos(ImVec2(385, 86), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(729, 571), ImGuiCond_FirstUseEver);
    vis_.show_window(g, show_vis_);
  }
}

void UI::draw_master_window(bool &generate_graph) {
  ImGui::SetNextWindowPos(ImVec2(31, 21), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(249, 189), ImGuiCond_FirstUseEver);
  ImGui::Begin("Master window", nullptr);

  ImGui::Checkbox("Show graph", &show_vis_);
  ImGui::Checkbox("Show config", &show_cfg_);
  ImGui::Checkbox("Show stepping", &show_step_);

  generate_graph = ImGui::Button("Generate graph");

  ImGui::End();
}

void UI::draw_config_window(Sim_Config &cfg) {
  if (show_cfg_) {
    ImGui::SetNextWindowPos(ImVec2(22, 233), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(352, 459), ImGuiCond_FirstUseEver);
    ImGui::Begin("Config window", &show_cfg_);
    draw(cfg);
    ImGui::End();
  }
}

void UI::draw(Sim_Config &cfg) {
  if (ImGui::CollapsingHeader("General configuration")) {
    draw(cfg.gga);
  }

  if (ImGui::CollapsingHeader("Visual configuration")) {
    draw(cfg.visual);
  }
}

void UI::draw(Vis_Config &cfg) { draw(cfg.gla); }

// === sim config

void UI::draw(Graph_Generation_Algorithm &gga) {
  constexpr const char *labels[] = {
      "Square Lattice 2D",
      "Dummy",
  };

  std::size_t idx = gga.index();

  for (std::size_t i = 0; i < std::size(labels); ++i) {
    if (ImGui::RadioButton(labels[i], idx == i)) {
      switch (i) {
      case 0:
        gga = gga_::Square_Lattice_2D{};
        break;
      case 1:
        gga = gga_::Dummy_GGA{};
        break;

      default:
        gga = gga_::Square_Lattice_2D{};
        break;
      }
    }
  }

  std::visit([this](auto &alg) { draw(alg); }, gga);
}

void UI::draw(gga_::Square_Lattice_2D &cfg) {
  ImGui::InputInt("Width", &cfg.width);
  ImGui::InputInt("Height", &cfg.height);
  ImGui::Checkbox("Circular on x axis", &cfg.circular_on_x);
  ImGui::Checkbox("Circular on y axis", &cfg.circular_on_y);
}

void UI::draw(gga_::Dummy_GGA &cfg) {
  ImGui::InputInt("Size", &cfg.size);
  ImGui::Checkbox("Bool", &cfg.boolean);
  ImGui::InputFloat("FT", &cfg.ft);
}

// === vis config

void UI::draw(Graph_Layout_Algorithm &gla) {
  constexpr const char *labels[] = {
      "Fruchterman Reingold 2D",
      "Dummy ui",
  };

  std::size_t idx = gla.index();

  for (std::size_t i = 0; i < std::size(labels); ++i) {
    if (ImGui::RadioButton(labels[i], idx == i)) {
      switch (i) {
      case 0:
        gla = gla_::Fruchterman_Reingold_2D{};
        break;
      case 1:
        gla = gla_::Dummy_GLA{};
        break;

      default:
        gla = gla_::Fruchterman_Reingold_2D{};
        break;
      }
    }
  }

  std::visit([this](auto &alg) { draw(alg); }, gla);
}

void UI::draw(gla_::Fruchterman_Reingold_2D &cfg) {
  std::size_t acc = cfg.accuracy;

  ImGui::Text("Accuracy");
  if (ImGui::RadioButton("High", acc == 0)) {
    acc = 0;
  }
  if (ImGui::RadioButton("Low", acc == 1)) {
    acc = 1;
  }
  if (ImGui::RadioButton("Auto", acc == 2)) {
    acc = 2;
  }

  cfg.accuracy = static_cast<gla_::FR2D_Accuracy>(acc);
}

void UI::draw(gla_::Dummy_GLA &cfg) { ImGui::InputInt("x", &cfg.x); }

bool UI::draw_stepping_control() {
  if (!show_step_) {
    return false;
  }
  ImGui::SetNextWindowPos(ImVec2(423, 16), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(175, 54), ImGuiCond_FirstUseEver);
  ImGui::Begin("Stepping control", &show_step_);

  bool step = ImGui::Button("Step");

  ImGui::End();

  return step;
}

} // namespace ssoc::ui
