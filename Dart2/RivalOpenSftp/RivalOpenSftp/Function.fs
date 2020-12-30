namespace RivalOpenSftp


open Amazon.Lambda.Core
open Amazon.Lambda.S3Events

open Amazon.S3
open Amazon.S3.Util


module sftper = 
    open Renci.SshNet
    open System.IO
    
    /// FSharp Async wrapper for SSH.NET SFTP
    type SftpClient with
        member x.ListDirectoryAsync path =
            Async.FromBeginEnd((fun(iar,state) ->
                x.BeginListDirectory(path, iar, state)), x.EndListDirectory)
    
        member x.DownloadFileAsync path output =
            Async.FromBeginEnd((fun(iar,state) ->
                x.BeginDownloadFile(path, output, iar, state)), x.EndDownloadFile)
    
        member x.UploadFileAsync input path =
            Async.FromBeginEnd((fun(iar,state) ->
                x.BeginUploadFile(input, path, iar, state)), x.EndUploadFile)
            |> Async.Catch
    
        member x.SynchronizeDirectoriesAsync sourcePath destinationPath searchPattern =
            Async.FromBeginEnd((fun(iar,state) ->
                x.BeginSynchronizeDirectories(
                    sourcePath, destinationPath, searchPattern, iar, state)),
                x.EndSynchronizeDirectories)
            |> Async.Catch
    
  
    
    let sftpExample host port username (password:string)  (reader:System.IO.StreamReader) (uploadfile:string) (context: ILambdaContext) =
        async {
            let workDir = "/140802/cat/upload/";
            
            use client = new SftpClient(host, port, username, password)
            client.Connect()
            sprintf "Connected to %s" host |> context.Logger.LogLine
    
            // Change directory:
            client.ChangeDirectory workDir
    
            // List files in directory:
            let! listDirectory = client.ListDirectoryAsync workDir
            listDirectory |> Seq.iter(fun file ->
                if file.IsDirectory then sprintf "/%s" file.Name |> context.Logger.LogLine
                else sprintf "%s" file.Name |> context.Logger.LogLine)            
            // Upload a file:
                
            sprintf "Uploading %s " uploadfile |> context.Logger.LogLine
            client.BufferSize <- 4096u // bypass Payload error large files
            let! r = client.UploadFileAsync reader.BaseStream uploadfile
            match r with
            | Choice1Of2 _ -> sprintf "Uploaded %s" uploadfile |> context.Logger.LogLine
            | Choice2Of2 err -> sprintf "Error uploading %s: %O" uploadfile err |> context.Logger.LogLine
    
        } |> Async.StartAsTask

// Assembly attribute to enable the Lambda function's JSON input to be converted into a .NET class.
[<assembly: LambdaSerializer(typeof<Amazon.Lambda.Serialization.Json.JsonSerializer>)>]
()


type Function(s3Client: IAmazonS3) =
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
            sprintf "Processing object %s from bucket %s" s3Event.Object.Key s3Event.Bucket.Name
            |> context.Logger.LogLine

            try
              
                let! resp = s3Client.GetObjectAsync("arn:aws:s3:us-east-1:285433708469:accesspoint/second-point", s3Event.Object.Key) |> Async.AwaitTask
                let reader = new System.IO.StreamReader(resp.ResponseStream)   

                sprintf "Got the resp with ID: %s" resp.ResponseMetadata.RequestId
                |> context.Logger.LogLine
                sprintf "About to call sftp"
                |> context.Logger.LogLine
                let test = sftper.sftpExample "10.134.100.122" 22 "cat140802ftp" "RoninProfessional1!" reader s3Event.Object.Key context
                test.Result
            with
            | ex -> sprintf "Caught this un: %s" ex.Message |> context.Logger.LogLine

            sprintf "After sftp Content Type" //response.Headers.ContentType
            |> context.Logger.LogLine

            return "Finished"//response.Headers.ContentType
        }

        fetchContentType (event.Records.Item(0).S3)
        |> Async.RunSynchronously



        (*let adder = System.Net.IPAddress.Parse("10.134.100.122")
        let ipEndPoint = new System.Net.IPEndPoint(adder, 22)
        let sock = new System.Net.Sockets.Socket(adder.AddressFamily, System.Net.Sockets.SocketType.Stream, System.Net.Sockets.ProtocolType.Tcp)
        sprintf "Before connect %s" (adder.ToString())
        |> context.Logger.LogLine
        sock.Connect(ipEndPoint)
        sprintf "After connect %s" (adder.ToString())
                   |> context.Logger.LogLine
        sock.Shutdown(System.Net.Sockets.SocketShutdown.Both);  
        sock.Close(); *)