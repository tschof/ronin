namespace cat1

module CatConfig =

    open System.Net
    open System
    open System.IO
    open FSharp.Collections
    open FSharp.Data
 
    type FinraSiteIMID = {crdid:string; firmName:string; IMID:string; exchangeID:string}

    type DestIMID = {crdid:string; IMID:string; exchangeID:string; destid:string; desttype:string; sectype:string}

    type DestData = CsvProvider< "../CATWeb1/data/roprodestlist.csv">
 
    type CatConfig = JsonProvider< "../CATWeb1/data/catconfig.json">

    type IMConfig = {imid:string;
                     custMap:Map<string, CatConfig.CustomerAccount>;
                     bdAccounts:Map<string, CatConfig.BDaccount>;
                     destinations:Map<string, CatConfig.DestImidVal>}

    type IMCat = JsonProvider< "../CATWeb1/data/IM_v2.2.1.json">

    exception BadCatConfig of string 

    let produceConfig = CatConfig.GetSample()

    let getIMCat = IMCat.GetSample()

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

    let getIMIDS () =
        let config = produceConfig
        config.AccountToImid |> Array.map(fun elem ->  {imid = elem.ImidVal;
                                                        custMap = buildCustomerAccountMap(elem.CustomerAccounts);
                                                        bdAccounts = buildBDAccountMap(elem.BDaccounts);
                                                        destinations = buildDestinationMap(elem.DestImidVals)})


    let fetchTextWebpage callback url searchstr =
        let req = WebRequest.Create(Uri(url))
        use response = req.GetResponse()
        use stream = response.GetResponseStream()
        use reader = new IO.StreamReader(stream)
        callback reader searchstr


    let parseIMIDStr (line:string) =
            let parts = line.Split([|'|'|], StringSplitOptions.None)
            {crdid = parts.[0]
             firmName = parts.[2]
             IMID = parts.[3]
             exchangeID = parts.[4]}
        

    let sortIMIDVals (reader:IO.StreamReader) filterStr =
        let imidvals = reader.ReadToEnd()
        let lines = imidvals.Split([|'\n'|], StringSplitOptions.RemoveEmptyEntries)
        let r1 = lines |> Array.filter (fun h -> h.Contains filterStr)
        r1 |> Array.map parseIMIDStr 

    let produceCATIMIDs =
        let eqDests = DestData.GetSample()
        eqDests.Rows |> Seq.map (fun eqdest -> (eqdest.Destid.ToString(), 
                                                {crdid = eqdest.Crd.ToString();
                                                IMID = eqdest.IMID;
                                                exchangeID = eqdest.ExchangeID;
                                                destid = eqdest.Destid.ToString();
                                                desttype = eqdest.DestinationType;
                                                sectype = eqdest.Sectype}))
        |> Map.ofSeq

    type DestBuilder = JsonProvider<"../CATWeb1/data/destinations.json">
        
    let buildJsonDests() =
        let config = produceConfig
        let outDests = File.Open(config.OutputDir + "rp_dests.json", FileMode.Create)
        let sw = new StreamWriter(outDests)
        let eqDests = DestData.GetSample()
        eqDests.Rows |> Seq.iter(fun acc ->
                                    match acc.Destid.HasValue with
                                    | false -> ()
                                    | true when acc.Destid.Value = 0 -> ()
                                    | true ->
                                        let destid = acc.Destid.Value
                                        let lDest = DestBuilder.Root(acc.Crd, 
                                                                     (match acc.FINRAMember with
                                                                     |"Y" -> true
                                                                     | _ -> false) ,
                                                                     acc.FirmName,
                                                                     acc.IMID,
                                                                     acc.ExchangeID,
                                                                     destid, acc.DestinationType, acc.Sectype)
                                        lDest.JsonValue.WriteTo(sw, JsonSaveOptions.None)
                                        sw.Flush()
                                        sw.Write(",")
                                        sw.Flush())

    let buildCustomerDests imid firmname=
        let config = produceConfig
        let outDests = File.Open(config.OutputDir + firmname + "custrp_dests.json", FileMode.Create)
        let sw = new StreamWriter(outDests)
        let eqDests = DestData.GetSample()
        eqDests.Rows |> Seq.iter(fun acc ->
                                    match acc.Destid.HasValue with
                                    | false -> ()
                                    | true when acc.Destid.Value = 0 -> ()
                                    | true ->
                                        let destid = acc.Destid.Value
                                        let lDest = DestBuilder.Root(acc.Crd, 
                                                                     (match acc.FINRAMember with
                                                                     |"Y" -> true
                                                                     | _ -> false) ,
                                                                     firmname,
                                                                     imid,
                                                                     "DART",
                                                                     destid, "F", acc.Sectype)
                                        lDest.JsonValue.WriteTo(sw, JsonSaveOptions.None)
                                        sw.Flush()
                                        sw.Write(",")
                                        sw.Flush())
    
    type CustomerAccount = JsonProvider<"../CATWeb1/data/custAccountDef.json">
    
    let produceJsonVals () =
        let config = produceConfig
        let outCusts = File.Open(config.OutputDir + "ronin_capital_accounts.json", FileMode.Create)
        let sw = new StreamWriter(outCusts)
        let rCusts = File.ReadAllLines(config.OutputDir + "ronin_cust")
        rCusts |> Array.iteri(fun i acc -> 
                                let lc = acc.Trim()
                                let tstr = i.ToString() + lc
                                let lCust = CustomerAccount.Root(true, "O", tstr, "A", lc)                          
                                lCust.JsonValue.WriteTo(sw, JsonSaveOptions.None)
                                sw.Flush()
                                sw.Write(",")
                                sw.Flush()) 
        
    type BDAccount = JsonProvider<"../CATWeb1/data/bdaccounts.json">
    let createBDAccounts () =
        let config = produceConfig
        let outCusts = File.Open(config.OutputDir + "bd_accounts.json", FileMode.Create)
        let sw = new StreamWriter(outCusts)
        let rCusts = File.ReadAllLines(config.OutputDir + "ronin_cust")
        rCusts |> Array.iteri(fun i acc -> 
                                let lc = acc.Trim()
                                let tstr = i.ToString() + lc
                                let lCust = BDAccount.Root(true, "O", tstr, "A", lc, "RNCB")                          
                                lCust.JsonValue.WriteTo(sw, JsonSaveOptions.None)
                                sw.Flush()
                                sw.Write(",")
                                sw.Flush()) 
        