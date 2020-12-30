namespace RivalPanther

module MetaFilePackager =

    open System.IO
    open System.Security.Cryptography
    open FSharp.Data

    type MetaDataC = CsvProvider< "../RivalPanther/metadata.csv", HasHeaders = false,
                                    Schema = "type,doneForDay,fileGenerationDate,reporter,submitter,fileVersion,files,thirdParty (string option)">

    type FileCounter = JsonProvider< "../RivalPanther/lastruntime.json", RootName = "Update",
                                      InferTypesFromValues = false>

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
        filename.ToArray() |> (new SHA256Managed()).ComputeHash |> hexEncode 

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
