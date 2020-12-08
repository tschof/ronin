namespace SPanther


open Amazon.Lambda.Core
open Amazon.Lambda.S3Events

open Amazon.S3
open Amazon.S3.Util

open System.Threading.Tasks
open FSharp.Data

module waiter = 
    let rec ready(resp:System.Threading.Tasks.Task, context: ILambdaContext) =
        match resp.Status with
        | TaskStatus.Created
        | TaskStatus.WaitingToRun
        | TaskStatus.WaitingForActivation -> resp.RunSynchronously()
        | TaskStatus.RanToCompletion -> ()
        | TaskStatus.Running
        | TaskStatus.WaitingForChildrenToComplete -> ready(resp, context) 
        | TaskStatus.Faulted
        | TaskStatus.Canceled
        |_ -> match resp.Exception with
                   |null -> match resp.IsCanceled with
                            | true -> ()
                            | false -> ready(resp, context)
                   | ex ->  sprintf "Caught another exception %s" ex.Message |> context.Logger.LogLine

    let getAlias (alias:string, fName:string) =
        match alias.Length with
        | 0 -> "$LATEST"
        |_ -> match alias.Equals(fName) with
                | true -> "$LATEST"
                | false -> alias
    
                 
// Assembly attribute to enable the Lambda function's JSON input to be converted into a .NET class.
[<assembly: LambdaSerializer(typeof<Amazon.Lambda.Serialization.Json.JsonSerializer>)>]
()



type JConfig = JsonProvider<"../SPanther/catconfig.json">

type Function(s3Client: AmazonS3Client) =
    new() = Function(new AmazonS3Client())

    /// <summary>
    /// This method is called for every Lambda invocation. This method takes in an S3 event object and can be used
    /// to respond to S3 notifications.
    /// </summary>
    /// <param name="event"></param>
    /// <param name="context"></param>
    /// <returns></returns>
    member __.FunctionHandler (event: S3Event) (context: ILambdaContext) =

        let fetchContentType (s3Event: S3EventNotification.S3Entity) = async {
            
            sprintf "befor s3Client GetObjectAsync ready status: %s, Object: %s" s3Event.Bucket.Name s3Event.Object.Key   
            |> context.Logger.LogLine
            
            let resp = s3Client.GetObjectAsync(s3Event.Bucket.Name, s3Event.Object.Key).Result
            
            sprintf "After Call to get obj asyn ready status: %s " (resp.Key)   
            |> context.Logger.LogLine
            let reader = new System.IO.StreamReader(resp.ResponseStream)
            
            sprintf "bytes read: %s" (reader.ReadLine())
            |> context.Logger.LogLine
            reader.Close()
            try
                let cp = new Amazon.S3.Model.CopyObjectRequest()
                cp.SourceBucket <- s3Event.Bucket.Name
                cp.SourceKey <- s3Event.Object.Key
                cp.SourceVersionId <- s3Event.Object.VersionId
            
                cp.DestinationBucket <- "roprocattest2"
                cp.DestinationKey <- "RoProFinra/" + s3Event.Object.Key 
                sprintf "Start with Copy: %s, Object: %s" cp.SourceBucket cp.DestinationBucket
                |> context.Logger.LogLine
                let! _cpFile = s3Client.CopyObjectAsync(cp) |> Async.AwaitTask 
                sprintf "Done with Copy: %s, Object: %s" cp.SourceKey cp.DestinationKey  
                |> context.Logger.LogLine
                let dl = new Amazon.S3.Model.DeleteObjectRequest()
                dl.BucketName <- s3Event.Bucket.Name
                dl.Key <- s3Event.Object.Key
                dl.VersionId <- s3Event.Object.VersionId
                sprintf "Start with Delete: %s, Object: %s" dl.BucketName dl.Key
                |> context.Logger.LogLine
                let! _dlFile = s3Client.DeleteObjectAsync(dl) |> Async.AwaitTask
                sprintf "Done with Delete: %s, Object: %s" dl.BucketName dl.Key
                |> context.Logger.LogLine
            with
                | ex -> ex.Message |> context.Logger.LogLine

            let fName = context.FunctionName
            let fArn = context.InvokedFunctionArn
            let arnStr = fArn.Split(':')
            let alias = arnStr.[arnStr.Length - 1]
            
            let configName = (waiter.getAlias(alias, fName)) + "/catconfig.json"
            sprintf "About to Try loading Json: %s, Object: %s" configName fArn  
            |> context.Logger.LogLine
                                      
            try
                let lister1 = new Amazon.S3.Model.ListObjectsRequest()
                lister1.BucketName <- "cat-configs"
                let objs = s3Client.ListObjectsAsync(lister1).Result
                objs.S3Objects |> Seq.iter(fun elem -> (sprintf "Object %s in %s" elem.Key elem.BucketName) |> context.Logger.LogLine)
                let request1 = new Amazon.S3.Model.GetObjectRequest()
                request1.BucketName <- "cat-configs"
                request1.Key <- configName
                let dfile = s3Client.GetObjectAsync(request1).Result
                sprintf "Key %s BucketName: %s versionid %s" dfile.Key dfile.BucketName dfile.VersionId
                |> context.Logger.LogLine 
                let configReader = new System.IO.StreamReader(dfile.ResponseStream)
                let cFile = JConfig.Parse(configReader.ReadToEnd())
                sprintf "Done loading Json: %s, Object: %s" configName cFile.SftpUser  
                |> context.Logger.LogLine
            with
                | ex -> ("Failed with the file " + ex.Message) |> context.Logger.LogLine            
        }

        fetchContentType(event.Records.Item(0).S3)
        |> Async.RunSynchronously
        