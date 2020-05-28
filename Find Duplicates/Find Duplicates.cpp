#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>

using namespace std;
namespace fs = std::filesystem;


//Functions:
void printProgramUsage();
vector<string> directoryFiles(const fs::path & path);
void print(vector<string> vec);
vector<string> findMatches(vector<string> filesList);
bool filesMatches(string firstFilePath, string secondFilePath);

//////


int main(int argc, char * argv[])
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
        auto filesList = directoryFiles(path);
        print(findMatches(filesList));


	}
	return 0;
}


void printProgramUsage()
{
	cout << "this program finds duplicated files in a given directory\n"
		"usage:\n"
		"findDub path\n";


}


vector<string> directoryFiles(const fs::path & path)
{
    vector<string> result;
    if (fs::is_directory(path))
    {
        for (const auto& entry : fs::directory_iterator(path))
        {
            const auto& currentEntry = entry.path();
            if (fs::is_directory(currentEntry))
            {
                auto currentDirectoryFiles = directoryFiles(currentEntry);
                result.insert(result.end(),currentDirectoryFiles.begin(),currentDirectoryFiles.end());
            }
            else if (fs::is_regular_file(currentEntry))
            {
                result.push_back(currentEntry.u8string());
            }

        }
        return result;
    }
}

void print(vector<string> vec)
{
    copy(vec.begin(), vec.end(), ostream_iterator<string>(cout, "\n"));
}

vector<string> findMatches(vector<string> filesList)
{
    vector<string> fileMatches;
    for (auto i = (filesList.size() - 1); i > 0; i--)
    {
        for (auto fileToCompareIndex = (i - 1); fileToCompareIndex >= 0; fileToCompareIndex--)
        {
            if (filesMatches(filesList[i], filesList[fileToCompareIndex]))
            {
                fileMatches.push_back(filesList[i]);
                fileMatches.push_back(filesList[fileToCompareIndex]);
            }

        }
        if (!filesList.back().empty())
        {
            filesList.push_back("");
        }
    }

    return vector<string>();
}

bool filesMatches(string firstFilePath, string secondFilePath)
{
    return false;
}
