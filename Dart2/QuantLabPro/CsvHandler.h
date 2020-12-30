#pragma once
#include <filesystem>
#include <fstream>
#include <vector>
#include "DataHandler.h"

using namespace std;

template <typename T>
class CsvHandler : public DataHandler<T>
{
public:
	CsvHandler(const string inputFileName, const string outputFileName)
		:m_inputPath(inputFileName), m_outputFile(outputFileName, ios::app),
		m_outFileName(outputFileName)
	{
	}
	~CsvHandler()
	{
		if (m_outputFile.is_open()) {
			m_outputFile.close();
		}
	}
	template<typename C>
	void outputData(const C& c);
	virtual void getData(vector<unique_ptr<T>>& dataStore, size_t maxVectorMem);
	template<typename callback>
	void getRawData(callback& c);

private:
	const filesystem::path m_inputPath;
	const string m_outFileName;
	ofstream m_outputFile;
};
/*THis is probably grossly inefficient but it just seemed so clean and simple.*/
template<typename T> template<typename C>
inline void CsvHandler<T>::outputData(const C& c) {
	if (m_outputFile.is_open()) {
		m_outputFile << c;
	}
	else {
		m_outputFile.open(m_outFileName, ios::app);
		m_outputFile << c;
	}
}
/*I dithered over the maxVector size. The comment about "larger than the memory on your box" made 
me want to put in some kind of backstop. I apologize that this is kind of a crufty C way of 
doing it but I figured the calling class could make the decision about how much data it would process.*/
template<typename T>
inline void CsvHandler<T>::getData(vector<unique_ptr<T>>& dStore, size_t maxVectorSize)
{
	if (exists(m_inputPath)) {
		ifstream input(m_inputPath.c_str(), ios::in);
		std::string reader;
		while (getline(input, reader) && maxVectorSize > 0) {
			dStore.push_back(make_unique<T>(reader));
			--maxVectorSize;
		}
		input.close();
	}
}
constexpr auto READ_CHUNK = 4086;

template<typename T> template<typename callback>
inline void CsvHandler<T>::getRawData(callback& ret)
{
	if (exists(m_inputPath) && filesystem::is_directory(m_inputPath)) {
		filesystem::recursive_directory_iterator iter(m_inputPath);
		filesystem::recursive_directory_iterator end;
		while (iter != end)
		{
			if (!filesystem::is_directory(iter->path)) {
				std::ifstream in(iter->path, std::ios::in | std::ios::binary);
				if (in)
				{
					char* contents = nullptr;
					in.seekg(0, std::ios::end);
					size_t fileSize = in.tellg();
					contents = new char[fileSize];
					in.seekg(0, std::ios::beg);
					in.read(contents, fileSize);
					in.close();
					ret.process(contents, fileSize);
					delete[] contents;
				}
				
			}
		}
	}
}
