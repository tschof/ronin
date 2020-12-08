namespace cat1

module FilePackager =

    open System.IO
    open System.Security.Cryptography
    open FSharp.Data
    
    type MetaData = JsonProvider< "../CATWeb1/data/metadata.json", RootName="meta">

    type MetaDataC = CsvProvider< "../CatWeb1/data/metadata.csv", HasHeaders = false,
                                    Schema = "type,doneForDay,fileGenerationDate,reporter,submitter,fileVersion,files,thirdParty (string option)">

    type dataFile = {fs:FileStream; sw:StreamWriter;fname:string; mutable recordCount:int}

    let fileNum = 
        let counter = ref 0
        fun () ->
               counter := !counter + 1
               !counter


    let createDateNGroup( i, groupid )=
        sprintf "%04d%02d%02d_%s%d_OrderEvents_%06d"  System.DateTime.Now.Year  System.DateTime.Now.Month  System.DateTime.Now.Day groupid i i
                
    let createCsvEventFile name=
        let config = CatConfig.produceConfig
        new StreamWriter(File.Open(config.DataDir + name + ".csv", FileMode.Create))
    
    (*%2X*)
    let hexEncode hash =
        (hash |> Array.map(sprintf "%02X")) |> Array.fold (+) ""

    let computeHash filename =
        File.ReadAllBytes(filename) |> (new SHA256Managed()).ComputeHash |> hexEncode 

    let zipper (fileToZip: dataFile) =
        fileToZip.sw.Close()
        fileToZip.fs.Close()
        let zipFile = new FileInfo(fileToZip.fname  + ".bz2")
        let zipInfo = new FileInfo(fileToZip.fname)
        use zipStream = zipInfo.OpenRead()
        use zipTargetStream = zipFile.Create()
        ICSharpCode.SharpZipLib.BZip2.BZip2.Compress(zipStream, zipTargetStream, true, 4096)
        zipFile

    let createDataFile groupid = 
        let lnum = fileNum()
        let config = CatConfig.produceConfig
        let fileName = config.OutputDir + config.CatSubmitterId.ToString() + "_" + config.CatReporterImid + "_" + createDateNGroup(lnum, groupid) + ".csv"
        let mout = File.Open(fileName, FileMode.Create) 
        {fs = mout;
         sw = new StreamWriter(mout);
         fname = fileName;
         recordCount = 0}

   
    let createMetaFile (dataFiles:List<dataFile>, groupid) =
        let lnum = fileNum()
        let config = CatConfig.produceConfig
        let fileName = config.CatSubmitterId.ToString() + "_" + config.CatReporterImid + "_" + createDateNGroup(lnum, groupid) + ".meta.csv"
        let metaOut = File.Open(config.OutputDir + fileName, FileMode.Create)
        let date = sprintf "%04d%02d%02d" System.DateTime.Now.Year  System.DateTime.Now.Month  System.DateTime.Now.Day
        let zfiles = dataFiles |> List.fold(fun z fdata -> 
            let zipped = zipper fdata
            let h = computeHash zipped.FullName 
            let r = sprintf "%s@%d@@%s" zipped.Name fdata.recordCount h
            List.append z [r] ) List.empty 
        let z2=  zfiles |> String.concat "| "
        let meta = new MetaDataC([MetaDataC.Row("META", "true", date, config.CatReporterImid, config.CatSubmitterId.ToString(),"2.2.1", z2, None)])
        let sw = new StreamWriter(metaOut) 
        sw.Write(meta.SaveToString())
        sw.Flush()
        
       