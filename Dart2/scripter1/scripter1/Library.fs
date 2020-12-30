namespace scripter1
module AccountRipper =

    open FSharp.Data


    type AuditerFields = CsvProvider<"../scripter1/data/HEADERS.CSV", HasHeaders = false>

    type RomMessage = CsvProvider<"../scripter1/data/romsample.csv">

    let optionalStrat = ["MQNE";
                            "46138";
                            "6046";
                            "6047"]

    let henrik = ["7YVK1209";
                    "CRON090";
                    "CRON026"]

    let diori = ["CRT020"]

    let mtg = ["6053";
                "CRON013";
                "6054";
                "46037";
                "6057";
                "CRON011";
                "6058";
                "CRT019";
                "CRON012";
                "CRON048";
                "CRT018";
                "6056";
                "CRON014"]

    let opportunistic = ["46525";
                            "CRON060";
                            "46519";
                            "46140";
                            "6010"]

    let gregoriou = ["6045";
                        "46406";
                        "CRON032";
                        "46040";
                        "6044";
                        "CRON033";
                        "CRT007"]

    let opiola = ["46497"]

    let bayesian = ["15076616";
                        "50001"]

    let mercator = ["48HSF";
                    "48HS1209"]

    let oleat = ["46456";
                    "16001"]

    let rvp = ["CRON070";
                "6052"]

    let trdai = ["CRON077";
                    "46442";
                    "MQFP";
                    "46435";
                    "6000";
                    "6071";
                    "6061";
                    "46520";
                    "43801315D9";
                    "CRONC01";
                    "6019";
                    "CRON017";
                    "46088";
                    "6069";
                    "46087"]

    let sumoAccounts = ["3FF21209";
                        "3FW41209";
                        "3CT81209";
                        "31XJ1209";
                        "3JM81209"]

    let woloAccounts = ["MQJW";
                        "6041";
                        "6037";
                        "46131";
                        "CRN15";
                        "32M1209";
                        "46134";
                        "6035";
                        "6032";
                        "150-76614";
                        "14132";
                        "6039";
                        "X32DM";
                        "CRON057";
                        "46157";
                        "6033";
                        "46158";
                        "6038";
                        "CRON081";
                        "46127";
                        "MQJR";
                        "6040";
                        "46156";
                        "46128";
                        "MQWW";
                        "MQWY"]

    let regoAccounts = ["AM011209"]

    let parplusAccounts = ["PAR01";"PAR02";"PAR03";"PAR04";"PAR06";"400-0042";"400-0043";"40000042";"40000043";"PARPLEDF"]

    let baseDir = "W:\\\\Dart_Operations\\Operations\\AuditLogData\\"
    let OutputDir = "C:\\\\AuditFiles\\"

    let RetrieveEquityDirRecords year month day =
        try
            let romDir = baseDir + "EquityTraffic" + "\\" + (year) + "\\ROM" + "\\" + (year) + (month) + day + "\\"
            let romfiles = System.IO.Directory.GetFiles(romDir, "*.csv")
            let romNYDir = baseDir + "EquityTraffic" + "\\" + (year) + "\\ROM_PAN" + "\\" + (year) + (month) + (day) + "\\"
            let romPanFiles = System.IO.Directory.GetFiles(romNYDir, "*.csv") 
            let romSumo = baseDir + "EquityTraffic" + "\\" + (year) + "\\ROM_SUMO" + "\\" + (year) + (month) + (day) + "\\"
            let romSumoFiles = System.IO.Directory.GetFiles(romSumo, "*.csv")
            Array.concat [romfiles;romPanFiles;romSumoFiles]
        with
            |_ -> [||]

    let RetrieveOptionRecords year month day = 
        try
            let romDir = baseDir + "OptionsTraffic" + "\\" + (year) + "\\ROM" + "\\" + (year) + (month) + (day) + "\\"
            System.IO.Directory.GetFiles(romDir, "*.csv")
        with
            | _ -> [||]

    let RetrieveFuturesRecords year month day = 
        try 
            let romDir = baseDir + "FuturesTraffic" + "\\" + (year) + "\\ROM" + "\\" + (year) + (month) + (day) + "\\"
            System.IO.Directory.GetFiles(romDir, "*.csv")
        with    
            |_ -> [||]

    let RetrieveCplxRecords year month day = 
        try 
            let romDir = baseDir + "ComplexTraffic" + "\\" + (year) + "\\ROM" + "\\" + (year) + (month) + (day) + "\\"
            System.IO.Directory.GetFiles(romDir, "*.csv")
        with    
            |_ -> [||]


    let getAllDirs year month day =
        Array.concat [RetrieveEquityDirRecords year month day; RetrieveOptionRecords year month day; RetrieveFuturesRecords year month day]

    let createDataFile name  = 
           let fileName = OutputDir + name + ".csv"
           System.IO.File.Open(fileName, System.IO.FileMode.Create) 


    let convertTif( tif:string, time:string) =
        match tif with
        | "1" ->"GTC"
        | "3" -> "IOC"
        | "6" -> "GTD"
        | _ -> "DAY=" + time.[0..8]

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
        
    let transStatus status =
        match status with 
        | "1" -> "Part/Open"
        | "2" -> "Fill"
        |_ -> "Fill"

    let translateSide side =
        match side with
        | "1" -> "Buy"
        | "2" -> "Sell Long"
        | _ -> "Sell Short"

    let monthDays = [1 .. 31]
    let monthDays2 = [1 .. 30]
    let feb = [1 .. 28]

    

    let GetFiles(accounts:List<string>, cFiles:List<string>, sw:System.IO.StreamWriter)=
       
        cFiles
            |> List.iter(fun elem -> RomMessage.Load(elem).Rows
                                        |> Seq.filter(fun elem -> elem.Status.Equals("1") || elem.Status.Equals ("2"))
                                        |> Seq.filter(fun elem -> List.contains elem.Clearingaccount accounts)
                                        |> Seq.map(fun elem -> AuditerFields.Row(elem.Romtime, elem.Trader,
                                                                                                transStatus elem.Status,
                                                                                                translateSide elem.Side,
                                                                                                elem.Lastshares,
                                                                                                elem.Symbol,
                                                                                                elem.Price,
                                                                                                elem.Exchange, 
                                                                                                elem.Clearingaccount,
                                                                                                elem.Sectype))
                                        |> Seq.iter(fun elem -> let f = new AuditerFields([elem])         
                                                                sw.Write(f.SaveToString())
                                                                sw.Flush()))
    
    let getCplxPull fileName accounts days year month = 
        let oFile = createDataFile (fileName + "Complex")
        let sw = new System.IO.StreamWriter(oFile)
        sw.WriteLine("Time,Trader,Status,Side,Executed Shares,Symbol,Price,Order Dest,ClearingAccount,SecType")
        sw.Flush()
        days
        |> List.iter(fun elem -> let eFiles = RetrieveCplxRecords year month (sprintf "%02d" elem)
                                 GetFiles(accounts, List.ofArray(eFiles), sw))
        sw.Flush()
        sw.Close()
        oFile.Close()

    let genPull fileName accounts days year month =
        let oFile = createDataFile (fileName + "Equities")
        let sw = new System.IO.StreamWriter(oFile)
        sw.WriteLine("Time,Trader,Status,Side,Executed Shares,Symbol,Price,Order Dest,ClearingAccount,SecType")
        sw.Flush()
        days
        |> List.iter(fun elem -> let eFiles = RetrieveEquityDirRecords year month (sprintf "%02d" elem)
                                 GetFiles(accounts, List.ofArray(eFiles), sw))
        sw.Flush()
        sw.Close()
        oFile.Close()
        let OptFile = createDataFile (fileName + "Options")
        let OptSW = new System.IO.StreamWriter(OptFile)
        OptSW.WriteLine("Time,Trader,Status,Side,Executed Shares,Symbol,Price,Order Dest,ClearingAccount,SecType")
        OptSW.Flush()
        days
        |> List.iter(fun elem -> let eFiles = RetrieveOptionRecords year month (sprintf "%02d" elem)
                                 GetFiles(accounts, List.ofArray(eFiles), OptSW))
        OptSW.Flush()
        OptSW.Close()
        OptFile.Close()
        let FutFile = createDataFile (fileName + "Futures")
        let FutSW = new System.IO.StreamWriter(FutFile)
        FutSW.WriteLine("Time,Trader,Status,Side,Executed Shares,Symbol,Price,Order Dest,ClearingAccount,SecType")
        FutSW.Flush()
        days
        |> List.iter(fun elem -> let eFiles = RetrieveFuturesRecords year month (sprintf "%02d" elem)
                                 GetFiles(accounts, List.ofArray(eFiles), FutSW))
        FutSW.Flush()
        FutSW.Close()
        FutFile.Close()



