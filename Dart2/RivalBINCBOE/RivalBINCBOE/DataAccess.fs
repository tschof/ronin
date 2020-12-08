module DataAccess

open Amazon.Lambda.Core
open Amazon.Lambda.S3Events
open System
open Amazon.S3
open FSharp.Control.Tasks.V2.ContextInsensitive
open Amazon.S3.Util

open destTypes

let getDests(filter:DestFilter) = 
    
    task {
            let s3Client = new AmazonS3Client(Amazon.RegionEndpoint.USEast1)
            let destObjReq = new Amazon.S3.Model.GetObjectRequest()
            destObjReq.BucketName <- "cat-configs"
            destObjReq.Key <- "Destinations.json"
            let! dests = s3Client.GetObjectAsync(destObjReq) |> Async.AwaitTask
            let destReader = new System.IO.StreamReader(dests.ResponseStream)
            let total = Destination.Parse(destReader.ReadToEnd())
            let fDests = 
                total.Destinations 
                |> Seq.filter(fun elem -> match filter.SecType with
                                            | Some x -> elem.SecType.Equals(x)
                                            | None -> true)
                |> Seq.filter(fun elem -> match filter.Destid with
                                            | Some x -> elem.Destid.Equals(x)
                                            | None -> true)
                |> Seq.filter(fun elem -> match filter.DestRouteID with
                                            | Some x -> elem.Destid.Equals(x)
                                            | None -> true)
                |> Seq.filter(fun elem -> match filter.DestType with 
                                            | Some x -> elem.DestType.Equals(x)
                                            | None -> true)
                |> Seq.filter(fun elem -> match filter.Imid with 
                                            | Some x -> elem.Imid.Equals(x)
                                            | None -> true)
            return fDests
    }

let addDests(newDestination:Destination.Destination) =
    task {
        let s3Client = new AmazonS3Client(Amazon.RegionEndpoint.USEast1)
        let destObjReq = new Amazon.S3.Model.GetObjectRequest()
        destObjReq.BucketName <- "cat-configs"
        destObjReq.Key <- "Destinations.json"
        let! dests = s3Client.GetObjectAsync(destObjReq) |> Async.AwaitTask
        let destReader = new System.IO.StreamReader(dests.ResponseStream)
        let total = Destination.Parse(destReader.ReadToEnd())
        let mem1 = new System.IO.MemoryStream()
        let text1 = new System.IO.StreamWriter(mem1)
        total.JsonValue.WriteTo(text1, FSharp.Data.JsonSaveOptions.None)
        text1.BaseStream.Seek(System.Convert.ToInt64(0), System.IO.SeekOrigin.End) |> ignore
        let s3Client = new AmazonS3Client(Amazon.RegionEndpoint.USEast1)
        let writeRequest = new Amazon.S3.Model.PutObjectRequest()
        writeRequest.BucketName <- "cat-configs"
        writeRequest.Key <- "Destinations.json"       
        newDestination.JsonValue.WriteTo(text1, FSharp.Data.JsonSaveOptions.None)
        text1.Flush()
        writeRequest.InputStream <- mem1
     
        return! s3Client.PutObjectAsync(writeRequest) |> Async.AwaitTask
    }