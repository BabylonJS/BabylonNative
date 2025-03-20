#include "nanovg_filterstack.h"
#include <regex>

std::regex blurRegex(R"(blur\((\d*\.?\d+)(px|rem)?\)|blur\(\))");

nanovg_filterstack::nanovg_filterstack()
{
    // create shaders used by the different elements
    // TODO: do I have to pass gl into here?
}

bool nanovg_filterstack::ValidString(const std::string& string)
{
    std::smatch match;
    return std::regex_match(string, match, blurRegex);
}

void nanovg_filterstack::ParseString(const std::string& string)
{
    stackElements.clear();

    std::smatch match;
    if (std::regex_match(string, match, blurRegex))
    {
        if ( match.size() > 1 && match[1].matched)
        {
            int radius = std::stoi(match[1].str());
            std::string unit = match[2].matched ? match[2].str() : "px";
            std::transform(unit.begin(), unit.end(), unit.begin(), ::tolower);

            if (unit != "px")
            {
                // TODO: convert non-px radius
            }

            StackElement element = {};
            element.blurElement = {radius, radius};
            stackElements.push_back(element);
        }
        else
        {
            // defaults to blur(0)
            StackElement element = {};
            element.blurElement = {0, 0};
            stackElements.push_back(element);
        }
    }
}

void nanovg_filterstack::Render(std::function<void()> element)
{
    if (stackElements.empty())
    {
        // no filter
        element();
    }
    else
    {
        // TODO: move over Cedric's glnvg__triangles hacks from nanovg_babylon.cpp
        // TODO: Instead of hackyFrameBuffer, implement render target manager
        // TODO: may need to pass down gl + call from glnvg__triangles + glnvg__stroke?
        for (int i = 0; i < stackElements.size(); i++)
        {
            // get new render target
            // bind it
            // render element to it
        }
        // composite to canvas at the end
    }
}