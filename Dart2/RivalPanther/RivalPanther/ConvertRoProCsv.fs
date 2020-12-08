namespace RivalPanther


module ConvertRoProCsv =
    open System.IO
    open FSharp.Data


    type RomMessage = CsvProvider<Schema = "command(string) ,sendtime,trader,clienttag,side,symbol,shares,price (string option),ordertype,tif,capacity,clearingid,clearingaccount,exchange,status,clordid,exchangeordid,romtag,minqty(string option),stopprice(string option),discretionary(string option),programtradingflag(string option),localaccount,firm,tradefor,querytag,dbsymbol,owner,romextag,exedest,expdate,opttype,strikeprice(string option),underlying(string option),maxfloor(string option),sectype(string),mmaccount(string option),mmhomeexchange(string option),positioneffect(string option),tradingsession,giveup (string option),orderdestination,lastshares,origshares,execprice,account,cmta,gtddatetime,cumshares,leaveshares,transactiontime,aveprice,romtime,clienttime,exchangeextag,secdef,currency,executioninst(string option),shortLender(string option),oatinstr,romnewtag (string option),orderstatus,tradeid,maturityday,rejectby,programtrading,pegpriceoff,cplxordtype,parenttag(string option),echo,effectivetime,billingcode,parentromtag,outseq,lastseq,algotype,regid,passthrough,starttime,endtime,ssecid", HasHeaders=false, AssumeMissingValues=true>
    type dataFile = {mw:MemoryStream;sw:StreamWriter; mutable recordCount:int}
    (*These are generated from the IM 2.2.1.json file*)                            
    type MECO = CsvProvider<Schema = "actionType (string),errorROEID (string option),firmROEID (string),type (string),CATReporterIMID (string option),orderKeyDate (string),orderID (string),symbol (string),parentOrderKeyDate (string),parentOrderID (string),originatingIMID (string option),eventTimestamp (string),side (string),price (string option),quantity (string),minQty (string option),orderType (string),timeInForce (string),tradingSession (string),handlingInstructions (string option),seqNum (string option),atsDisplayInd (string option),displayPrice (string option),workingPrice (string option),displayQty (string option),nbbPrice (string option),nbbQty (string option),nboPrice (string option),nboQty (string option),nbboSource (string option),nbboTimestamp (string option), ",HasHeaders=false>
    type MECOC = CsvProvider<Schema = "actionType (string),errorROEID (string option),firmROEID (string),type (string),CATReporterIMID (string option),orderKeyDate (string),orderID (string),symbol (string),originatingIMID (string option),eventTimestamp (string),side (string),cancelQty (string),leavesQty (string),reservedForFutureUse (string option),seqNum (string option), ",HasHeaders=false>
    type MECOM = CsvProvider<Schema = "actionType (string),errorROEID (string option),firmROEID (string),type (string),CATReporterIMID (string option),orderKeyDate (string),orderID (string),symbol (string),priorOrderKeyDate (string option),priorOrderID (string option),originatingIMID (string option),eventTimestamp (string),side (string),price (string option),quantity (string),minQty (string option),leavesQty (string),orderType (string),timeInForce (string),tradingSession (string),handlingInstructions (string option),seqNum (string option),atsDisplayInd (string option),displayPrice (string option),workingPrice (string option),displayQty (string option),nbbPrice (string option),nbbQty (string option),nboPrice (string option),nboQty (string option),nbboSource (string option),nbboTimestamp (string option), ",HasHeaders=false>
    type MEFA = CsvProvider<Schema = "actionType (string),errorROEID (string option),firmROEID (string),type (string),CATReporterIMID (string option),fillKeyDate (string),fulfillmentID (string),priorFillKeyDate (string option),priorFulfillmentID (string option),symbol (string),originatingIMID (string option),eventTimestamp (string),manualFlag (string),electronicTimestamp (string option),quantity (string),capacity (string),price (string),fulfillmentLinkType (string),clientDetails (string),firmDetails (string option), ",HasHeaders=false>
    type MEIR = CsvProvider<Schema = "actionType (string),errorROEID (string option),firmROEID (string),type (string),CATReporterIMID (string option),orderKeyDate (string),orderID (string),symbol (string),parentOrderKeyDate (string option),parentOrderID (string option),originatingIMID (string option),eventTimestamp (string),manualFlag (string),electronicTimestamp (string option),deptType (string),receivingDeskType (string),infoBarrierID (string option),side (string),price (string option),quantity (string),minQty (string option),orderType (string),handlingInstructions (string option),timeInForce (string),tradingSession (string), ",HasHeaders=false>
    type MENO = CsvProvider<Schema = "actionType (string),errorROEID (string option),firmROEID (string),type (string),CATReporterIMID (string option),orderKeyDate (string),orderID (string),symbol (string),eventTimestamp (string),manualFlag (string),electronicDupFlag (string),electronicTimestamp (string option),manualOrderKeyDate (string option),manualOrderID (string option),deptType (string),reservedForFutureUse (string option),reservedForFutureUse (string option),side (string),price (string option),quantity (string),minQty (string option),orderType (string),timeInForce (string),tradingSession (string),handlingInstructions (string option),custDspIntrFlag (string),firmDesignatedID (string),accountHolderType (string),affiliateFlag (string),infoBarrierID (string option),aggregatedOrders (string option),negotiatedTradeFlag (string),representativeInd (string),seqNum (string option),atsDisplayInd (string option),displayPrice (string option),workingPrice (string option),displayQty (string option),atsOrderType (string option),nbbPrice (string option),nbbQty (string option),nboPrice (string option),nboQty (string option),nbboSource (string option),nbboTimestamp (string option) ",HasHeaders=false>
    type MENOS = CsvProvider<Schema = "actionType (string),errorROEID (string option),firmROEID (string),type (string),CATReporterIMID (string option),orderKeyDate (string),orderID (string),symbol (string),originatingIMID (string option),eventTimestamp (string),aggregatedOrders (string option),firmDesignatedID (string option), ",HasHeaders=false>
    type MENQ = CsvProvider<Schema = "actionType (string),errorROEID (string option),firmROEID (string),type (string),CATReporterIMID (string option),quoteKeyDate (string),quoteID (string),symbol (string),priorQuoteKeyDate (string option),priorQuoteID (string option),eventTimestamp (string),seqNum (string option),senderIMID (string option),destination (string option),routedQuoteID (string option),onlyOneQuoteFlag (string),bidPrice (string option),bidQty (string option),askPrice (string option),askQty (string option),firmDesignatedID (string),accountHolderType (string),unsolicitedInd (string),mpStatusCode (string option),quoteRejectedFlag (string option), ",HasHeaders=false>
    type MEOA = CsvProvider<Schema = "actionType (string),errorROEID (string option),firmROEID (string),type (string),CATReporterIMID (string option),orderKeyDate (string),orderID (string),symbol (string),eventTimestamp (string),manualFlag (string),electronicDupFlag (string),electronicTimestamp (string option),receiverIMID (string),senderIMID (string),senderType (string),routedOrderID (string option),manualOrderKeyDate (string option),manualOrderID (string option),affiliateFlag (string),deptType (string),side (string),price (string option),quantity (string),minQty (string option),orderType (string),timeInForce (string),tradingSession (string),isoInd (string),handlingInstructions (string option),custDspIntrFlag (string),infoBarrierID (string option),seqNum (string option),atsDisplayInd (string option),displayPrice (string option),workingPrice (string option),displayQty (string option),atsOrderType (string option),nbbPrice (string option),nbbQty (string option),nboPrice (string option),nboQty (string option),nbboSource (string option),nbboTimestamp (string option) ",HasHeaders=false>
    type MEOC = CsvProvider<Schema = "actionType (string),errorROEID (string option),firmROEID (string),type (string),CATReporterIMID (string option),orderKeyDate (string),orderID (string),symbol (string),originatingIMID (string option),eventTimestamp (string),manualFlag (string),electronicTimestamp (string option),cancelQty (string),leavesQty (string),initiator (string),seqNum (string option) ",HasHeaders=false>
    type MEOF = CsvProvider<Schema = "actionType (string),errorROEID (string option),firmROEID (string),type (string),CATReporterIMID (string option),fillKeyDate (string),fulfillmentID (string),symbol (string),eventTimestamp (string),manualFlag (string),electronicTimestamp (string option),fulfillmentLinkType (string),cancelFlag (string),cancelTimestamp (string option),quantity (string),price (string),capacity (string),clientDetails (string),firmDetails (string option), ",HasHeaders=false>
    type MEOJ = CsvProvider<Schema = "actionType (string),errorROEID (string option),firmROEID (string),type (string),CATReporterIMID (string option),orderKeyDate (string),orderID (string),symbol (string),priorOrderKeyDate (string option),priorOrderID (string option),originatingIMID (string option),eventTimestamp (string),manualFlag (string),electronicTimestamp (string option),initiator (string),price (string option),quantity (string option),minQty (string option),leavesQty (string option),seqNum (string option),atsDisplayInd (string option),displayPrice (string option),workingPrice (string option),displayQty (string option),nbbPrice (string option),nbbQty (string option),nboPrice (string option),nboQty (string option),nbboSource (string option),nbboTimestamp (string option), ",HasHeaders=false>
    type MEOM = CsvProvider<Schema = "actionType (string),errorROEID (string option),firmROEID (string),type (string),CATReporterIMID (string option),orderKeyDate (string),orderID (string),symbol (string),priorOrderKeyDate (string option),priorOrderID (string option),originatingIMID (string option),eventTimestamp (string),manualFlag (string),manualOrderKeyDate (string option),manualOrderID (string option),electronicDupFlag (string),electronicTimestamp (string option),receiverIMID (string option),senderIMID (string option),senderType (string option),routedOrderID (string option),reservedForFutureUse (string option),reservedForFutureUse (string option),reservedForFutureUse (string option),reservedForFutureUse (string option),initiator (string),side (string),price (string option),quantity (string),minQty (string option),leavesQty (string),orderType (string),timeInForce (string),tradingSession (string),isoInd (string),handlingInstructions (string option),custDspIntrFlag (string),infoBarrierID (string option),aggregatedOrders (string option),representativeInd (string),seqNum (string option),atsDisplayInd (string option),displayPrice (string option),workingPrice (string option),displayQty (string option),atsOrderType (string option),nbbPrice (string option),nbbQty (string option),nboPrice (string option),nboQty (string option),nbboSource (string option),nbboTimestamp (string option) ",HasHeaders=false>
    type MEOMS = CsvProvider<Schema = "actionType (string),errorROEID (string option),firmROEID (string),type (string),CATReporterIMID (string option),orderKeyDate (string),orderID (string),symbol (string),originatingIMID (string option),eventTimestamp (string),aggregatedOrders (string), ",HasHeaders=false>
    type MEOR = CsvProvider<Schema = "actionType (string),errorROEID (string option),firmROEID (string),type (string),CATReporterIMID (string option),orderKeyDate (string),orderID (string),symbol (string),originatingIMID (string option),eventTimestamp (string),manualFlag (string),electronicDupFlag (string),electronicTimestamp (string option),senderIMID (string option),destination (string option),destinationType (string),routedOrderID (string option),session (string option),side (string),price (string option),quantity (string),minQty (string option),orderType (string),timeInForce (string),tradingSession (string),affiliateFlag (string),isoInd (string),handlingInstructions (string option),routeRejectedFlag (string option),dupROIDCond (string),seqNum (string option) ",HasHeaders=false>
    type MEOT = CsvProvider<Schema = "actionType (string),errorROEID (string option),firmROEID (string),type (string),CATReporterIMID (string option),tradeKeyDate (string),tradeID (string),symbol (string),eventTimestamp (string),manualFlag (string),electronicTimestamp (string option),cancelFlag (string),cancelTimestamp (string option),quoteKeyDate (string option),quoteID (string option),quantity (string),price (string),capacity (string),tapeTradeID (string option),marketCenterID (string option),sideDetailsInd (string),buyDetails (string option),sellDetails (string option),reportingExceptionCode (string option),seqNum (string option),nbbPrice (string option),nbbQty (string option),nboPrice (string option),nboQty (string option),nbboSource (string option),nbboTimestamp (string option), ",HasHeaders=false>
    type MEOTS = CsvProvider<Schema = "actionType (string),errorROEID (string option),firmROEID (string),type (string),CATReporterIMID (string option),tradeKeyDate (string),tradeID (string),symbol (string),eventTimestamp (string),buyDetails (string option),sellDetails (string option), ",HasHeaders=false>
    type MEQC = CsvProvider<Schema = "actionType (string),errorROEID (string option),firmROEID (string),type (string),CATReporterIMID (string option),quoteKeyDate (string),quoteID (string),symbol (string),originatingIMID (string option),eventTimestamp (string),seqNum (string option),reservedForFutureUse (string option),initiator (string),mpStatusCode (string option), ",HasHeaders=false>
    type MEQR = CsvProvider<Schema = "actionType (string),errorROEID (string option),firmROEID (string),type (string),CATReporterIMID (string option),quoteKeyDate (string),quoteID (string),symbol (string),receivedQuoteID (string),eventTimestamp (string),seqNum (string option),receiverIMID (string),senderIMID (string),onlyOneQuoteFlag (string),priorQuoteKeyDate (string option),priorQuoteID (string option),bidPrice (string option),bidQty (string option),askPrice (string option),askQty (string option),mpStatusCode (string option),unsolicitedInd (string),quoteWantedInd (string option), ",HasHeaders=false>
    type MOCO = CsvProvider<Schema = "actionType (string),errorROEID (string option),firmROEID (string),type (string),CATReporterIMID (string option),orderKeyDate (string),orderID (string),optionID (string),parentOrderKeyDate (string),parentOrderID (string),originatingIMID (string option),eventTimestamp (string),side (string),price (string option),quantity (string),minQty (string option),orderType (string),timeInForce (string),tradingSession (string),handlingInstructions (string option),openCloseIndicator (string),priorUnlinked (string option),nextUnlinked (string option), ",HasHeaders=false>
    type MOCOC = CsvProvider<Schema = "actionType (string),errorROEID (string option),firmROEID (string),type (string),CATReporterIMID (string option),orderKeyDate (string),orderID (string),optionID (string),originatingIMID (string option),eventTimestamp (string),side (string),cancelQty (string),leavesQty (string),reservedForFutureUse (string option),priorUnlinked (string option), ",HasHeaders=false>
    type MOCOM = CsvProvider<Schema = "actionType (string),errorROEID (string option),firmROEID (string),type (string),CATReporterIMID (string option),orderKeyDate (string),orderID (string),optionID (string),priorOrderKeyDate (string option),priorOrderID (string option),originatingIMID (string option),eventTimestamp (string),side (string),price (string option),quantity (string),minQty (string option),leavesQty (string),orderType (string),timeInForce (string),tradingSession (string),handlingInstructions (string option),openCloseIndicator (string option),priorUnlinked (string option),nextUnlinked (string option), ",HasHeaders=false>
    type MOFA = CsvProvider<Schema = "actionType (string),errorROEID (string option),firmROEID (string),type (string),CATReporterIMID (string option),fillKeyDate (string),fulfillmentID (string),optionID (string),priorFillKeyDate (string option),priorFulfillmentID (string option),originatingIMID (string option),eventTimestamp (string),manualFlag (string),electronicTimestamp (string option),fulfillmentLinkType (string),quantity (string),price (string),clientDetails (string),firmDetails (string option),priorUnlinked (string option), ",HasHeaders=false>
    type MOIR = CsvProvider<Schema = "actionType (string),errorROEID (string option),firmROEID (string),type (string),CATReporterIMID (string option),orderKeyDate (string),orderID (string),optionID (string),parentOrderKeyDate (string option),parentOrderID (string option),originatingIMID (string option),eventTimestamp (string),manualFlag (string),electronicTimestamp (string option),deptType (string),receivingDeskType (string),side (string),price (string option),quantity (string),minQty (string option),orderType (string),handlingInstructions (string option),openCloseIndicator (string option),priorUnlinked (string option),nextUnlinked (string option),timeInForce (string),tradingSession (string), ",HasHeaders=false>
    type MONO = CsvProvider<Schema = "actionType (string),errorROEID (string option),firmROEID (string),type (string),CATReporterIMID (string option),orderKeyDate (string),orderID (string),optionID (string),eventTimestamp (string),manualFlag (string),manualOrderKeyDate (string option),manualOrderID (string option),electronicDupFlag (string),electronicTimestamp (string option),deptType (string),side (string),price (string option),quantity (string),minQty (string option),orderType (string),timeInForce (string),tradingSession (string),handlingInstructions (string option),firmDesignatedID (string),accountHolderType (string),affiliateFlag (string),aggregatedOrders (string option),reservedForFutureUse (string option),openCloseIndicator (string option),representativeInd (string),nextUnlinked (string option), ",HasHeaders=false>
    type MONOS = CsvProvider<Schema = "actionType (string),errorROEID (string option),firmROEID (string),type (string),CATReporterIMID (string option),orderKeyDate (string),orderID (string),optionID (string),originatingIMID (string option),eventTimestamp (string),aggregatedOrders (string option),priorUnlinked (string option),nextUnlinked (string option),firmDesignatedID (string option), ",HasHeaders=false>
    type MOOA = CsvProvider<Schema = "actionType (string),errorROEID (string option),firmROEID (string),type (string),CATReporterIMID (string option),orderKeyDate (string),orderID (string),optionID (string),eventTimestamp (string),manualOrderKeyDate (string option),manualOrderID (string option),manualFlag (string),electronicDupFlag (string),electronicTimestamp (string option),receiverIMID (string),senderIMID (string),senderType (string),routedOrderID (string option),deptType (string),side (string),price (string option),quantity (string),minQty (string option),orderType (string),timeInForce (string),tradingSession (string),handlingInstructions (string option),affiliateFlag (string),reservedForFutureUse (string option),reservedForFutureUse (string option),openCloseIndicator (string option),priorUnlinked (string option),nextUnlinked (string option), ",HasHeaders=false>
    type MOOC = CsvProvider<Schema = "actionType (string),errorROEID (string option),firmROEID (string),type (string),CATReporterIMID (string option),orderKeyDate (string),orderID (string),optionID (string),originatingIMID (string option),eventTimestamp (string),manualFlag (string),electronicTimestamp (string option),cancelQty (string),leavesQty (string),initiator (string),priorUnlinked (string option), ",HasHeaders=false>
    type MOOF = CsvProvider<Schema = "actionType (string),errorROEID (string option),firmROEID (string),type (string),CATReporterIMID (string option),fillKeyDate (string),fulfillmentID (string),optionID (string),eventTimestamp (string),manualFlag (string),electronicTimestamp (string option),quantity (string),price (string),fulfillmentLinkType (string),clientDetails (string),firmDetails (string option),priorUnlinked (string option),cancelFlag (string),cancelTimestamp (string option), ",HasHeaders=false>
    type MOOJ = CsvProvider<Schema = "actionType (string),errorROEID (string option),firmROEID (string),type (string),CATReporterIMID (string option),orderKeyDate (string),orderID (string),optionID (string),priorOrderKeyDate (string option),priorOrderID (string option),originatingIMID (string option),eventTimestamp (string),manualFlag (string),electronicTimestamp (string option),initiator (string),price (string option),quantity (string option),minQty (string option),leavesQty (string option),priorUnlinked (string option),nextUnlinked (string option), ",HasHeaders=false>
    type MOOM = CsvProvider<Schema = "actionType (string),errorROEID (string option),firmROEID (string),type (string),CATReporterIMID (string option),orderKeyDate (string),orderID (string),optionID (string),priorOrderKeyDate (string option),priorOrderID (string option),originatingIMID (string option),eventTimestamp (string),manualOrderKeyDate (string option),manualOrderID (string option),manualFlag (string),electronicDupFlag (string),electronicTimestamp (string option),receiverIMID (string option),senderIMID (string option),senderType (string option),routedOrderID (string option),initiator (string),side (string),price (string option),quantity (string),minQty (string option),leavesQty (string),orderType (string),timeInForce (string),tradingSession (string),handlingInstructions (string option),openCloseIndicator (string option),reservedForFutureUse (string option),reservedForFutureUse (string option),aggregatedOrders (string option),representativeInd (string),priorUnlinked (string option),nextUnlinked (string option), ",HasHeaders=false>
    type MOOR = CsvProvider<Schema = "actionType (string),errorROEID (string option),firmROEID (string),type (string),CATReporterIMID (string option),orderKeyDate (string),orderID (string),optionID (string),originatingIMID (string option),eventTimestamp (string),manualFlag (string),electronicDupFlag (string),electronicTimestamp (string option),senderIMID (string option),destination (string option),destinationType (string),routedOrderID (string option),session (string option),side (string),price (string option),quantity (string),minQty (string option),orderType (string),timeInForce (string),tradingSession (string),handlingInstructions (string option),routeRejectedFlag (string option),exchOriginCode (string option),affiliateFlag (string),reservedForFutureUse (string option),openCloseIndicator (string option),priorUnlinked (string option),nextUnlinked (string option), ",HasHeaders=false>


    exception BadRomMessage of string
    exception UnknownExchange of string
    exception UnknownAccount of string

    let roeID = 
        let roe = ref 0
        fun () ->
               roe := !roe + 1
               !roe
    (*jdt:OutCat.StringOrArray,*)                               

    let createRoeID(destid:string, romtime:string) = 
        let num = roeID()
        romtime.[0..7] + "_" + (num.ToString()) + destid + System.DateTime.Now.Ticks.ToString()

    let getSession tif =
        match tif with
        | "0" -> "REG"
        | "3" -> "REG"
        | "2" -> "REG"
        | _ -> "ALL"   

    let checkStop romStop =
        match romStop with
        | None -> None
        | Some x -> Some ("|STOP=" + x)

    let checkRomAuction romAuction =
        match romAuction with
        | None -> None
        | Some x -> Some ("|AucResp=" + x)

    let checkRomDispQty maxfloor =
        match maxfloor with
        | None -> None
        | Some x -> Some ("|DISQ=" + x)

    let checkOpenCloseAuction ordType tif =
        match tif with
        | "2" when ordType.Equals("2") -> Some "|LOO"
        | "2" when ordType.Equals("1") -> Some "|MOO"
        | _ -> match ordType with 
               | "11" -> Some "|LOC"
               | "12" -> Some "|MOO"
               | "5" -> Some "|MOC"
               | _ -> None

    let checkPeg exInst ordType =
        match ordType with
        | "90" -> Some "|P"
        | "91" -> Some "|R"
        | "92" -> Some "|M"
        | _ -> match exInst with 
               | Some "M" -> Some "|M"
               | Some "P" -> Some "|P"
               | Some "R" -> Some "|R"
               | _ -> None

    let checkAddLiq inst exeInst =
        match inst with 
        | "A" -> Some "|ALO"
        | _ -> match exeInst with
               | Some "A" -> Some "|ALO"
               |_ -> None

    let checkImbalance exeInst instr = 
        match exeInst with
        | Some "I" -> Some "|IO"
        | _ -> match instr with
                | "I" -> Some "|IO"
                | _ -> None

    let checkNYSEFloorBroker instr =
        match instr with
        | "DQ" -> Some "|FBA"
        | _ -> None

    let checkSelfTrade romDesc destid =
        match romDesc with
        | Some _ -> Some "|STP"
        | None -> match destid with
                    | "241"
                    | "124"
                    | "285"
                    | "287"
                    | "506"
                    | "507"
                    | "508"
                    | "509"
                    | "510"
                    | "511"
                    | "512"
                    | "513"
                    | "514"
                    | "515"
                    | "516"
                    | "517"
                    | "518"
                    | "519"
                    | "522"
                    | "525" -> Some "|STP"
                    | _ -> None
        
    let buildHandlingInstructions(csv:RomMessage.Row) = 
        [Some "DIR"] 
            |> (fun elem -> List.append elem [checkStop csv.Stopprice]) 
            |> (fun elem -> List.append elem [checkRomAuction csv.Parenttag])
            |> (fun elem -> List.append elem [checkAddLiq csv.Clordid csv.Executioninst])
            |> (fun elem -> List.append elem [checkRomDispQty csv.Maxfloor])
            |> (fun elem -> List.append elem [checkOpenCloseAuction csv.Ordertype csv.Tif])
            |> (fun elem -> List.append elem [checkPeg csv.Executioninst csv.Ordertype])
            |> (fun elem -> List.append elem [checkImbalance csv.Executioninst csv.Clordid])
            |> (fun elem -> List.append elem [checkSelfTrade csv.Giveup csv.Exchange])
            |> (fun elem -> List.append elem [checkNYSEFloorBroker csv.Clordid])
            |> List.fold (fun acc elem -> match elem with 
                                          | None -> acc
                                          | Some x -> acc + x) ""
        
        
    let getExchange (key:string, destMap:Map<string,ConfigBuilder.CatConfig.DestImidVal>) =
        let cimid = Map.find key destMap
        cimid.ExchangeId

    let getExType (key:string, destMap:Map<string, ConfigBuilder.CatConfig.DestImidVal>) =
        let cimid = Map.find key destMap
        cimid.DestType

    let utcToEastern (time:string) = 
        match time.Length with 
        | x when x >= 21 -> 
                let utcTime = System.DateTime.ParseExact(time.Substring(0,21), "yyyyMMdd-HH:mm:ss.fff", System.Globalization.CultureInfo.InvariantCulture)
                let estZone = TimeZoneConverter.TZConvert.GetTimeZoneInfo("Eastern Standard Time")///System.TimeZoneInfo.FindSystemTimeZoneById("Eastern Standard Time")
                let estTime = System.TimeZoneInfo.ConvertTimeFromUtc(utcTime, estZone)
                sprintf "%04d%02d%02d %02d%02d%02d.%03d" estTime.Year estTime.Month estTime.Day estTime.Hour estTime.Minute estTime.Second estTime.Millisecond
        | _ -> printfn "Bad time: %s" time;""

    let convertTime(csv:RomMessage.Row) =
        match csv.Clienttime.Length with 
        | 0 -> utcToEastern csv.Romtime
        | _ -> utcToEastern csv.Clienttime

    let convertTif( tif:string, time:string) =
        match tif with
        | "1" ->"GTC"
        | "3" -> "IOC"
        | "6" -> "GTD"
        | _ -> "DAY=" + time.[0..7]


    let convertOrderType ordType =
        match ordType with
        | "12"
        | "5"
        | "3"
        | "90"
        | "91"
        | "92"
        | "1" -> "MKT"
        |_ -> "LMT"

    let translateSide side =
        match side with
        | "1" -> "B"
        | "2" -> "SL"
        | _ -> "SS"
    let getDisplay(maxfl:(string option)) =
        match maxfl with
        | None -> "false"
        | Some _ -> "true"

    let createNewOrderEvent(imi:ConfigBuilder.IMConfig,
                            csv:RomMessage.Row) =
        match imi.custMap.TryFind csv.Clearingaccount with
        | None -> None
        | Some custAccount ->  
                                let firmid = (match custAccount.FirmId.String with
                                             | None -> match custAccount.FirmId.Number with
                                                        | None -> raise(ConfigBuilder.BadCatConfig("No Firm ID for account in create New Ord"))
                                                        | Some x -> x.ToString()
                                             | Some x -> x)
                                let holder = custAccount.HolderType
                                Some ( MENO.Row("NEW", None, createRoeID(csv.Exchange, csv.Romtime),
                                               "MENO",Some imi.imid, convertTime csv, csv.Clienttag,
                                               csv.Symbol, utcToEastern csv.Romtime, "false", "false", None, None,
                                               None,custAccount.DeptType, None, None,
                                               translateSide csv.Side, csv.Price, csv.Shares,
                                               csv.Minqty, convertOrderType csv.Ordertype,
                                               convertTif( csv.Tif, utcToEastern(csv.Romtime)), getSession csv.Tif,
                                               Some (buildHandlingInstructions csv), (getDisplay(csv.Maxfloor)),
                                               firmid, holder, custAccount.Affiliate.ToString(),
                                               None, None, "false", "N", None, None,None, None, None,
                                               None, None, None, None, None,None,None))

    let createOrderRouteRpt(imi:ConfigBuilder.IMConfig,
                            csv:RomMessage.Row,
                            romTime:string) =
        let affiliateStr = match imi.custMap.TryFind csv.Clearingaccount with
                           | None -> match imi.bdAccounts.TryFind csv.Clearingaccount with
                                        | None -> raise(UnknownAccount(sprintf "%s%s" csv.Clearingaccount csv.Romtag))
                                        | Some x -> x.Affiliate.ToString()
                           | Some x -> x.Affiliate.ToString()
        let dest = match imi.destinations.TryFind csv.Exchange with
                    |None -> raise(UnknownExchange(sprintf "%s%s"csv.Exchange csv.Romtag))
                    |Some x -> x
        let orderId =  match dest.ExchangeId with 
                        | "DART" -> csv.Clienttag
                        | _ -> csv.Romtag
        let routeID = match dest.ExchangeId with
                        | "DART" -> csv.Clienttag
                        |_ -> csv.Clordid
        let session = match dest.DestType with
                        | "E" -> Some (getSession csv.Tif)
                        | _ -> None
        MEOR.Row("NEW", None, createRoeID(csv.Exchange, csv.Romtime), "MEOR", Some imi.imid, utcToEastern romTime,
                  orderId, csv.Symbol, None, utcToEastern romTime, "false", "false",None, Some dest.Imid, 
                  Some dest.ExchangeId, dest.DestType, Some routeID, session, translateSide csv.Side, csv.Price,
                  csv.Shares, csv.Minqty, convertOrderType csv.Ordertype,
                  convertTif( csv.Tif, romTime), getSession csv.Tif,affiliateStr,
                  "NA", Some (buildHandlingInstructions csv), None, "false", None)

    let createOrderAccepted(imi:ConfigBuilder.IMConfig,
                            csv:RomMessage.Row) =
        match imi.bdAccounts.TryFind csv.Clearingaccount with
        | None -> None
        | Some custAccount ->                                                 
                            Some (MEOA.Row("NEW", None, createRoeID(csv.Exchange, csv.Romtime), "MEOA", Some imi.imid,
                                            utcToEastern csv.Romtime,
                                            csv.Romtag, csv.Symbol, utcToEastern csv.Romtime, "false", "false",
                                            None, imi.imid,
                                            custAccount.Imid, "F", Some csv.Clienttag, None, None,
                                            custAccount.Affiliate.ToString(), custAccount.DeptType,
                                            translateSide csv.Side, csv.Price, csv.Shares,
                                            csv.Minqty, convertOrderType csv.Ordertype,
                                            convertTif( csv.Tif, utcToEastern(csv.Romtime)), getSession csv.Tif,
                                            "NA", Some (buildHandlingInstructions csv),
                                            getDisplay csv.Maxfloor,None, None, None, None,
                                            None, None, None, None, None, None, None,
                                            None, None))

    let createOrderCancelledEvent(imi:ConfigBuilder.IMConfig,
                                   csv:RomMessage.Row) =
        let dest = match imi.destinations.TryFind csv.Exchange with
                            |None -> raise(UnknownExchange(sprintf "%s%s"csv.Exchange csv.Romtag))
                            |Some x -> x
        let time =  match dest.ExchangeId with 
                    | "DART" -> match csv.Clienttime.Length with
                                    | x when x > 0 -> csv.Clienttime 
                                    | _ -> csv.Romtime
                    | _ -> csv.Romtime
        let orderId =  match dest.ExchangeId with 
                        | "DART" -> csv.Clienttag
                        | _ -> csv.Romtag
        MEOC.Row("NEW", None, createRoeID(csv.Exchange, csv.Romtime), "MEOC",Some imi.imid, utcToEastern time, orderId,
                    csv.Symbol, None,
                    utcToEastern time, "false", None, csv.Leaveshares, csv.Leaveshares, "C", None)

    let createOrderModifiedEvent(imi:ConfigBuilder.IMConfig,
                                  csv:RomMessage.Row,
                                  orgTimes:Map<string,string>) =
        let dest = match imi.destinations.TryFind csv.Exchange with 
                    | None -> raise(UnknownExchange(sprintf "%s%s" csv.Exchange csv.Romtag))
                    | Some x -> x
        let routeID = match dest.ExchangeId with
                            | "DART" -> csv.Clienttag
                            |_ -> csv.Clordid
        let senderIMID = match imi.bdAccounts.TryFind csv.Clearingaccount with
                            | None -> None
                            | Some x -> Some x.Imid
        let receiverIMID = match imi.bdAccounts.TryFind csv.Clearingaccount with
                                 | None -> None
                                 | Some x -> Some imi.imid
        let orderKeyDate = match orgTimes.TryFind csv.Romtag with
                            | None -> csv.Romtime
                            | Some x -> x
        MEOM.Row("NEW", None, createRoeID(csv.Exchange, csv.Romtime), "MEOM",Some imi.imid,
                 utcToEastern orderKeyDate, csv.Romtag,
                 csv.Symbol, None, None, None, utcToEastern csv.Romtime, "false", None, None, "false",
                 None, receiverIMID, senderIMID, Some "F", Some routeID,None, None,
                 None, None,"C",translateSide csv.Side,
                 csv.Price, csv.Shares, csv.Minqty, csv.Leaveshares, convertOrderType csv.Ordertype, 
                 convertTif(csv.Tif, utcToEastern(csv.Romtime)), getSession csv.Tif,
                 "NA", Some (buildHandlingInstructions csv),
                 getDisplay csv.Maxfloor, None,None, "N", None, None, None, None, None, None, None , None,
                 None , None, None, None)
    

    let buildClientNewAndRouteCsv (rom:seq<RomMessage.Row>,
                                    imi:ConfigBuilder.IMConfig) = 
        rom
        |> Seq.filter(fun elem -> elem.Command.Equals("E"))
        |> Seq.filter(fun elem -> match imi.custMap.TryFind elem.Clearingaccount with 
                                    | None -> false
                                    | Some _ -> true)
        |> Seq.map(fun elem -> (createNewOrderEvent(imi, elem)))
    
    let buildClientRoute(rom:seq<RomMessage.Row>,
                           imi:ConfigBuilder.IMConfig,
                            origTimes:Map<string,string>) =
        rom
        |> Seq.filter(fun elem -> elem.Command.Equals("13"))
        |> Seq.filter(fun elem -> match imi.custMap.TryFind elem.Clearingaccount with 
                                    | None -> false
                                    | Some _ -> true)
        |> Seq.map(fun elem -> createOrderRouteRpt(imi, elem, match origTimes.TryFind(elem.Romtag) with
                                                                | None -> utcToEastern(elem.Romtime)
                                                                | Some x -> x))
    
    let buildAccept(rom:seq<RomMessage.Row>,
                    imi:ConfigBuilder.IMConfig) =
        rom
        |> Seq.filter(fun elem -> elem.Command.Equals("E"))
        |> Seq.map(fun elem -> createOrderAccepted(imi, elem))

    let buildRouteForBD(rom:seq<RomMessage.Row>,
                        imi:ConfigBuilder.IMConfig,
                        origTimes:Map<string, string>) =
        rom
        |> Seq.filter(fun elem -> elem.Status.Equals("13")) 
        |> Seq.filter(fun elem -> match imi.bdAccounts.TryFind elem.Clearingaccount with 
                                           | None -> false
                                           | Some _ -> true)
        |> Seq.map(fun elem -> createOrderRouteRpt(imi, elem, match origTimes.TryFind(elem.Romtag) with
                                                                | None -> utcToEastern(elem.Romtime)
                                                                | Some x -> x))

    let buildClientReplaceNRoute (rom:seq<RomMessage.Row>, 
                                   imi:ConfigBuilder.IMConfig,
                                   orgTimes:Map<string,string>) =
        rom
        |> Seq.filter(fun elem -> elem.Status.Equals("27")) 
        |> Seq.filter(fun elem -> match imi.bdAccounts.TryFind elem.Clearingaccount with 
                                    | None -> match imi.custMap.TryFind elem.Clearingaccount with
                                                | None -> false
                                                | Some _-> true
                                    | Some _ -> true)
        |> Seq.map(fun elem -> (createOrderModifiedEvent(imi, elem, orgTimes),
                                createOrderRouteRpt(imi, elem, elem.Romtime)))
       
    let buildClientCancel(rom:seq<RomMessage.Row>,
                           imi:ConfigBuilder.IMConfig) = 
        rom
        |> Seq.filter(fun elem -> elem.Status.Equals("26"))
        |> Seq.filter(fun elem -> match imi.bdAccounts.TryFind elem.Clearingaccount with 
                                    | None -> match imi.custMap.TryFind elem.Clearingaccount with
                                                | None -> false
                                                | Some _-> true
                                    | Some _ -> true)
        |> Seq.map(fun elem -> (createOrderCancelledEvent(imi, elem)))

    let getOrigTimes(rom:seq<RomMessage.Row>) =
        let orig = Map.empty
        rom
        |> Seq.filter(fun elem -> elem.Symbol <> "ZVZZT" && elem.Symbol <> "NTEST")
        |> Seq.filter(fun elem -> elem.Command.Equals("E"))
        |> Seq.fold (fun  (acc:Map<string,string>) elem -> acc.Add(elem.Romtag, elem.Romtime)) orig 

    let buildEquityFile(rows:seq<RomMessage.Row>, imi:ConfigBuilder.IMConfig) =
        let origTimes = getOrigTimes(rows)
        let memStr = new System.IO.MemoryStream()
        let ofile = {mw = memStr; sw = new System.IO.StreamWriter(memStr); recordCount = 0}
        let noTest = rows |> Seq.filter(fun elem -> elem.Symbol <> "ZVZZT" && elem.Symbol <> "NTEST")
                          |> Seq.filter(fun elem -> match imi.destinations.TryFind(elem.Exchange) with
                                                        | None -> false
                                                        | Some _ -> true)
        let custNewOrds = buildClientNewAndRouteCsv(noTest, imi)
        let custRoutes = buildClientRoute(noTest, imi, origTimes)
        let bdAccepts = buildAccept(noTest, imi)
        let bdRoutes = buildRouteForBD(noTest, imi, origTimes)
        let replaces = buildClientReplaceNRoute(noTest, imi, origTimes)
        let cancels = buildClientCancel(noTest, imi)
        custNewOrds |> Seq.iter(fun elem -> match elem with
                                            | Some x -> let f = new MENO([x]);
                                                        ofile.sw.Write(f.SaveToString());
                                                        ofile.sw.Flush();
                                                        ofile.recordCount <- ofile.recordCount + 1
                                            | None -> ())
        custRoutes |> Seq.iter(fun elem -> let f = new MEOR([elem]);
                                           ofile.sw.Write(f.SaveToString());
                                           ofile.sw.Flush();
                                           ofile.recordCount <- ofile.recordCount + 1)
                                      
        bdAccepts |> Seq.iter(fun elem -> match elem with 
                                            | Some x -> let f = new MEOA([x]);
                                                        ofile.sw.Write(f.SaveToString())
                                                        ofile.sw.Flush();
                                                        ofile.recordCount <- ofile.recordCount + 1
                                            | None -> ())
        bdRoutes |> Seq.iter(fun elem -> let f = new MEOR([elem]);
                                         ofile.sw.Write(f.SaveToString());
                                         ofile.sw.Flush();
                                         ofile.recordCount <- ofile.recordCount + 1)
        replaces |> Seq.iter(fun (e1,e2) -> let f = new MEOM([e1]); 
                                            let f1 = new MEOR([e2])
                                            ofile.sw.Write(f.SaveToString());
                                            ofile.sw.Write(f1.SaveToString());
                                            ofile.sw.Flush();
                                            ofile.recordCount <- ofile.recordCount + 2)
        cancels |> Seq.iter(fun elem -> let f = new MEOC([elem]);
                                        ofile.sw.Write(f.SaveToString());
                                        ofile.sw.Flush();
                                        ofile.recordCount <- ofile.recordCount + 1)
        (memStr.GetBuffer(), memStr.Length,ofile.recordCount)
    
                                            