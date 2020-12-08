namespace RivalPanther

module ConfigBuilder =

    open FSharp.Data
       (*
         Access: AKIAUE5JMX624CYD3IJM
         Secret: 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
        *)
    type CatConfig = JsonProvider< "../RivalPanther/catconfig.json">

    type IMConfig = {imid:string;
                    custMap:Map<string, CatConfig.CustomerAccount>;
                    bdAccounts:Map<string, CatConfig.BDaccount>;
                    destinations:Map<string, CatConfig.DestImidVal>}

    type IMCat = JsonProvider< "../RivalPanther/IM-2.2.1.json">

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


