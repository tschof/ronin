module destTypes

open FSharp.Data


type Destination = JsonProvider< "../RivalBINCBOE/Destinations.json">



[<CLIMutable>]
type DestFilter =
    {SecType: Option<string>
     Imid: Option<string>
     DestType:Option<string>
     Destid:Option<int>
     DestRouteID:Option<int>}

