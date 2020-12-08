#pragma once
#include <filesystem>
#include <fstream>
#include <string>
class FileGrabber
{
public:
	FileGrabber() = delete;
	FileGrabber(const FileGrabber&) = delete;
	FileGrabber& operator=(const FileGrabber&) = delete;
	FileGrabber(const std::string& path, const std::string& month)
		:baseDir(path), month(month)
	{}
	template<typename FileParser>
	void GetData(FileParser& cb);
private:
	bool FoundMonthStr(const std::string& path);
	std::filesystem::path baseDir;
	std::string month;
};
bool FileGrabber::FoundMonthStr(const std::string& path)
{
	return (path.find(month) != std::string::npos);
}
template<typename FileParser>
inline void FileGrabber::GetData(FileParser& cb)
{
	std::filesystem::recursive_directory_iterator curFile(baseDir);
	std::filesystem::recursive_directory_iterator end;
	while (curFile != end) {
		std::string name(curFile->path().string());
		if (curFile->is_directory()) {
			//do nothing
		} else if (FoundMonthStr(name)) {
			std::ifstream input(name.c_str(), std::ios::in);
			std::string reader;
			while (getline(input, reader)) {
					cb.parse(reader);	
			}
			input.close();
		}
		++curFile;
	}
}
