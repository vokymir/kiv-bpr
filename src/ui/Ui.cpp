#include "Ui.hpp"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl3.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <cmath>
#include <format>
#include <stdexcept>

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
  SDL_Quit();
}

void UI::draw_demo() {
  bool open = true;
  ImGui::ShowDemoWindow(&open);
}
static inline ImVec2 WorldToScreen(const std::pair<double, double> &world_pos,
                                   const ImVec2 &origin, const ImVec2 &pan,
                                   float zoom) {
  return ImVec2(origin.x + pan.x + (float)world_pos.first * zoom,
                origin.y + pan.y + (float)world_pos.second * zoom);
}

void UI::draw_graph(const ssoc::graph::Graph &g) {
  ImGui::Begin("Graph Visualization");

  ImVec2 origin = ImGui::GetCursorScreenPos();
  ImVec2 size = ImGui::GetContentRegionAvail();
  ImVec2 center = ImVec2(origin.x + size.x * 0.5f, origin.y + size.y * 0.5f);
  ImVec2 footer = ImVec2(origin.x + size.x, origin.y + size.y);

  // Capture input
  ImGui::InvisibleButton("##canvas", size, ImGuiButtonFlags_MouseButtonRight);
  const bool is_hovered = ImGui::IsItemHovered();
  const bool is_active = ImGui::IsItemActive();
  ImGuiIO &io = ImGui::GetIO();

  // 1. Zoom Logic (Start 'zoom' at ~200.0f in your constructor for [-1,1]
  // layouts!)
  if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
    pan.x += io.MouseDelta.x;
    pan.y += io.MouseDelta.y;
  }

  if (is_hovered && io.MouseWheel != 0.0f) {
    float old_zoom = zoom;
    zoom *= (io.MouseWheel > 0) ? 1.1f : 0.9f;

    // Pan adjustment to zoom toward mouse
    pan.x = (io.MousePos.x - center.x) -
            ((io.MousePos.x - center.x) - pan.x) * (zoom / old_zoom);
    pan.y = (io.MousePos.y - center.y) -
            ((io.MousePos.y - center.y) - pan.y) * (zoom / old_zoom);
  }

  // 2. Rendering
  ImDrawList *draw_list = ImGui::GetWindowDrawList();
  draw_list->PushClipRect(origin, footer, true);

  // Transform: Screen Center + Pan + (WorldCoord * Zoom)
  auto to_screen = [&](std::pair<double, double> pos) {
    return ImVec2(center.x + pan.x + (float)pos.first * zoom,
                  center.y + pan.y + (float)pos.second * zoom);
  };

  // Draw Edges
  for (int v = 0; v < g.node_count_; ++v) {
    ImVec2 p1 = to_screen(g.positions_[v]);
    int start = g.neighbour_idxs_[v];
    int end = (v + 1 < g.node_count_) ? g.neighbour_idxs_[v + 1]
                                      : (int)g.neighbours_.size();

    for (int i = start; i < end; ++i) {
      draw_list->AddLine(p1, to_screen(g.positions_[g.neighbours_[i]]),
                         IM_COL32(150, 150, 150, 255));
    }
  }

  // Draw Nodes
  for (int v = 0; v < g.node_count_; ++v) {
    // We keep the node radius constant-ish so they don't get giant
    // Or use: 5.0f * (zoom / 200.0f) if you want them to scale
    draw_list->AddCircleFilled(to_screen(g.positions_[v]), 5.0f,
                               IM_COL32(200, 50, 50, 255));
  }

  draw_list->PopClipRect();
  ImGui::End();
}

} // namespace ssoc::ui
