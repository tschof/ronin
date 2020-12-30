namespace cat1

module RetrieveCSVFileRecs  =
 
    type SecType =
        | Equity = 0
        | Option = 1
        | Complex = 2

    type FFileQuery = FFileQuery of tdir:string * year:string * month:string * day:string
    exception BadSecType of string

    let createCorrectSecType wtype =
        match wtype with
        | 0 -> SecType.Equity
        | 1 -> SecType.Option
        | 2 -> SecType.Complex
        | _ -> raise (BadSecType("Incorrect SecType enumeration passed in")) 

    let createFFileQuery stype year month day : FFileQuery =
        let dirConfigs = CatConfig.produceConfig  
        let ldir = 
            match stype with
            | SecType.Equity -> dirConfigs.Directory.ToString() + dirConfigs.EquityDir.ToString()
            | SecType.Option -> dirConfigs.Directory.ToString() + dirConfigs.OptionsDir.ToString()
            | SecType.Complex -> dirConfigs.Directory.ToString() + dirConfigs.SpreadDir.ToString()
            |_ -> raise (BadSecType("Incorrect SecType enumeration passed in"))
        FFileQuery(ldir, year, month, day)

    let RetrieveROMCSVRecords (fquery:FFileQuery) = 
        let romDir = 
            match fquery with
            |FFileQuery(tdir, year, month, day) -> (string tdir) + "\\" + (string year) + "\\ROM" + "\\" + (string year) + (string month) + (string day) + "\\"
        System.IO.Directory.GetFiles(romDir)
        
     
    let RetrieveEquityDirRecords(fquery:FFileQuery) =
        let romDir = 
            match fquery with
            |FFileQuery(tdir, year, month, day) -> (string tdir) + "\\" + (string year) + "\\ROM" + "\\" + (string year) + (string month) + (string day) + "\\"
        let romfiles = System.IO.Directory.GetFiles(romDir)
        let romNYDir = 
            match fquery with
            |FFileQuery(tdir, year, month, day) -> (string tdir) + "\\" + (string year) + "\\ROM_PAN" + "\\" + (string year) + (string month) + (string day) + "\\"
        let romPanFiles = System.IO.Directory.GetFiles(romNYDir) 
        let romSumo = 
            match fquery with
            |FFileQuery(tdir, year, month, day) -> (string tdir) + "\\" + (string year) + "\\ROM_SUMO" + "\\" + (string year) + (string month) + (string day) + "\\"
        let romSumoFiles = System.IO.Directory.GetFiles(romSumo)
        Array.concat [romfiles;romPanFiles;romSumoFiles]

        
 



