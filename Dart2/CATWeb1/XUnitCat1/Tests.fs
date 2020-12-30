module Tests

open System
open FSharp.Data
open Xunit


[<Fact>]
let ``Get SecType `` () =
    let stype =  cat1.RetrieveCSVFileRecs.createCorrectSecType 0
    Assert.Equal(cat1.RetrieveCSVFileRecs.SecType.Equity, stype)

[<Fact>]
let ``Get JSON toplevel `` () =
    let jason = cat1.CatConfig.getIMCat
    let info = jason.Description
    Assert.Equal("CAT schemas for Industry Member reportable events", info)

[<Fact>]
let ``Parse JSON event defs `` () =
    let jason = cat1.CatConfig.getIMCat
    let evtdef = jason.EventDefinitions
    try
        Assert.Equal(32, evtdef.Length)
    with
        | :? Xunit.Sdk.EqualException -> Assert.True(false)
    evtdef |> Array.iter (fun event -> Assert.NotNull(event.EventName)
                                       Assert.True(event.EventName.Length <= 5))


[<Fact>]
let ``Confirm IM message types `` () =
    let jason = cat1.CatConfig.getIMCat
    let dtypes = jason.DataTypes
    let mtypes = dtypes |> Array.filter(fun ddtype -> ddtype.DataType.Equals("Message Type")) 
    Assert.True(mtypes.Length.Equals(1))
    mtypes |> Array.iter(fun mmtype -> 
                                    let mmmtype = mmtype.AllowedValues.Strings 
                                    mmmtype |> Array.iter (fun aval -> Assert.NotNull(aval)))                     
     
    

[<Fact>]
let ``Get EX fer bad sectype`` () =
    try 
        let jj = cat1.RetrieveCSVFileRecs.createCorrectSecType 8
        Assert.Null(jj)
    with 
       | :? cat1.RetrieveCSVFileRecs.BadSecType -> Assert.True(true)
       | _ -> Assert.False(true)

[<Fact>]
let ``Test create File Query`` () =
    let jType = cat1.RetrieveCSVFileRecs.createCorrectSecType 1
    Assert.Equal(cat1.RetrieveCSVFileRecs.SecType.Option, jType)
    let ff = cat1.RetrieveCSVFileRecs.createFFileQuery jType "2018" "08" "11"
    match ff with
    | cat1.RetrieveCSVFileRecs.FFileQuery (tdir, year, month, day) -> Assert.Equal("11", day)
                                                                      Assert.Equal("2018", year)
                                                                      Assert.Equal("08", month)

(*[<Fact>]
let ``Get equity data for ROM 2019 11 26`` () =
    let lstype = cat1.RetrieveCSVFileRecs.createCorrectSecType 0
    let ffiler = cat1.RetrieveCSVFileRecs.createFFileQuery lstype "2019" "11" "26"
    let actual = cat1.RetrieveCSVFileRecs.RetrieveROMCSVRecords ffiler
    Assert.NotEmpty(actual)

[<Fact>]
let ``Get Complex sec type`` () =
    let lstype = cat1.RetrieveCSVFileRecs.createCorrectSecType 2
    let ffiler = cat1.RetrieveCSVFileRecs.createFFileQuery lstype "2019" "11" "26"
    let actual = cat1.RetrieveCSVFileRecs.RetrieveROMCSVRecords ffiler
    actual |> Array.iter (fun sact -> Assert.Contains("20191126", sact))
    *)
[<Fact>]
let ``Check Parse IMID str``() =
    let vals = "140802|Y|RONIN PROFESSIONAL LLC|319|NOM||"
    let v2 = cat1.CatConfig.parseIMIDStr vals
    Assert.Equal("319",  v2.IMID)
    Assert.Equal("RONIN PROFESSIONAL LLC", v2.firmName)
    Assert.Equal("NOM", v2.exchangeID)
    Assert.Equal("140802", v2.crdid)
    
[<Fact>]
let ``Get IMID Vals`` () =
    let url = "https://files.catnmsplan.com/firm-data/IMID_Daily_List.txt"
    let v2 = cat1.CatConfig.fetchTextWebpage cat1.CatConfig.sortIMIDVals url "RONIN PROFESSIONAL LLC"
    v2 |> Array.iter (fun arg -> Assert.Equal("RONIN PROFESSIONAL LLC", arg.firmName))

[<Fact>]
let ``Get equity dests `` () =
    let dests = cat1.CatConfig.produceCATIMIDs
    Assert.NotEmpty(dests)
    Assert.Equal("EDGA", dests.["215"].exchangeID)
                                 

[<Fact>]
let ``Load Dem Configs`` () =
    let configs = cat1.CatConfig.produceConfig
    Assert.Equal ("EquityTraffic", configs.EquityDir)

[<Fact>]
let ``Build Handling Instructions`` () = 
    let rm = cat1.Converters.RomMessage.Parse("E,,REDSOXS,,1,FXI,100,15.4,1,0,A,0501,AC8E1209,153,0,,,,,,,,DODO,ABN,REDSOXS,,,,,,,,,,,E,,,,,,,,,,,,,,,,,,,,,,P,,,,,,,,,,,,,,,,,,,,,,,").Rows |> List.ofSeq 
    match rm with
    | head::tail -> let hTech = cat1.Converters.buildHandlingInstructions(head)
                    Assert.NotNull(hTech)
    | _ -> Assert.False(true)
    ()
    

[<Fact>]
let ``Load Alternate IMIDs `` () = ()
(*
    let configs = cat1.CatConfig.produceConfig
    configs.AccountToImid |> Seq.iter(fun acc ->
                                        Assert.NotNull(acc.ImidVal)
                                        Assert.NotEmpty(acc.Accounts.Strings)
                                        if acc.ImidVal.Equals("SUMO") then
                                          Assert.Contains("31XJ1209",acc.Accounts.Strings)
                                          Assert.DoesNotContain("CRON067", acc.Accounts.Strings)
                                        else if acc.ImidVal.Equals("DART") then
                                            Assert.Contains("PAR02", acc.Accounts.Strings)
                                            Assert.DoesNotContain("31XJ1209", acc.Accounts.Strings))
                                            *)
                                        


[<Fact>]
let `` Load Customer Accounts `` () =
    let configs = cat1.CatConfig.produceConfig
    configs.AccountToImid |> Array.iter (fun imid -> Assert.NotNull(imid))

[<Fact>]
let ``Load base dir``() =
    let configs = cat1.CatConfig.produceConfig 
    Assert.Equal ("C:\\\\Dart_Operations\\", configs.Directory)

[<Fact>]
let ``Create meta file``() =()
    (*let f1 = cat1.FilePackager.createDataFile("Test1")
    let f2 = cat1.FilePackager.createDataFile("Test2")
    let f3 = cat1.FilePackager.createDataFile("Test3")
    cat1.FilePackager.createMetaFile([f1; f2; f3], "Test4")
    Assert.NotEqual(f1, f2)
    Assert.NotEqual(f2,f3)
    
    
    private static void ReadS3Object(AmazonS3Client client)
    {
        GetObjectRequest request = new GetObjectRequest();

        request.WithBucketName(BUCKET_NAME);
        request.WithKey(S3_KEY);

        GetObjectResponse response = client.GetObject(request);

        StreamReader reader = new StreamReader(response.ResponseStream);

        String content = reader.ReadToEnd();

        Console.Out.WriteLine("Read S3 object with key " + S3_KEY + " in bucket " + BUCKET_NAME + ". Content is: " + content);
    }
    
    *)

[<Fact>]
let ``Test fileNum ``() =
    Assert.Equal(1, cat1.FilePackager.fileNum())
    Assert.Equal(2, cat1.FilePackager.fileNum())
    Assert.Equal(3, cat1.FilePackager.fileNum())

[<Fact>]
let ``Get OrigTimes``() =
    let lstype = cat1.RetrieveCSVFileRecs.createCorrectSecType 0
    let ffiler = cat1.RetrieveCSVFileRecs.createFFileQuery lstype "2020" "03" "17"
    let m1 = cat1.Converters.getOrigTimes(ffiler)
    match m1.TryFind("MM8F-580191126-6")with
    |None -> Assert.False(false)
    |Some _ -> Assert.True(false)
    

[<Fact>]
let ``CSV ta JSON one`` () =
    //let url = "https://files.catnmsplan.com/firm-data/IMID_Daily_List.txt"
    //let v2 = cat1.CatConfig.fetchTextWebpage cat1.CatConfig.sortIMIDVals url "RONIN PROFESSIONAL LLC"
    let lstype = cat1.RetrieveCSVFileRecs.createCorrectSecType 0
    let ffiler = cat1.RetrieveCSVFileRecs.createFFileQuery lstype "2020" "03" "17"
    (*let actual = cat1.RetrieveCSVFileRecs.RetrieveROMCSVRecords ffiler
    cat1.Converters.generateCsvSchema
    cat1.CatConfig.produceJsonVals()
    cat1.CatConfig.buildJsonDests()
    cat1.CatConfig.createBDAccounts()
    cat1.CatConfig.buildCustomerDests "RONI" "Ronin Capital"
    cat1.CatConfig.buildCustomerDests "SUMO" "Sumo Capital"
    let head = Array.head actual
    Assert.NotEmpty(actual)
    Assert.NotNull(head)*)
    cat1.Converters.buildEquityFiles(ffiler)
    //cat1.catSftp.test
    (*let recs = cat1.Converters.munchEquityFirstRunCsv(head, (Array.head v2))
    recs |> Seq.iter(fun row -> Assert.NotNull(row))*)
                                            


