#include "pch.h"
#include "../QuantLabPro/DataHandler.h"
#include "../QuantLabPro/CsvHandler.h"
#include "../QuantLabPro/TradeEvent.h"
#include "../QuantLabPro/PerSymbolDailyTrades.h"
#include "../QuantLabPro/TradeCollator.h"
#include "../QuantLabPro/Order.h"
#include "../QuantLabPro/MatchingEngine.h"
#include "../QuantLabPro/StdOutPrinter.h"
#include "../QuantLabPro/Validator.h"
#include "../QuantLabPro/MessageFactory.h"
#include "../QuantLabPro/MessageReaper.h"


TEST(MatchingEngineTests, badMessage)
{
    MessageFactory<Order> mf;
    MessageFactory<Modify> mmf;
    OrdMessageValidator omv;
    std::string str1 = { "BUY GFD 200 10" };
    Order* ord1 = mf.create(str1);
    std::string badMod = { "MODIFY order3 " };
    std::string bmod2 = { "MODIFY order3 BUY 1 0" };
    Modify* mod = mmf.create(badMod);
    Modify* m2 = mmf.create(bmod2);
    EXPECT_FALSE(omv.validate(mod));
    EXPECT_FALSE(omv.validate(m2));
    EXPECT_FALSE(omv.validate(ord1));
    delete ord1;
}


TEST(MatchingEngineTests, placeInMap1)
{
    MessageFactory<Order> mf;
    OrdMessageValidator omv;
    StdOutPrinter<Order> sop;
    MessageReaper rep;
    MatchingEngine<StdOutPrinter<Order>, MessageReaper> matcher(sop, rep);
    std::string str1 = { "BUY GFD 200 10 order1" };
    std::string str2 = { "SELL GFD 220 20 order2" };
    std::string str3 = { "BUY GFD 210 30 order3" };
    std::string str4 = { "SELL GFD 240 50 order4" };
    Order* ord1 = mf.create(str1);
    EXPECT_TRUE(omv.validate(ord1));
    EXPECT_EQ(ord1->orderid, "order1");
    matcher.processOrd(ord1);
 
    Order* ord2 = mf.create(str2);
    EXPECT_EQ(ord2->price, 220);
    EXPECT_TRUE(omv.validate(ord2));
    matcher.processOrd(ord2);
    Order* ord3 = mf.create(str3);
    EXPECT_EQ(ord3->size, 30);
    EXPECT_EQ(ord3->leaves, 30);
    EXPECT_TRUE(omv.validate(ord3));
    matcher.processOrd(ord3);
    Order* ord4 = mf.create(str4);
    EXPECT_TRUE(omv.validate(ord4));
    EXPECT_EQ(ord4->orderid, "order4");
    matcher.processOrd(ord4);
    matcher.printCurrentState();
}

TEST(MatchingEngineTests, createMessages)
{
    MessageFactory<Order> mf;
    OrdMessageValidator omv;
    std::string str1 = { "BUY GFD 200 10 order1" };
    std::string str2 = { "SELL GFD 220 20 order2" };
    std::string str3 = { "BUY GFD 210 30 order3" };
    std::string str4 = { "SELL GFD 240 50 order4" };
    Order* ord1 = mf.create(str1);
    EXPECT_TRUE(omv.validate(ord1));
    EXPECT_EQ(ord1->orderid, "order1");
    Order* ord2 = mf.create(str2);
    EXPECT_EQ(ord2->price, 220);
    EXPECT_TRUE(omv.validate(ord2));
    Order* ord3 = mf.create(str3);
    EXPECT_EQ(ord3->size, 30);
    EXPECT_EQ(ord3->leaves, 30);
    EXPECT_TRUE(omv.validate(ord3));
    Order* ord4 = mf.create(str4);
    EXPECT_TRUE(omv.validate(ord4));
    EXPECT_EQ(ord4->orderid, "order4");
    delete ord1;
    delete ord2;
    delete ord3;
    delete ord4;
}

TEST(CollatorFunctionalTest, endToEnd) {
    TradeCollator<CsvHandler<TradeEvent>, TradeEvent, PerSymbolDailyTrades> 
        collator("input.csv", "output.csv", UINT32_MAX);
    collator.collateTrades();
    EXPECT_EQ(1, 1);
}

TEST(CsvHandlerTests, createNGet) {
    CsvHandler<string> testHander("input.csv", "output.csv");
    std::vector<unique_ptr<string>> testV;
    testHander.getData(testV, 20);
    EXPECT_GT(testV.size(), 0UL);
    EXPECT_LE(testV.size(), 20UL);
}

TEST(CsvHandlerTests, getLess) {
    CsvHandler<string> tHandler("input.csv", "output.csv");
    std::vector<unique_ptr<string>> tVec;
    tHandler.getData(tVec, UINT32_MAX);
    EXPECT_LT(tVec.size(), UINT32_MAX);
    EXPECT_EQ(tVec.size(), 20346);
}
TEST(CsvHandlerTests, withTradeEvent) {
    CsvHandler<TradeEvent> tHandler("input.csv", "output.csv");
    std::vector<unique_ptr<TradeEvent>> tVec;
    tHandler.getData(tVec, 20);
    EXPECT_EQ(36, tVec[0]->getPrice());
    EXPECT_EQ("ebe", tVec[5]->getSymbol());
    EXPECT_EQ(188, tVec[10]->getSize());
    EXPECT_EQ(51300223150, tVec[8]->getTimeStamp());
}

TEST(PerSymbolDailyTradesTests, create) {
    PerSymbolDailyTrades psdt("aaa");
    EXPECT_EQ("aaa", psdt.getSymbol());
}

TEST(PerSymbolDailyTradesTests, addEvent) {
    PerSymbolDailyTrades psdt("fcf");
    EXPECT_EQ("fcf", psdt.getSymbol());
    auto te = make_unique<TradeEvent>("51300223150,fcf,202,42");
    int worked = psdt.addTrade(te);
    EXPECT_EQ(1, worked);
    auto t2 = make_unique<TradeEvent>("23455678,gad,342,232");
    worked = psdt.addTrade(t2);
    EXPECT_EQ(0, worked);
}

TEST(PerSymbolDailyTradesTests, outputTest) {
    PerSymbolDailyTrades psdt("fcf");
    EXPECT_EQ("fcf", psdt.getSymbol());
    auto te = make_unique<TradeEvent>("51300223150,fcf,202,42");
    int worked = psdt.addTrade(te);
    auto t2 = make_unique<TradeEvent>("51300223380,fcf,38,22");
    worked = psdt.addTrade(t2);
    psdt.calcAvgPrice();
    std::stringstream ss;
    ss << psdt;
    EXPECT_EQ("fcf,230,240,38,42\n", ss.str());
}

/*20 shares of aaa @ 18
	5 shares of aaa @ 7
	Weighted Average Price = ((20 * 18) + (5 * 7)) / (20 + 5) = 15*/
TEST(PerSymbolDailyTradesTests, calcDataTest) {
    PerSymbolDailyTrades psdt("aaa");
    auto t1 = make_unique<TradeEvent>("51300223150,aaa,20,18");
    auto t2 = make_unique<TradeEvent>("51300224345,aaa,5,7");
    int worked = psdt.addTrade(t1);
    EXPECT_EQ(worked, 1);
    worked = psdt.addTrade(t2);
    EXPECT_EQ(worked, 1);
    psdt.calcAvgPrice();
    std::stringstream ss;
    ss << psdt;
    EXPECT_EQ("aaa,1195,25,15,18\n", ss.str());
}
TEST(PerSymbolDailyTradesTests, failToAdd) {
    PerSymbolDailyTrades psdt("aaa");
    auto t = make_unique<TradeEvent>("51300316017,dda,147,169");
    int worked = psdt.addTrade(t);
    EXPECT_EQ(0, worked);
}
/*
YEs I copied this from your instructions.
52924702,aaa,13,1136
52924702,aac,20,477
52925641,aab,31,907
52927350,aab,29,724
52927783,aac,21,638
52930489,aaa,18,1222
52931654,aaa,9,1077
52933453,aab,9,756
*/
TEST(PerSymbolDailyTradesTests, smallSampleTest) {
    PerSymbolDailyTrades psdt("aaa");
    auto t1 = make_unique<TradeEvent>("52924702,aaa,13,1136");
    auto t2 = make_unique<TradeEvent>("52930489,aaa,18,1222");
    auto t3 = make_unique<TradeEvent>("52931654,aaa,9,1077");
    int worked = psdt.addTrade(t1);
    EXPECT_EQ(1, worked);
    worked = psdt.addTrade(t2);
    EXPECT_EQ(1, worked);
    worked = psdt.addTrade(t3);
    EXPECT_EQ(1, worked);
    psdt.calcAvgPrice();
    std::stringstream ss;
    ss << psdt;
    EXPECT_EQ("aaa,5787,40,1161,1222\n", ss.str());
}
TEST(PerSymbolDailyTradesTests, oneDataPointTest) {
    PerSymbolDailyTrades psdt("dda");
    auto t = make_unique<TradeEvent>("51300316017,dda,147,169");
    int worked = psdt.addTrade(t);
    EXPECT_EQ(1, worked);
    psdt.calcAvgPrice();
    std::stringstream ss;
    ss << psdt;
    EXPECT_EQ("dda,0,147,169,169\n", ss.str());
}

TEST(PerSymbolDailyTradesTests, lessThanTest) {
    PerSymbolDailyTrades p1("fcf");
    PerSymbolDailyTrades p2("gad");
    PerSymbolDailyTrades p3("gcc");
    PerSymbolDailyTrades p4("xrt");
    std::map<string, PerSymbolDailyTrades>setOne;
    setOne.insert(std::pair<string, PerSymbolDailyTrades>(p4.getSymbol(), p4));
    setOne.insert(std::pair<string, PerSymbolDailyTrades>(p2.getSymbol(),p2));
    setOne.insert(std::pair<string, PerSymbolDailyTrades>(p1.getSymbol(), p1));
    setOne.insert(std::pair<string, PerSymbolDailyTrades>(p3.getSymbol(), p3));
    auto j = setOne.find("gcc");
    EXPECT_NE(j, setOne.end());
    int i = 0;
    for (auto psdt : setOne) {
        switch (i)
        {
        case 0:
            EXPECT_EQ("fcf", psdt.second.getSymbol());
            break;
        case 1:
            EXPECT_EQ("gad", psdt.second.getSymbol());
            break;
        case 2:
            EXPECT_EQ("gcc", psdt.second.getSymbol());
            break;
        case 3:
            EXPECT_EQ("xrt", psdt.second.getSymbol());
            break;
        default:
            break;
        }
        ++i;
    }
}

TEST(TradeEventTests, createTradeEvent) {
    TradeEvent te("234567,aaa,45,12");
    EXPECT_EQ("aaa", te.getSymbol());
    EXPECT_EQ(234567, te.getTimeStamp());
    EXPECT_EQ(45, te.getSize());
    EXPECT_EQ(12, te.getPrice());
}