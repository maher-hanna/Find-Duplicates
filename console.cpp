#ifdef _WIN32
#include <Windows.h>
#endif // _WIN32

#include "console.h"
#include <iostream>

#ifdef _WIN32
namespace {
	constexpr WORD getWindowsCodeOfColor(Maher::Console::TextColor textColor)
	{
		switch (textColor)
		{
		case Maher::Console::TextColor::black:
			return 0;
			break;
		case Maher::Console::TextColor::red:
			return FOREGROUND_RED;
			break;
		case Maher::Console::TextColor::green:
			return FOREGROUND_GREEN;
			break;
		case Maher::Console::TextColor::yellow:
			return 14;
			break;
		case Maher::Console::TextColor::blue:
			return FOREGROUND_BLUE;
			break;
		case Maher::Console::TextColor::magenta:
			return 5;
			break;
		case Maher::Console::TextColor::cyan:
			return 3;
			break;
		case Maher::Console::TextColor::white:
			return 15;
			break;
		
		}
	}

	
}
#endif

namespace Maher
{
namespace Console
{

void printColoredText(const std::string& text, TextColor foregroundColor
,TextColor backgroundColor)
{
#ifdef _WIN32
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	WORD currentConsoleAttr;
	CONSOLE_SCREEN_BUFFER_INFO   screenBufferInfo;
	//save the current text color
	if (GetConsoleScreenBufferInfo(hConsole, &screenBufferInfo))
		currentConsoleAttr = screenBufferInfo.wAttributes;

	auto bColor = getWindowsCodeOfColor(backgroundColor);
	auto fColor = getWindowsCodeOfColor(foregroundColor);
	WORD color = ((bColor & 0x0F) << 4) + (fColor & 0x0F);

	SetConsoleTextAttribute(hConsole, color);
	std::cout << text;

	//restore the previous color
	SetConsoleTextAttribute(
		hConsole,
		currentConsoleAttr);

#else
	//"\033[0m" resets the color to default color
	//add 10 to background color because the code of black background color starts at 40
	std::cout << "\033[" << int(foregroundColor) << ";" << int(backgroundColor) + 10 <<  "m"
                << text << "\033[0m";
#endif


}

void printColoredText(const std::string& text, TextColor foregroundColor)
{
#ifdef _WIN32
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	WORD currentConsoleAttr;
	CONSOLE_SCREEN_BUFFER_INFO   screenBufferInfo;
	//save the current text color
	if (GetConsoleScreenBufferInfo(hConsole, &screenBufferInfo))
		currentConsoleAttr = screenBufferInfo.wAttributes;

	SetConsoleTextAttribute(hConsole, getWindowsCodeOfColor(foregroundColor));
	std::cout << text;

	//restore the previous color
	SetConsoleTextAttribute(
		hConsole,
		currentConsoleAttr);

#else
	//"\033[0m" resets the color to default color
	//add 10 to background color because the code of black background color starts at 40
	std::cout << "\033[" << int(foregroundColor)  <<  "m"
                << text << "\033[0m";
#endif


}


}
}


