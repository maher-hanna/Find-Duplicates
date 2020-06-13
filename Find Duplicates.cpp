#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <cstring>
#include "console.h"

using namespace std;
using namespace Maher;
namespace fs = std::filesystem;

const size_t fileReadBlockSize = 1024;



class File
{
public:
	streampos size;
	char content[fileReadBlockSize];
	string path;

	File() : size(0), content{}, path("")
	{
		
	}
	
};




//Functions:
vector<string> directoryFiles(const fs::path& path);
vector<string> findMatches(vector<string> & filesList);
bool filesIdentical(const File & primaryFile, const string &seconderyFilePath);
void printProgramUsage();
Console::TextColor getNextGroupColor();
File readFile(const string &path);
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

		if (!fs::exists(path))
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



vector<string> findMatches(vector<string> & filesList)
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

	auto primaryFile = filesList.begin();
	while (primaryFile != filesList.end() - 1)
	{

		//add the file to compare only once at the begining of the group of matched files
		bool currentFileHasMatches = false;
		bool firstFileAddedToGroup = false;
		auto primary = readFile(*primaryFile);

		auto seconderyFile = primaryFile + 1;
		while (seconderyFile != filesList.end())
		{

			if (filesIdentical(primary, *seconderyFile))
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
					fileMatches.push_back(*primaryFile);
					Console::printColoredText(*primaryFile, Console::TextColor::white, currentGroupColor);
					cout << endl;
					firstFileAddedToGroup = true;

				}

				fileMatches.push_back(*seconderyFile);
				Console::printColoredText(*seconderyFile, Console::TextColor::white, currentGroupColor);
				cout << endl;
				seconderyFile = filesList.erase(seconderyFile);
			}


			seconderyFile++;

		}

		if (currentFileHasMatches)
		{
			fileMatches.push_back("");
			cout << '\n';
		}

		currentFileHasMatches = false;

		primaryFile++;

	}

	return fileMatches;
}

bool filesIdentical(const File & primaryFile, const string & seconderyFilePath)
{
	ifstream seconderyFileStream(seconderyFilePath, ios::ate | ios::binary);

	auto seconderyFileSize = seconderyFileStream.tellg();

	if (primaryFile.size != seconderyFileSize)
	{
		return false;
	}

	seconderyFileStream.seekg(0);

	char primaryContentBuffer[fileReadBlockSize] = { 0 };
	char seconderyContentBuffer[fileReadBlockSize] = { 0 };
	
	seconderyFileStream.read(seconderyContentBuffer, fileReadBlockSize);
	if (!equal(primaryFile.content, primaryFile.content + fileReadBlockSize, seconderyContentBuffer))
	{
		return false;
	}

	//if primary file first block matches secondery file first block 
	//continue reading the rest of blocks of the primary file
	//and compare it to the secondery file blocks;
	ifstream primaryFileStream(primaryFile.path, ios::binary);
	primaryFileStream.seekg(fileReadBlockSize, ios::beg);
	

	while (!seconderyFileStream.eof())
	{
		primaryFileStream.read(primaryContentBuffer, fileReadBlockSize);
		seconderyFileStream.read(seconderyContentBuffer, fileReadBlockSize);
		

		if (!equal(primaryContentBuffer, primaryContentBuffer + fileReadBlockSize, seconderyContentBuffer))
		{
			return false;
		}

	
	}
	primaryFileStream.close();
	seconderyFileStream.close();
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


File readFile(const string& path)
{
	File file;
	ifstream fileStream(path, ios::ate | ios::binary);
	file.size = fileStream.tellg();
	fileStream.seekg(0);
	
	fileStream.read(file.content, fileReadBlockSize);
	file.path = path;
	return file;




}