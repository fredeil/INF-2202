module Server.Ais

open System
open System.IO
open System.Net.Sockets
open System.Threading

open Reaction
open NAisParser
open Shared

let ofAis () : IAsyncObservable<MessageType123> =
    let parser = new Parser()
    let server = "153.44.253.27"
    let port = 5631

    let worker (obv: IAsyncObserver<MessageType123>) (token: CancellationToken)  = async {
        let client = new TcpClient ()
        client.Connect (server, port)
        let stream = client.GetStream ()
        use reader = new StreamReader (stream)

        while not token.IsCancellationRequested do
            let! line = reader.ReadLineAsync () |> Async.AwaitTask
            let result = parser.Parse line
            match result with
            | Ok aisResult ->
                match aisResult.Type with
                | 1uy | 2uy | 3uy ->
                    let res = parser.ParseType123 aisResult
                    match res with
                    | Result.Ok msg ->
                        do! obv.OnNextAsync msg
                    | Result.Error err -> ()
                // | 4uy ->
                //     let res = parser.ParseType4 aisResult
                //     match res with
                //     | Result.Ok msg ->
                //         do! obv.OnNextAsync msg
                //     | Result.Error err -> ()
                // | 5uy ->
                //     let res = parser.ParseType5 aisResult
                //     match res with 
                //     | Result.Ok msg ->
                //         do! obv.OnNextAsync msg
                //     | Result.Error err -> ()                                                                        
                | _ ->
                    ()
            | Error err ->
                printfn "Got exception: %A" (err.ToString ())
            | Continue ->
                ()
    }

    Create.ofAsyncWorker(worker)

/// Converts from AIS MessageType123 to Msg
let ais2Msg (aisMsg : MessageType123) : Msg =
    {
        Mmsi = aisMsg.Mmsi
        Time = DateTime.UtcNow
        Latitude = aisMsg.Latitude
        Longitude = aisMsg.Longitude
    }
