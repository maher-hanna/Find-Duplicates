#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <cstring>
#include <cstdio>
#include <map>
#include "console.h"

using namespace std;
using namespace maher;
namespace fs = std::filesystem;

const size_t fileReadBlockSize = 1024;



//Functions:
vector<string> directoryFiles(const fs::path& path);
vector<string> matches(vector<string>& filesList);
vector<string> matchesInGroup(vector<string> fileGroup);
bool removeUnmatchedInGroup(vector<vector<size_t>>& fileGroup, char* buffers, vector<ifstream*>& filesStreams, streampos offset);
vector<string> matchesInFileSize(vector<string> filesList);

void printProgramUsage();
console::TextColor getNextGroupColor();
bool confirmDeleteDuplicates();
vector<size_t> duplicateIndexes(const vector<string> filesList);

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

		cout << "Searching.... \n";
		auto filesList = directoryFiles(path);
		auto filesMatched = matches(filesList);
		if (filesMatched.empty())
		{
			cout << "There is no matches\n";

		}
		else
		{
			auto duplicatesToDelete = duplicateIndexes(filesMatched);
			cout << "Found " << duplicatesToDelete.size() << " duplicates\n";
			if (confirmDeleteDuplicates())
			{
				for (auto& fileToDeleteIndex : duplicatesToDelete)
				{
					remove(filesMatched[fileToDeleteIndex].c_str());

				}
			}


		}



	}


	return 0;
}



vector<string> directoryFiles(const fs::path& path)
{
	vector<string> result;

	for (const auto& entry : fs::recursive_directory_iterator(path))
	{

		if (fs::is_regular_file(entry))
		{
			result.push_back(entry.path().u8string());
		}

	}
	return result;

}




vector<string> matches(vector<string>& filesList)
{
	vector<string> fileMatches;
	//if the folder containes no files or just one file return empty list of matches
	if (filesList.size() < 2)
	{
		return fileMatches;
	}

	//get a list of matched files with size then
	//get a list of it that containes files with matched first block
	vector<string> group;
	auto matchedSizes = matchesInFileSize(filesList);
	for (auto& path : matchedSizes)
	{
	
		if (path == "")
		{
			
			auto groupMatches = matchesInGroup(group);
			if (!groupMatches.empty())
			{
				
				fileMatches.insert(fileMatches.end(), groupMatches.begin(),
					groupMatches.end());
				
			}
			group.clear();
			continue;
		}
		group.push_back(path);

	}

	return fileMatches;


}


vector<string> matchesInFileSize(vector<string> filesList)
{

	vector<size_t> fileSizes;
	vector<string> fileSizeMatches;
	for (size_t i = 0; i < filesList.size(); i++)
	{
		fileSizes.push_back(filesystem::file_size(filesList[i]));
	}

	for (size_t primary = 0; primary < (fileSizes.size() - 1); primary++)
	{
		//group potential identecal files in together and separate between them with blank item
		bool currentFileMayHasMatches = false;
		bool firstFileAddedToGroup = false;
		auto secondary = primary + 1;
		while (secondary < fileSizes.size())
		{

			if (fileSizes[primary] == fileSizes[secondary])
			{
				currentFileMayHasMatches = true;
				if (!firstFileAddedToGroup)
				{
					fileSizeMatches.push_back(filesList[primary]);
					firstFileAddedToGroup = true;
				}
				fileSizeMatches.push_back(filesList[secondary]);
				filesList.erase(filesList.begin() + secondary);
				fileSizes.erase(fileSizes.begin() + secondary);

			}
			else
			{
				secondary++;
			}
		}
		if (currentFileMayHasMatches)
		{
			fileSizeMatches.push_back("");
			currentFileMayHasMatches = false;
		}

	}
	return fileSizeMatches;
}

vector<string> matchesInGroup(vector<string> fileGroup)
{
	vector<string> matchesIngroup;
	char* buffers = new char[fileReadBlockSize * fileGroup.size()]{ 0 };
	vector<ifstream*> inputFilesStreams;
	vector<vector<size_t>> stillMatching;

	for (size_t currentFile = 0; currentFile < fileGroup.size(); currentFile++)
	{

		std::ifstream* f = new std::ifstream(fileGroup[currentFile].c_str(), ios::binary | ios::in);

		inputFilesStreams.push_back(f);
	}


	//Fill the structure with indexes of every possible matches in file group
	for (size_t primaryIndex = 0; primaryIndex < (fileGroup.size() - 1); primaryIndex++)
	{
		vector<size_t> primaryFileMatches;
		for (size_t secondaryIndex = (primaryIndex + 1); secondaryIndex < fileGroup.size(); secondaryIndex++)
		{
			primaryFileMatches.push_back(secondaryIndex);

		}
		stillMatching.push_back(primaryFileMatches);
	}

	size_t readOffset = 0;

	bool stillAreMatches = true;
	do
	{
		stillAreMatches = removeUnmatchedInGroup(stillMatching, buffers, inputFilesStreams, readOffset);
		readOffset++;
	} while (stillAreMatches && !(inputFilesStreams.back()->eof()));

	//Close files end clean memeory
	for (auto streamIndex = 0; streamIndex < inputFilesStreams.size(); streamIndex++)
	{
		inputFilesStreams[streamIndex]->close();
		delete inputFilesStreams[streamIndex];
	}
	inputFilesStreams.clear();

	delete[] buffers;
	//--------------

	vector<string> result;


	//remove duplicated groups


	for (auto primary = 0; primary < stillMatching.size() - 1; primary++)
	{
		for (auto secondary = primary + 1; secondary < stillMatching.size(); secondary++)
		{
			if (includes(stillMatching[primary].begin(), stillMatching[primary].end(),
				stillMatching[secondary].begin(), stillMatching[secondary].end()))
			{
				stillMatching[secondary].clear();
			}
		}
	}
	//-----------

	//print matched files with colors
	console::TextColor currentGroupColor;

	for (auto primary = 0; primary < stillMatching.size(); primary++)
	{
		if (stillMatching[primary].empty())
		{
			continue;
		}
		else
		{
			currentGroupColor = getNextGroupColor();
			result.push_back(fileGroup[primary]);
			console::printColoredText(result.back(), console::TextColor::white, currentGroupColor);
			cout << '\n';
			for (auto secondary = 0; secondary < stillMatching[primary].size(); secondary++)
			{
				result.push_back(fileGroup[stillMatching[primary][secondary]]);
				console::printColoredText(result.back(), console::TextColor::white, currentGroupColor);
				cout << '\n';
			}
			result.push_back("");
			cout << '\n';
		}

	}

	return result;
}


bool removeUnmatchedInGroup(vector<vector<size_t>>& fileGroup, char* buffers, vector<ifstream*>& filesStreams, streampos offset)
{
	//match the current sector of every file in group
	//and remove every tow unmatched files


	char* currentBuffer = buffers;
	memset(buffers, 0, fileReadBlockSize * filesStreams.size());
	for (size_t fileIndexInBuffer = 0; fileIndexInBuffer < filesStreams.size(); fileIndexInBuffer++)
	{
		filesStreams[fileIndexInBuffer]->seekg(offset * fileReadBlockSize);
		filesStreams[fileIndexInBuffer]->read(currentBuffer, fileReadBlockSize);
		currentBuffer += fileReadBlockSize;
	}


	bool isStillMatchingEmpty = false;
	size_t numberOfRemainingMatching = fileGroup.size();

	for (auto primary = 0; primary < fileGroup.size(); primary++)
	{

		auto secondary = fileGroup[primary].begin();
		while (secondary != fileGroup[primary].end())
		{
			if (!equal(buffers + (primary * fileReadBlockSize), buffers + (primary * fileReadBlockSize) + fileReadBlockSize,
				buffers + ((*secondary) * fileReadBlockSize)))
			{
				secondary = fileGroup[primary].erase(secondary);


			}
			else
			{
				secondary++;
			}

		}
		if (fileGroup[primary].empty())
		{
			
			numberOfRemainingMatching--;
		}
	}

	if (numberOfRemainingMatching == 0)
	{
		return false;
	}
	return true;


}

void printProgramUsage()
{
	cout << "This program finds duplicated files in a given directory\n\n"
		"Usage:\n"
		"findDuplicates path\n";


}

console::TextColor getNextGroupColor()
{

	static int index = 0;

	index++;
	if (index > 5)
	{
		index = 0;
	}

	console::TextColor color = console::TextColor::black;

	switch (index)
	{
	case 0:
		color = console::TextColor::black;
		break;
	case 1:
		color = console::TextColor::blue;
		break;
	case 2:
		color = console::TextColor::cyan;
		break;
	case 3:
		color = console::TextColor::green;
		break;
	case 4:
		color = console::TextColor::magenta;
		break;
	case 5:
		color = console::TextColor::red;
		break;
	case 6:
		color = console::TextColor::yellow;
		break;

	}

	return color;

}


bool confirmDeleteDuplicates()
{
	bool answer = false;
	cout << "Do you want to delete the duplicated files\n";
	cout << "yes/y to delete or no/n : " << endl;
	string input;
	cin >> input;
	if (input == "y" || input == "yes")
	{
		answer = true;
	}
	return answer;
}

vector<size_t> duplicateIndexes(const vector<string> filesList)
{
	bool skipFirstFileInGroup = false;
	vector<size_t> indexes;
	for (auto currenFile = 1; currenFile < filesList.size(); currenFile++)
	{

		if (filesList[currenFile] == "")
		{
			skipFirstFileInGroup = true;

			continue;
		}
		else
		{
			if (skipFirstFileInGroup)
			{
				skipFirstFileInGroup = false;
				continue;
			}
			indexes.push_back(currenFile);
		}
	}

	return indexes;
}
