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
	void printHeader(const std::string& header, uint64_t size);
	void printRow(Trade* t);
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

inline void CsvPrinter::printHeader(const std::string& header, uint64_t size)
{
	printToFile(header, ", Total Shares,", size, ",\n");
}
/*	auto trade = clearing.second;
			p.printRow(trade->romTime, trade->side, trade->symbol, trade->exshares,
						trade->dest, trade->clearingAcc, trade->liqCode);*/
void CsvPrinter::printRow(Trade* t)
{
	printToFile(t->romTime, ",", t->side, ",", t->symbol, ",", t->price, ",", t->exshares,
				",", getDestName(t->dest),	"," , t->clearingAcc , ",",  t->liqCode,
				",", t->exId, ",\n");
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
