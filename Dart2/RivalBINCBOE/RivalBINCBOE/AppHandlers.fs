module AppHandlers

open System
open Microsoft.Extensions.Logging
open Giraffe

open Microsoft.AspNetCore.Http
open destTypes
open FSharp.Control.Tasks


module Views = 
    open Giraffe.GiraffeViewEngine
    let page =
        div [] [
            script [_type "application/javascript"] [
                rawText """
                window.greet = function () {
                    console.log("ping from the greet method");
                }
                """
            ]
            button [_id "script-tag-js"
                    _onclick "greet();"] [str "Say Hello"]
        ]

let indexHandler  =
    fun (next : HttpFunc) (ctx : HttpContext) ->
        htmlView Views.page next ctx
        //text "Serverless Giraffe Web API" next ctx

let arrayExampleHandler (itemCount:int) =
    fun (next : HttpFunc) (ctx : HttpContext) ->
        let values = seq { for a in 1 .. itemCount do yield sprintf "value%i" a }
        text (String.concat ", " values) next ctx

let handleDestFilter (next:HttpFunc) (ctx:HttpContext) =
    task {
        let filter = ctx.BindQueryString<DestFilter>()
        let dests = DataAccess.getDests filter
        return! json dests next ctx
    }

let handleAddDest (next:HttpFunc) (ctx:HttpContext) =
    task {
        let! dest = ctx.BindJsonAsync<Destination.Destination>()
        DataAccess.addDests dest |> ignore
        return! text (sprintf "Added %d to the dests" dest.Destid) next ctx
    }

let webApp:HttpHandler =
    choose [
        GET >=>
            choose [
                route "/" >=> indexHandler
                route "/dests" >=> warbler (fun _ -> handleDestFilter)
                route "/array" >=> arrayExampleHandler 2
                routef "/array/%i" arrayExampleHandler
            ]
        POST >=> choose [route "/dests/add" >=> warbler (fun _ -> handleAddDest)]
        setStatusCode 404 >=> text "Not Found" ]

// ---------------------------------
// Error handler
// ---------------------------------

let errorHandler (ex : Exception) (logger : ILogger) =
    logger.LogError(EventId(), ex, "An unhandled exception has occurred while executing the request.")
    clearResponse >=> setStatusCode 500 >=> text ex.Message

