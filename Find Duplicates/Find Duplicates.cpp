#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <iterator>
#include <fstream>

using namespace std;
namespace fs = std::filesystem;


//Functions:
vector<string> directoryFiles(const fs::path& path);
vector<string> findMatches(vector<string> filesList);
bool filesIdentical(string firstFilePath, string secondFilePath);
void printProgramUsage();
void printList(const vector<string>& vec);

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
		else
		{
			cout << "Found Matches: \n";
			printList(filesMatched);
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

	return fileMatches;

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
				if (!firstFileAddedToGroup)
				{
					fileMatches.push_back(filesList[i]);
					firstFileAddedToGroup = true;

				}

				fileMatches.push_back(filesList[fileToCompareIndex]);
			}

		}

		if (currentFileHasMatches)
		{
			fileMatches.push_back("");
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

	const size_t readBlockSize = 100;
	char firstContentBuffer[readBlockSize];
	char secondContentBuffer[readBlockSize];
	while (!firstFile.eof())
	{
		firstFile.read(firstContentBuffer, readBlockSize);
		secondFile.read(secondContentBuffer, readBlockSize);
		if (!equal(firstContentBuffer, firstContentBuffer + sizeof(firstContentBuffer), secondContentBuffer))
		{
			return false;
		}
	}
	return true;

}


void printProgramUsage()
{
	cout << "this program finds duplicated files in a given directory\n"
		"usage:\n"
		"findDub path\n";


}


void printList(const vector<string>& vec)
{
	copy(vec.begin(), vec.end(), ostream_iterator<string>(cout, "\n"));
}