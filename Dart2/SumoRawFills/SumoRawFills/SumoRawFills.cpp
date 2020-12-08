// SumoRawFills.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "FileGrabber.h"
#include "trade.h"
#include "CsvParser.h"
#include "TradeFactory.h"
#include "TradeSorter.h"
#include "CsvPrinter.h"

int main()
{
    CsvPrinter outPut("C:\\Users\\pflynn\\SumoRawFills202005.csv");
    FileGrabber fg("W:\\Dart_Operations\\Operations\\AuditLogData\\EquityTraffic\\2020", "202005");
    FileGrabber fgCplx("W:\\Dart_Operations\\Operations\\AuditLogData\\ComplexTraffic\\2020", "202005");
    FileGrabber fgOption("W:\\Dart_Operations\\Operations\\AuditLogData\\OptionsTraffic\\2020", "202005");
    FileGrabber fgFut("W:\\Dart_Operations\\Operations\\AuditLogData\\FuturesTraffic\\2020", "202005");
    std::cout << "Hello World!\n";
    TradeSorter<CsvPrinter> ts;
    TradeFactory<Trade, TradeSorter<CsvPrinter>>tf;
    CsvBillingTradeParser<TradeFactory<Trade, TradeSorter<CsvPrinter>>, TradeSorter<CsvPrinter>>cbtp(ts);
    fg.GetData(cbtp);
    fgCplx.GetData(cbtp);
    fgOption.GetData(cbtp);
    fgFut.GetData(cbtp);
    ts.printTrades(outPut);
    std::cout << "Hello World!\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
