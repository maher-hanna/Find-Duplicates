#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <chrono>

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
	auto start = std::chrono::high_resolution_clock::now();
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

	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	cout << "Duration in milliseconds " << duration.count() << endl;
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
					fileMatches.push_back(filesList[i]);
					cout << filesList[i] << endl;
					
					firstFileAddedToGroup = true;

				}

				fileMatches.push_back(filesList[fileToCompareIndex]);
				cout << filesList[fileToCompareIndex] << endl;

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


void printList(const vector<string>& vec)
{
	copy(vec.begin(), vec.end(), ostream_iterator<string>(cout, "\n"));
}
