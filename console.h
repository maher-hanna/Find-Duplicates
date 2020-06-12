#pragma once
#include <string>


namespace Maher
{

namespace Console
{

        enum class TextColor {
                black = 30,
                red,
                green,
                yellow,
                blue,
                magenta,
                cyan,
                white
        };
 
        void printColoredText(const std::string& text, TextColor foregroundColor);

        void printColoredText(const std::string& text, TextColor foregroundColor,TextColor backgroundColor);





}

}
