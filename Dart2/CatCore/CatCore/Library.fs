namespace CatCore

module PantherCore =
    open FSharp.Data
    open System.IO
    open System.Security.Cryptography
          (*
            Access: AKIAUE5JMX624CYD3IJM
            Secret: 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
           *)

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

       type MetaDataC = CsvProvider< "../../CatCore/CatCore/metadata.csv", HasHeaders = false,
                                        Schema = "type,doneForDay,fileGenerationDate,reporter,submitter,fileVersion,files,thirdParty (string option)">

       type FileCounter = JsonProvider< "../../CatCore/CatCore/lastruntime.json", RootName = "Update",
                                            InferTypesFromValues = false>

       type CatConfig = JsonProvider< "../../CatCore/CatCore/catconfig.json">

       type IMConfig = {imid:string;
                       custMap:Map<string, CatConfig.CustomerAccount>;
                       bdAccounts:Map<string, CatConfig.BDaccount>;
                       destinations:Map<string, CatConfig.DestImidVal>}

       exception BadRomMessage of string
       exception UnknownExchange of string
       exception UnknownAccount of string

       let roeID = 
           let roe = ref 0
           fun () ->
                  roe := !roe + 1
                  !roe

       let createRoeID(destid:string, romtime:string) = 
               let num = roeID()
               romtime.[0..7] + "_" + (num.ToString()) + destid + System.DateTime.Now.Ticks.ToString()

        
       exception BadCatConfig of string 

       let buildCustomerAccountMap (array:CatConfig.CustomerAccount[]) =
           array |> Array.map(fun (elem:CatConfig.CustomerAccount) -> match elem.Name.String with
                                                                       | None -> match elem.Name.Number with 
                                                                                   | None -> match elem.FirmId.String with 
                                                                                               |None -> match elem.FirmId.Number with
                                                                                                           | None -> raise(BadCatConfig("No Name or firmid for account"))
                                                                                                           | Some x -> (x.ToString(), elem)
                                                                                               |Some x -> (x, elem)
                                                                                   | Some x -> (x.ToString(), elem)
                                                                       | Some x -> (x, elem)) |> Map.ofArray

       let buildBDAccountMap (aRay:CatConfig.BDaccount[]) =
           aRay |> Array.map(fun (elem:CatConfig.BDaccount) -> match elem.Name.String with
                                                               |None -> match elem.Name.Number with
                                                                           | None -> match elem.FirmId.String with                                                                          
                                                                                       |None -> match elem.FirmId.Number with
                                                                                                   | None -> raise(BadCatConfig("No Name or firmid for account"))
                                                                                                   | Some x -> (x.ToString(), elem)
                                                                                       |Some x -> (x, elem)
                                                                           | Some x -> (x.ToString(), elem)
                                                               |Some x -> (x, elem)) |> Map.ofArray
      
       let buildDestinationMap(aRay:CatConfig.DestImidVal[]) =
           aRay |> Array.map(fun (elem:CatConfig.DestImidVal) -> (elem.Destid.ToString(), elem)) |>Map.ofArray

       let getIMIDS (config:CatConfig.AccountToImid []) =    
           config |> Array.map(fun elem ->  {imid = elem.ImidVal;
                                             custMap = buildCustomerAccountMap(elem.CustomerAccounts);
                                             bdAccounts = buildBDAccountMap(elem.BDaccounts);
                                             destinations = buildDestinationMap(elem.DestImidVals)})
                                        

       let fileNum = 
            let counter = ref 0
            fun () ->
                   counter := !counter + 1
                   !counter


       let createDateNGroup(groupid, offset)=
            let i = (fileNum() + offset)
            sprintf "%04d%02d%02d_%s%d_OrderEvents_%06d"  System.DateTime.Now.Year  System.DateTime.Now.Month  System.DateTime.Now.Day groupid i i
    
        (*%2X*)
       let hexEncode hash =
            (hash |> Array.map(sprintf "%02X")) |> Array.fold (+) ""

       let computeHash (filename:MemoryStream) =
            filename.GetBuffer() |> (new SHA256Managed()).ComputeHash |> hexEncode 

       let zipper (fileToZip:MemoryStream) =
            let zipTargetStream = new MemoryStream()
            ICSharpCode.SharpZipLib.BZip2.BZip2.Compress(fileToZip, zipTargetStream, true, 4096)
            zipTargetStream

       let resetLastRunConfig(preOff:int) =
            let curFile = (fileNum() + preOff)
            let date = sprintf "%04d%02d%02d" System.DateTime.Now.Year System.DateTime.Now.Month  System.DateTime.Now.Day
            FileCounter.Update(System.Convert.ToDecimal(curFile), date)


       let getCurrentFileNum (lRun:FileCounter.Update)=
            let date = sprintf "%04d%02d%02d" System.DateTime.Now.Year System.DateTime.Now.Month  System.DateTime.Now.Day
            match lRun.LastRunDate.Equals(date) with
            | true -> System.Convert.ToInt32(lRun.FileNum)
            | false -> 0

