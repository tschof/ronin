namespace ITGTest

module ConvertITGFile =
    open FSharp.Data
    open CatCore
    open CatCore.PantherCore
    open System.IO
    
    type ITGCsv = CsvProvider<"../SumoFilesTest/itgsample.csv", "^">
    type dataFile = {mw:MemoryStream;sw:StreamWriter; mutable recordCount:int}

    (*02/28/2020 08:31:31:703*)
    let switchITGCentralToEast (itgTs:string) =
        //let parts = itgTs.Split([|'/';' ';':'|], System.StringSplitOptions.None)
        let centralTime = System.DateTime.ParseExact(itgTs, "MM/dd/YYYY HH:mm:ss:fff", System.Globalization.CultureInfo.InvariantCulture)
        let estZone = TimeZoneConverter.TZConvert.GetTimeZoneInfo("Eastern Standard Time")
        let est = System.TimeZoneInfo.ConvertTime(centralTime, estZone)
        sprintf "%04d%02d%02d %02d%02d%02d.%03d" est.Year est.Month est.Day est.Hour est.Minute est.Second est.Millisecond
    
    let translateSide side =
        match side with
        | "SELL" -> "SL"
        | "SELLSH" -> "SS"
        |_ -> "B"

    let isIceBerg (dispQty:string, qty:int) =
        match dispQty.Length with
        | 0 -> "false" 
        |_ -> match dispQty.Equals(qty.ToString()) with
                | true -> "true"
                | false -> "false"

    let getPrice(price:decimal) =
        Some (price.ToString())

    let translateTif (tif:string) =
        let parts = tif.Split([|'/';' ';':'|], System.StringSplitOptions.None)
        sprintf "Day=%s%s%s" parts.[2] parts.[0] parts.[1]

    let createNewOrderEvent(imi:PantherCore.IMConfig,
                            csv:ITGCsv.Row) =
        match imi.custMap.TryFind csv.Clearing_house_account with
            | None -> None
            | Some custAccount ->
                    let firmid = (match custAccount.FirmId.String with
                                    | None -> match custAccount.FirmId.Number with
                                                |None -> raise (PantherCore.BadCatConfig("No Firm ID for Account"))
                                                | Some x -> x.ToString()
                                    |Some x -> x)
                    let holder = custAccount.HolderType
                    Some (PantherCore.MENO.Row("NEW", None, PantherCore.createRoeID(csv.Exchange_name,
                                                csv.Mc_execution_timestamp),
                                                "MENO", Some imi.imid, 
                                                switchITGCentralToEast csv.Mc_execution_timestamp,
                                                csv.OrderId, csv.Symbol,
                                                switchITGCentralToEast csv.Mc_execution_timestamp,
                                                "false", "false", None,None,None,
                                                custAccount.DeptType, None,None,
                                                translateSide csv.Side, getPrice(csv.Order_price),
                                                (csv.Order_quantity.ToString()),
                                                None, csv.Contract_type,
                                                translateTif(csv.Mc_execution_timestamp),"REG", None,
                                                isIceBerg(csv.Display_size,csv.Order_quantity),
                                                firmid, holder, custAccount.Affiliate.ToString(),
                                                None,None,"false", "N", None, None, None, None, 
                                                None, None, None, None, None, None, None, None))

    let createOrderRouteRpt(imi:PantherCore.IMConfig,
                            csv:ITGCsv.Row) =
            let affiliateStr = match imi.custMap.TryFind csv.Clearing_house_account with
                               | None -> match imi.bdAccounts.TryFind csv.Clearing_house_account with
                                            | None -> raise(UnknownAccount(sprintf "%s%s" csv.Clearing_house_account csv.OrderId))
                                            | Some x -> x.Affiliate.ToString()
                               | Some x -> x.Affiliate.ToString()
            let dest = match imi.destinations.TryFind csv.Exchange_name with
                        |None -> raise(UnknownExchange(sprintf "%s%s"csv.Exchange_name csv.OrderId))
                        |Some x -> x
            Some (PantherCore.MEOR.Row("NEW", None, PantherCore.createRoeID(csv.Exchange_name, 
                                        csv.Mc_execution_timestamp), "MEOR", Some imi.imid,
                                        switchITGCentralToEast csv.Mc_execution_timestamp,
                                        csv.OrderId, csv.Symbol, None, switchITGCentralToEast csv.Mc_execution_timestamp,
                                        "false", "false", None, Some imi.imid, Some dest.ExchangeId, dest.DestType,
                                        Some csv.OrderId,None, translateSide csv.Side,
                                        getPrice(csv.Order_price), (csv.Order_quantity.ToString()), 
                                        None, csv.Contract_type, translateTif(csv.Mc_execution_timestamp),
                                        "REG", affiliateStr, "N", Some "RAR", None, "false", None))


    let buildEquityFile (rows:seq<ITGCsv.Row>, imi:PantherCore.IMConfig) =
        let memStr = new System.IO.MemoryStream()
        let ofile = {mw = memStr; sw = new System.IO.StreamWriter(memStr); recordCount = 0}
        (memStr.GetBuffer(), memStr.Length, ofile.recordCount)




