namespace RivalPanther


open Amazon.Lambda.Core
open Amazon.Lambda.S3Events

open Amazon.S3
open Amazon.S3.Util


module FilePackager =
    let writeOutFiles(imi:ConfigBuilder.IMConfig,s3Client: IAmazonS3,context: ILambdaContext,
                       jConfig:ConfigBuilder.CatConfig.Root, stuff:byte[], blen:int64, recordCount:int, fileNum:int) =
        let outMemoryStream = new System.IO.MemoryStream()
        let zipTargetStream = new ICSharpCode.SharpZipLib.BZip2.BZip2OutputStream(outMemoryStream)
        zipTargetStream.IsStreamOwner <- false
        zipTargetStream.Write(stuff, 0, System.Convert.ToInt32(blen))
        zipTargetStream.Flush()
        sprintf "added: %d, bytes to zipTarget %d" (zipTargetStream.Length) (zipTargetStream.BytesWritten) |> context.Logger.LogLine
        zipTargetStream.Close()
        
        sprintf "OutMemStream bytes Written: %d, capacity %d" (outMemoryStream.Length) (outMemoryStream.Capacity) |> context.Logger.LogLine
        let hash = MetaFilePackager.computeHash(outMemoryStream)
        let fReqest = new Amazon.S3.Model.PutObjectRequest()
        fReqest.BucketName <- jConfig.FinishedBucket
        fReqest.Key <-jConfig.CatSubmitterId.ToString() + "_" + imi.imid + "_" + MetaFilePackager.createDateNGroup(imi.imid, fileNum) + ".csv.bz2"
        fReqest.InputStream <- outMemoryStream
        sprintf "added: %d, bytes to Request input" fReqest.InputStream.Length  |> context.Logger.LogLine
        let _ = s3Client.PutObjectAsync(fReqest).Result 
        let date = sprintf "%04d%02d%02d" System.DateTime.Now.Year  System.DateTime.Now.Month  System.DateTime.Now.Day
        let r = sprintf "%s@%d@@%s" fReqest.Key recordCount hash
        let meta = new MetaFilePackager.MetaDataC([MetaFilePackager.MetaDataC.Row("META", "true", date, imi.imid, jConfig.CatSubmitterId.ToString(), "2.2.1", r, None)])
        let memWrite = new System.IO.MemoryStream()
        let sw = new System.IO.StreamWriter(memWrite)
        sw.Write(meta.SaveToString())
        sw.Flush()
        let mRequest = new Amazon.S3.Model.PutObjectRequest()
        mRequest.BucketName <- jConfig.FinishedBucket
        mRequest.Key <- jConfig.CatSubmitterId.ToString() + "_" + imi.imid + "_" + MetaFilePackager.createDateNGroup(imi.imid, fileNum) + ".meta.csv"
        mRequest.InputStream <- memWrite
        s3Client.PutObjectAsync(mRequest).Result

    let processFile(imids:seq<ConfigBuilder.IMConfig>,s3Client: IAmazonS3,context: ILambdaContext,
                    jConfig:ConfigBuilder.CatConfig.Root, rows:seq<ConvertRoProCsv.RomMessage.Row>, fileNum:int) =
        sprintf "In process File %d" (Seq.length imids) |> context.Logger.LogLine
        imids |> Seq.iter(fun elem -> let (memBuff, bLen,records) = ConvertRoProCsv.buildEquityFile(rows, elem)
                                      sprintf "Built the file %d, %s, cust:%d bd:%d file: %d, %i" records elem.imid elem.custMap.Count elem.bdAccounts.Count memBuff.Length bLen
                                      |> context.Logger.LogLine
                                      let resp = writeOutFiles(elem, s3Client, context, jConfig, memBuff, bLen, records, fileNum)
                                      sprintf "Done with process file: %s" (resp.ToString()) |> context.Logger.LogLine)

            
  
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
            sprintf "Processing object %s from bucket %s function: %s" s3Event.Object.Key s3Event.Bucket.Name context.FunctionName
            |> context.Logger.LogLine
            let getAlias (alias:string, fName:string) =
                match alias.Length with
                | 0 -> "$LATEST"
                |_ -> match alias.Equals(fName) with
                        | true -> "$LATEST"
                        | false -> alias
            
            let fArn = context.InvokedFunctionArn.Split(':')
            let alias = fArn.[fArn.Length - 1]
            
            let configName = getAlias(alias, context.FunctionName) + "/catconfig.json"            
            let configRequest = new Amazon.S3.Model.GetObjectRequest()
            configRequest.BucketName <- "cat-configs"
            configRequest.Key <- configName
            let! configFile = s3Client.GetObjectAsync(configRequest) |> Async.AwaitTask
            let configReader = new System.IO.StreamReader(configFile.ResponseStream)
            let jConfig = ConfigBuilder.CatConfig.Parse(configReader.ReadToEnd())
            sprintf "Done Loading Json:ID %d, %s" jConfig.CatSubmitterId configName
            |> context.Logger.LogLine

            let lRunConfigName = getAlias(alias, context.FunctionName) + "/" + jConfig.LastRunConfig
            let runRequest = new Amazon.S3.Model.GetObjectRequest()
            runRequest.BucketName <- "cat-configs"
            runRequest.Key <- lRunConfigName
            let! runFile = s3Client.GetObjectAsync(runRequest) |> Async.AwaitTask
            let runReader = new System.IO.StreamReader(runFile.ResponseStream)
            let lRunDate = MetaFilePackager.FileCounter.Parse(runReader.ReadToEnd())
            sprintf "Done Loading lastRunDate %d, %s" (System.Convert.ToInt32(lRunDate.FileNum)) lRunConfigName
            |> context.Logger.LogLine

            let! resp = s3Client.GetObjectAsync(s3Event.Bucket.Name, s3Event.Object.Key) |> Async.AwaitTask
            let reader = new System.IO.StreamReader(resp.ResponseStream)   
          
            let rows = ConvertRoProCsv.RomMessage.Parse(reader.ReadToEnd())
            let imids = ConfigBuilder.getIMIDS(jConfig.AccountToImid)
            let fileOff = MetaFilePackager.getCurrentFileNum(lRunDate)
            FilePackager.processFile(imids,s3Client,context,jConfig, rows.Rows, fileOff)
            sprintf "Done with process File" |> context.Logger.LogLine
            let cp = new Amazon.S3.Model.CopyObjectRequest()
            cp.SourceBucket <- s3Event.Bucket.Name
            cp.SourceKey <- s3Event.Object.Key
            cp.SourceVersionId <- s3Event.Object.VersionId
            
            cp.DestinationBucket <- jConfig.OutputBucket
            cp.DestinationKey <- jConfig.OutputDir + "/" + s3Event.Object.Key 
            sprintf "Start with Copy: %s, Object: %s SourcKey %s " cp.SourceBucket cp.DestinationBucket cp.SourceKey
            |> context.Logger.LogLine
            let! _cpFile = s3Client.CopyObjectAsync(cp) |> Async.AwaitTask
            sprintf "Done with Copy"
            |> context.Logger.LogLine
            let dl = new Amazon.S3.Model.DeleteObjectRequest()
            dl.BucketName <- s3Event.Bucket.Name
            dl.Key <- s3Event.Object.Key
            dl.VersionId <- s3Event.Object.VersionId
            sprintf "Start with Delete: %s, Object: %s" dl.BucketName dl.Key
            |> context.Logger.LogLine
            let! _dlFile = s3Client.DeleteObjectAsync(dl) |> Async.AwaitTask
            sprintf "Done with Delete"
            |> context.Logger.LogLine

            let newLastRunFile = MetaFilePackager.resetLastRunConfig(fileOff)
            let writeRequest = new Amazon.S3.Model.PutObjectRequest()
            writeRequest.BucketName <- "cat-configs"
            writeRequest.Key <- lRunConfigName
            let mem1 = new System.IO.MemoryStream()
            let text1 = new System.IO.StreamWriter(mem1)
            newLastRunFile.JsonValue.WriteTo(text1, FSharp.Data.JsonSaveOptions.None)
            text1.Flush()
            writeRequest.InputStream <- mem1
            let _ = s3Client.PutObjectAsync(writeRequest).Result
            sprintf "Done with Put of New run time %d %s" (System.Convert.ToInt32(newLastRunFile.FileNum)) newLastRunFile.LastRunDate
            |> context.Logger.LogLine
            return jConfig.OutputDir
        }

        fetchContentType (event.Records.Item(0).S3)
        |> Async.RunSynchronously


        (**)