#pragma once

#include <SDL3/SDL.h>
#include <imgui.h>
namespace ssoc::ui {

class Window_Context {
private:
  SDL_Window *window_ = nullptr;
  SDL_GLContext gl_context_;

public:
  // initialize everything the UI (ImGui, OpenGL & SDL) need
  void init();
  // clear all data after ending the mainloop
  void shutdown();

  // poll events in every iteration of main loop
  // set given bool to true if app should end
  void pollevs(bool &should_end);

  // begin the ImGui frame
  void begin_frame();
  // end the ImGui frame & render
  void end_frame(const ImVec4 &clear_colour);
};

} // namespace ssoc::ui
