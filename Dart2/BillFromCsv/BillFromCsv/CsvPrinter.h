#pragma once
#include <filesystem>
#include <string>
#include <fstream>


class CsvPrinter
{
public:
	CsvPrinter() = delete;
	CsvPrinter(const CsvPrinter&) = delete;
	CsvPrinter& operator=(const CsvPrinter&) = delete;

	CsvPrinter(const std::string& outputFile)
		:outputPath(outputFile), outputFile(outputFile, std::ios::app)
	{}
	~CsvPrinter() {
		if (outputFile.is_open()) {
			outputFile.close();
		}
	}

	void printBlank(int rows);
	void printHeader(const std::string& header);
	void printRow(const std::string& acc, uint16_t dest, const std::string& liqCode, uint64_t shares);
	void print(const std::string& acc);

private:
	void printToFile();
	template< typename T, typename ... Args>
	void printToFile(T& first, Args ... args);
	std::string getDestName(uint16_t destid);
	std::string outputPath;
	std::ofstream outputFile;
};

inline std::string CsvPrinter::getDestName(uint16_t destid)
{
	std::string destName;
	switch (destid) {
	case 146:
		destName = { "Nasdaq" };
		break;
	case 520:
	case 521:
		destName = { "ISE" };
		break;
	case 99:
		destName = { "CFE" };
		break;
	case 369:
	case 469:
		destName = { "Citadel" };
		break;
	case 490:
	case 492:
	case 94:
	case 590:
	case 496:
	case 292:
	case 95:
		destName = { "CME" };
		break;
	case 10:
		destName = { "Bats" };
		break;
	case 517:
	case 516:
		destName = { "ARCA" };
		break;
	case 298:
		destName = { "ICE" };
		break;
	case 626:
		destName = { "CBOE" };
		break;
	case 181:
		destName = { "BOX" };
		break;
	case 627:
		destName = { "GSOPT" };
		break;
	case 56:
		destName = { "GSEquity" };
		break;
	case 506:
		destName = { "NYSE" };
		break;
	}			
	return destName;
}

inline void CsvPrinter::printBlank(int rows)
{
	for (int i = 0; i < rows; ++i) {
		printToFile("\n");
	}
}

inline void CsvPrinter::printHeader(const std::string& header)
{
	printToFile(header, ",\n");
}
void CsvPrinter::printRow(const std::string& acc, uint16_t dest, const std::string& liqCode, uint64_t shares)
{
	printToFile(acc, ",", getDestName(dest),",", liqCode, "," ,shares, ",\n");
}

inline void CsvPrinter::print(const std::string& acc)
{
	printToFile(acc, ",\n");
}

inline void CsvPrinter::printToFile()
{}

template<typename T, typename ...Args>
inline void CsvPrinter::printToFile(T& first, Args ...args)
{
	if (outputFile.is_open()) {
		outputFile << first;
	}
	else {
		outputFile.open(outputPath, std::ios::app);
		outputFile << first;
	}
	printToFile(args ...);
}
