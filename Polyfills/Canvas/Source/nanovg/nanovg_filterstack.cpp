#include "nanovg_filterstack.h"


void nanovg_filterstack::Render(std::function<void()> element)
{
    if (stackElements.empty())
    {
        // no filter
        element();
    }
    else
    {
        for (int i = 0; i < stackElements.size(); i++)
        {
            // get new render target
            // bind it
            // render element to it
        }
        // composite to canvas at the end
    }
}