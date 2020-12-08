// Learn more about F# at http://fsharp.org

open System

[<EntryPoint>]
let main argv =
    printfn "Hello World from F#!"
    scripter1.AccountRipper.getCplxPull "WOLOSEPTCPLX" scripter1.AccountRipper.woloAccounts scripter1.AccountRipper.monthDays2 "2019" "09"
    scripter1.AccountRipper.getCplxPull "WOLOJANCPLX" scripter1.AccountRipper.woloAccounts scripter1.AccountRipper.monthDays "2020" "01"
    scripter1.AccountRipper.getCplxPull "WOLODECCPLX" scripter1.AccountRipper.woloAccounts scripter1.AccountRipper.monthDays "2019" "12"
    scripter1.AccountRipper.getCplxPull "TRDAIFEBCPLX" scripter1.AccountRipper.trdai scripter1.AccountRipper.feb "2019" "02"
    scripter1.AccountRipper.getCplxPull "TRDAIOCTCPLX" scripter1.AccountRipper.trdai scripter1.AccountRipper.monthDays "2019" "10"
    scripter1.AccountRipper.getCplxPull "OptionalStratCPLX" scripter1.AccountRipper.optionalStrat scripter1.AccountRipper.monthDays "2019" "07"
    scripter1.AccountRipper.getCplxPull "OLEATCPLX" scripter1.AccountRipper.oleat scripter1.AccountRipper.monthDays "2019" "03"
    scripter1.AccountRipper.getCplxPull "BAYCPLX" scripter1.AccountRipper.bayesian scripter1.AccountRipper.monthDays2 "2019" "04"
    scripter1.AccountRipper.getCplxPull "OPIOLACPLX" scripter1.AccountRipper.opiola scripter1.AccountRipper.monthDays "2019" "05"
    scripter1.AccountRipper.getCplxPull "GEGORIOUCPLX" scripter1.AccountRipper.gregoriou scripter1.AccountRipper.monthDays2 "2019" "06"
    scripter1.AccountRipper.getCplxPull "OPPORTUNISTIC_CPLX" scripter1.AccountRipper.opportunistic scripter1.AccountRipper.monthDays2 "2019" "06"
   
    0// return an integer exit code
