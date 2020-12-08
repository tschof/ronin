module CboeBinReader

open System.IO


type dataFile = {mw:MemoryStream;sw:StreamWriter; mutable recordCount:int}



let buildOptionFile(data:System.IO.BinaryReader, imid:CatCore.PantherCore.IMConfig) =
    let memStr = new System.IO.MemoryStream()
    let ofile = {mw = memStr; sw = new System.IO.StreamWriter(memStr); recordCount = 0}
    (memStr.GetBuffer(), memStr.Length, ofile.recordCount)


