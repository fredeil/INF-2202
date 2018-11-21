module Server.Main
open System
open System.IO
open System.Threading.Tasks
open Microsoft.AspNetCore
open Microsoft.AspNetCore.Builder
open Microsoft.AspNetCore.Hosting
open Microsoft.Extensions.DependencyInjection
open Microsoft.Extensions.Logging
open FSharp.Control.Tasks.V2.ContextInsensitive
open Giraffe
open Giraffe.Serialization
open Reaction.Giraffe.Middleware
open Reaction
open Reaction.AsyncObservable
open NAisParser
open Shared
open Server.Ais

let publicPath = Path.GetFullPath "../Client/public"
let port = 8085us

let webApp =
    // Not used, ignore
    route "/ping" >=> text "pong"

    /// Predicate for filtering out MessageTypes that are outside 100km radius of TOS
let distancePredicate (aisMsg : MessageType123) =
    
    // Distance in KM from Tromsø we want ships to appear
    let radiusFromTOS = 50.

    // The point of interest
    let (latTOS, lngTOS) = (69.6811428, 18.9314564)
    let (lat, lng) = (aisMsg.Latitude, aisMsg.Longitude)

    Utils.calculateDistance ((latTOS, lngTOS), (lat,  lng)) < radiusFromTOS

let query (connectionId: ConnectionId) (_: IAsyncObservable<Msg*ConnectionId>) : IAsyncObservable<Msg*ConnectionId> =
    ofAis()
    |> filter distancePredicate
    |> map ais2Msg
    |> map (fun msg -> (msg, connectionId))

let configureApp (app : IApplicationBuilder) =
    app.UseWebSockets()
       .UseReaction<Msg>(fun options ->
       { options with
           Query = query
           Encode = Msg.Encode
           Decode = Msg.Decode
       })
       .UseDefaultFiles()
       .UseStaticFiles()
       .UseGiraffe webApp

let configureServices (services : IServiceCollection) =
    services.AddGiraffe() |> ignore
    let fableJsonSettings = Newtonsoft.Json.JsonSerializerSettings()
    fableJsonSettings.Converters.Add(Fable.JsonConverter())
    services.AddSingleton<IJsonSerializer>(NewtonsoftJsonSerializer fableJsonSettings) |> ignore

let configureLogging (builder : ILoggingBuilder) =
    builder.SetMinimumLevel(LogLevel.Information)
    |> ignore

WebHost
    .CreateDefaultBuilder()
    .UseWebRoot(publicPath)
    .UseContentRoot(publicPath)
    .Configure(Action<IApplicationBuilder> configureApp)
    .ConfigureServices(configureServices)
    .UseUrls("http://0.0.0.0:" + port.ToString() + "/")
    .ConfigureLogging(configureLogging)
    .Build()
    .Run()