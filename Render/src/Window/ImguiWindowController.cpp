#include "ImguiWindowController.hpp"
#include "SDLImguiWindowController.hpp"
SampleRenderV2::ImguiWindowController* SampleRenderV2::ImguiWindowController::Instantiate(const std::shared_ptr<Window>* window)
{
    return new SDLImguiWindowController((const std::shared_ptr<SDLWindow>*)window);
}
