#include <iostream>
#include "DataHandler.h"
#include "DataEvent.h"
#include "TradeEvent.h"
#include "CsvHandler.h"
#include "PerSymbolDailyTrades.h"
#include "PerClearingAccountTrades.h"
#include "ClearingTradeEvent.h"
#include "TradeCollator.h"
#include "Validator.h"
#include "MatchingEngine.h"
#include "Order.h"
#include "StdOutPrinter.h"
#include "MessageFactory.h"
#include "MessageReaper.h"



/*Originally I was going to read the input and output off of argv, but then I got lazy
and copied the body of this funtion from my endToEnd functional test.*/
int main(int argc, char** argv)
{
    MessageFactory<Order> mf;
    MessageFactory<Cancel> mcf;
    MessageFactory<Modify> mmod;
    OrdMessageValidator omv;
    StdOutPrinter<Order> sop;
    MessageReaper rep;
    MatchingEngine<StdOutPrinter<Order>,MessageReaper> matcher(sop, rep);
    std::string str1 = { "BUY GFD 200 10 order1" };
    std::string str2 = { "SELL GFD 220 20 order2" };
    std::string str3 = { "BUY GFD 210 30 order3" };
    std::string str4 = { "SELL GFD 240 50 order4" };
    Order* ord1 = mf.create(str1);
    matcher.processOrd(ord1);
    Order* ord2 = mf.create(str2);
    matcher.processOrd(ord2);
    Order* ord3 = mf.create(str3);
    matcher.processOrd(ord3);
    Order* ord4 = mf.create(str4);
    matcher.processOrd(ord4);
    Order* dup = mf.create(str4);
    matcher.processOrd(dup);
    std::string samePrice = { "BUY GFD 200 30 order500" };
    Order* same = mf.create(samePrice);
    matcher.processOrd(same);

    std::string wiffer = { "BUY IOC 200 90 order1" };
    Order* iowif = mf.create(wiffer);
    matcher.processOrd(iowif);
    matcher.printCurrentState();
    std::cout << "\n Should have 40 @ 200\n\n";


    std::string str5 = { "CANCEL order4" };
    Cancel* can = mcf.create(str5);
    matcher.processCancel(can);
    Cancel* canCan = mcf.create(str5);
    matcher.processCancel(canCan);
    matcher.printCurrentState();
    std::string str6 = { "MODIFY order2 SELL 210 20" };
    Modify* mod = mmod.create(str6);
    matcher.processModify(mod);
    matcher.printCurrentState();
    std::cout << "\n\n";
    std::string str7 = { "CANCEL order1" };
    std::string str8 = { "CANCEL order3" };
    Cancel* can1 = mcf.create(str7);
    Cancel* can2 = mcf.create(str8);
    matcher.processCancel(can1);
    matcher.printCurrentState();
    matcher.processCancel(can2);
    matcher.printCurrentState();
    std::cout << "\n\n\n";
    std::string str10 = { "BUY GFD 200 10 order10" };
    std::string str30 = { "BUY GFD 210 30 order30" };
    std::string str40 = { "SELL GFD 200 50 order40" };
    Order* o10 = mf.create(str10);
    matcher.processOrd(o10);
    Order* o20 = mf.create(str30);
    matcher.processOrd(o20);
    matcher.printCurrentState();
    Order* o40 = mf.create(str40);
    matcher.processOrd(o40);
    matcher.printCurrentState();
    std::string str11 = { "CANCEL order40" };
    Cancel* can3 = mcf.create(str11);
    matcher.processCancel(can3);
    Cancel* can5 = mcf.create("CANCEL order30");
    Cancel* can6 = mcf.create("CANCEL order10");
    Cancel* can7 = mcf.create("CANCEL order1");
    Cancel* can8 = mcf.create("CANCEL order500");
    matcher.processCancel(can5);
    matcher.processCancel(can6);
    matcher.processCancel(can7);
    matcher.processCancel(can8);
    matcher.printCurrentState();

    std::cout << "Starting Test 17: \n\n\n";

    std::string test24 = { "BUY GFD 5900 30 order0BUY IOC 4200 41 order1PRINTMODIFY order1 SELL 1 31CANCEL order1BUY IOC 2300 58 order2CANCEL order1SELL GFD 8300 85 order3SELL IOC 300 61 order4BUY IOC 3500 7 order5SELL IOC 9700 30 order6CANCEL order0MODIFY order0 BUY 1 17CANCEL order1BUY GFD 5900 55 order7BUY IOC 3200 11 order8PRINTBUY GFD 500 33 order9MODIFY order5 BUY 1 96CANCEL order1MODIFY order2 SELL 1 14MODIFY order6 SELL 1 39MODIFY order4 BUY 1 60MODIFY order7 SELL 1 25CANCEL order5SELL IOC 9600 10 order10BUY IOC 2500 53 order11PRINTSELL IOC 3300 78 order12MODIFY order5 BUY 1 47MODIFY order8 SELL 1 93PRINTPRINTSELL GFD 200 93 order13CANCEL order5SELL GFD 8500 24 order14BUY IOC 2900 34 order15PRINTMODIFY order7 SELL 1 74MODIFY order3 SELL 1 96BUY IOC 4500 55 order16PRINTSELL GFD 6500 12 order17MODIFY order11 BUY 1 68SELL GFD 1100 73 order18PRINTMODIFY order2 SELL 1 90CANCEL order6CANCEL order14PRINTCANCEL order4" };
    std::string test22 = { "BUY IOC 2300 52 order0BUY IOC 3900 97 order1SELL IOC 6700 20 order2SELL GFD 9400 56 order3SELL GFD 1900 34 order4SELL GFD 7600 48 order5PRINTPRINTCANCEL order0BUY GFD 900 56 order6MODIFY order0 BUY 1 84SELL GFD 7800 31 order7MODIFY order4 BUY 1 97PRINTBUY GFD 5200 32 order8MODIFY order2 BUY 1 14CANCEL order1MODIFY order7 SELL 1 44SELL GFD 2600 35 order9MODIFY order6 BUY 1 10PRINTPRINTBUY GFD 2400 0 order10SELL IOC 7800 60 order11PRINTMODIFY order5 SELL 1 95BUY IOC 4400 50 order12BUY GFD 3600 88 order13SELL IOC 8600 23 order14MODIFY order0 SELL 1 49BUY IOC 9000 78 order15PRINTPRINTPRINTPRINTBUY GFD 5800 13 order16BUY GFD 2900 5 order17MODIFY order1 SELL 1 73MODIFY order17 BUY 1 97BUY IOC 5400 2 order18MODIFY order2 BUY 1 64PRINTSELL GFD 4900 46 order19CANCEL order3CANCEL order5PRINTCANCEL order5PRINTCANCEL order18PRINTCANCEL order19" };
    std::string test21 = { "BUY GFD 3300 71 order0PRINTMODIFY order0 SELL 1 82PRINTCANCEL order0BUY GFD 2500 85 order1BUY GFD 4500 36 order2SELL IOC 5400 97 order3MODIFY order2 BUY 1 10MODIFY order3 BUY 1 75SELL GFD 3300 5 order4PRINTCANCEL order4BUY IOC 7400 86 order5CANCEL order1CANCEL order4PRINTBUY IOC 3200 89 order6SELL IOC 0 87 order7CANCEL order2MODIFY order6 SELL 1 35PRINTBUY GFD 4700 55 order8SELL IOC 5400 67 order9BUY IOC 800 28 order10CANCEL order7CANCEL order3MODIFY order2 BUY 1 13BUY IOC 0 3 order11CANCEL order6SELL IOC 8000 88 order12CANCEL order8SELL GFD 2600 2 order13SELL GFD 6100 80 order14BUY GFD 600 22 order15PRINTBUY IOC 9200 36 order16CANCEL order6BUY IOC 4400 8 order17CANCEL order1CANCEL order10BUY IOC 5500 36 order18CANCEL order1BUY GFD 3000 29 order19MODIFY order5 BUY 1 56PRINTMODIFY order14 BUY 1 86PRINTMODIFY order3 SELL 1 99PRINTBUY IOC 7100 74 order20" };
    std::string test18 = { "BUY GFD 2600 53 order0MODIFY order0 SELL 1 80CANCEL order0SELL GFD 5100 73 order1PRINTCANCEL order0CANCEL order1SELL GFD 2600 20 order2PRINTPRINTBUY GFD 1700 81 order3CANCEL order1MODIFY order2 SELL 1 42BUY GFD 100 63 order4BUY GFD 7500 91 order5MODIFY order5 BUY 1 53CANCEL order3MODIFY order4 SELL 1 65SELL GFD 4000 84 order6MODIFY order0 SELL 1 9CANCEL order5CANCEL order3SELL IOC 900 19 order7MODIFY order7 SELL 1 10MODIFY order2 BUY 1 54MODIFY order1 SELL 1 5BUY GFD 7500 9 order8BUY IOC 7900 68 order9CANCEL order6BUY IOC 5500 92 order10MODIFY order9 SELL 1 21MODIFY order2 SELL 1 51SELL GFD 1000 97 order11MODIFY order7 SELL 1 3PRINTMODIFY order0 SELL 1 12BUY IOC 2300 71 order12PRINTSELL GFD 9300 48 order13PRINTSELL GFD 1000 60 order14MODIFY order9 BUY 1 61CANCEL order14SELL GFD 7700 73 order15PRINTSELL IOC 1000 50 order16MODIFY order9 SELL 1 54CANCEL order13SELL IOC 3300 60 order17SELL GFD 200 52 order18MODIFY order13 SELL 1 32" };
    std::stringstream ss("BUY GFD 9300 67 order0\nMODIFY order0 SELL 1 85\nPRINT\nPRINT\nSELL GFD 1500 71 order1\nCANCEL order0\nPRINT\nSELL IOC 4700 64 order2\nBUY GFD 6500 54 order3\nCANCEL order2\nMODIFY order1 SELL 1 84\nCANCEL order3\nBUY GFD 3800 84 order4\nSELL IOC 3700 87 order5\nMODIFY order0 BUY 1 61\nBUY IOC 8500 53 order6\nPRINT\nPRINT\nMODIFY order1 SELL 1 64\nBUY IOC 5200 41 order7\nBUY IOC 8300 0 order8\nBUY IOC 6300 25 order9\nMODIFY order3 BUY 1 24\nPRINT\nSELL GFD 200 56 order10\nSELL GFD 8100 7 order11\nSELL IOC 300 65 order12\nPRINT\nMODIFY order1 BUY 1 90\nBUY GFD 5700 23 order13\nMODIFY order10 BUY 1 72\nSELL GFD 4300 41 order14\nSELL GFD 7400 74 order15\nSELL IOC 900 18 order16\nSELL IOC 6400 79 order17\nCANCEL order13\nMODIFY order11 SELL 1 35\nCANCEL order10\nCANCEL order16\nCANCEL order6\nCANCEL order9\nMODIFY order3 BUY 1 0\nPRINT\nBUY IOC 9300 86 order18\nPRINT\nPRINT\nMODIFY order0 SELL 1 50\nSELL IOC 6900 84 order19\nBUY IOC 2200 56 order20\nSELL IOC 8000 48 order21\nBUY GFD 7700 30 order22");

        string getter;
        while (getline(ss, getter)) {
            switch (getter[0]) {
            case 'B':
            case 'S':
            {
                Order* ord = mf.create(getter);
                if (omv.validate(ord)) {
                    matcher.processOrd(ord);
                } else {
                    delete ord;
                }
            }
            break;
            case 'M':
            {
                Modify* mod = mmod.create(getter);
                if (omv.validate(mod)) {
                    matcher.processModify(mod);
                } else {
                    delete mod;
                }
            }
            break;
            case 'C':
            {
                Cancel* can = mcf.create(getter);
                if (can->orderid.size() > 0) {
                    matcher.processCancel(can);
                }
                else { delete can; }
            }
            break;
            case 'P':
                matcher.printCurrentState();
                break;
            }
        }
    
    return 0;

}