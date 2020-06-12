#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <chrono>
#include "console.h"

using namespace std;
using namespace Maher;
namespace fs = std::filesystem;


//Functions:
vector<string> directoryFiles(const fs::path& path);
vector<string> findMatches(vector<string> filesList);
bool filesIdentical(string firstFilePath, string secondFilePath);
void printProgramUsage();
Console::TextColor getNextGroupColor();
//////


int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printProgramUsage();
		return 1;
	}
	else
	{
		string directory = argv[1];
		filesystem::path path(directory);

		if(!fs::exists(path))
		{
			cout << "The given folder doesn't exist\n";
			return 1;
		}

		if (!fs::is_directory(path))
		{
			cout << "The given path is not folder\n";
			return 1;
		}

		auto filesList = directoryFiles(path);
		auto filesMatched = findMatches(filesList);
		if (filesMatched.empty())
		{
			cout << "There is no matches\n";

		}
		


	}

	
	return 0;
}



vector<string> directoryFiles(const fs::path& path)
{
	vector<string> result;

	for (const auto& entry : fs::directory_iterator(path))
	{
		const auto& currentEntry = entry.path();
		if (fs::is_directory(currentEntry))
		{
			auto currentDirectoryFiles = directoryFiles(currentEntry);
			result.insert(result.end(), currentDirectoryFiles.begin(), currentDirectoryFiles.end());
		}
		else if (fs::is_regular_file(currentEntry))
		{
			result.push_back(currentEntry.u8string());
		}

	}
	return result;

}



vector<string> findMatches(vector<string> filesList)
{
	//group ech group of identical files together in the list
	//by separating them from other group by an empty element
	vector<string> fileMatches;

	//if the folder containes no files or just one file return empty list of matches
	if (filesList.size() < 2)
	{
		return fileMatches;
	}

	bool foundMatchesPrinted = false;
	Console::TextColor currentGroupColor;

	for (auto i = (filesList.size() - 1); i > 0; i--)
	{

		//add the file to compare only once at the begining of the group of matched files
		bool currentFileHasMatches = false;
		bool firstFileAddedToGroup = false;
	
		for (long fileToCompareIndex = (i - 1); fileToCompareIndex >= 0; fileToCompareIndex--)
		{

			if (filesIdentical(filesList[i], filesList[fileToCompareIndex]))
			{
				currentFileHasMatches = true;
				if (!foundMatchesPrinted)
				{
					cout << "Found matches: \n\n";
					foundMatchesPrinted = true;
				}
				

				if (!firstFileAddedToGroup)
				{
					currentGroupColor = getNextGroupColor();
					fileMatches.push_back(filesList[i]);
					Console::printColoredText(filesList[i], Console::TextColor::white, currentGroupColor);
					cout << endl;
					firstFileAddedToGroup = true;

				}

				fileMatches.push_back(filesList[fileToCompareIndex]);
				Console::printColoredText(filesList[fileToCompareIndex], Console::TextColor::white, currentGroupColor);
				cout << endl;

			}

		}

		if (currentFileHasMatches)
		{
			fileMatches.push_back("");
			cout << '\n';
		}
		currentFileHasMatches = false;


	}

	return fileMatches;
}

bool filesIdentical(string firstFilePath, string secondFilePath)
{
	ifstream firstFile(firstFilePath, ios::ate | ios::binary);
	ifstream secondFile(secondFilePath, ios::ate | ios::binary);

	auto firstFileSize = firstFile.tellg();
	auto secondFileSize = secondFile.tellg();
	if (firstFileSize != secondFileSize)
	{
		return false;
	}

	const size_t readBlockSize = 1024;
	char firstContentBuffer[readBlockSize] = {0};
	char secondContentBuffer[readBlockSize] = {0};
	while (!firstFile.eof())
	{
		firstFile.read(firstContentBuffer, readBlockSize);
		secondFile.read(secondContentBuffer, readBlockSize);
		if (!equal(firstContentBuffer, firstContentBuffer + sizeof(firstContentBuffer), secondContentBuffer))
		{
			return false;
		}
	}
	firstFile.close();
	secondFile.close();
	return true;

}


void printProgramUsage()
{
	cout << "this program finds duplicated files in a given directory\n\n"
		"usage:\n"
		"findDuplicates path\n";


}


Console::TextColor getNextGroupColor()
{

	static int index = 0;

	index++;
	if (index > 5)
	{
		index = 0;
	}

	Console::TextColor color;

	switch (index)
	{
	case 0:
		color = Console::TextColor::black;
		break;
	case 1:
		color = Console::TextColor::blue;
		break;
	case 2:
		color = Console::TextColor::cyan;
		break;
	case 3:
		color = Console::TextColor::green;
		break;
	case 4:
		color = Console::TextColor::magenta;
		break;
	case 5:
		color = Console::TextColor::red;
		break;
	case 6:
		color = Console::TextColor::yellow;
		break;
	
	}

	return color;
	
}
