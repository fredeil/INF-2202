module Client
open Elmish
open Elmish.React
open Fable.Core.JsInterop
open Fable.Helpers.React
open Fable.Helpers.React.Props
open Fable.Reaction
open Fable.Reaction.WebSocket
open Reaction
open Shared
open Fulma
open ReactLeaflet

type Model = { Ships : Map<Mmsi, Ship> }
let init () : Model = { Ships = Map.empty }

let update (msg : Msg) (currentModel : Model) : Model =

    let oldShipState = msg.Mmsi |> currentModel.Ships.TryFind

    let heading = 
        match oldShipState with 
        | Some ship -> ((ship.Latitude, ship.Longitude), (msg.Latitude, msg.Longitude)) 
                    |> Utils.bearing
                    |> string
        | _ -> "N/A"

    let speed = 
        match oldShipState with
        | Some ship -> ((ship.Latitude, ship.Longitude), (msg.Latitude, msg.Longitude))
                    |> Utils.calculateDistance
                    |> fun y -> y * 1000. // KM to M
                    |> fun y -> y / (msg.Time - ship.Time).TotalSeconds // m/s
                    |> fun y -> y * 1.94384 // knots
                    |> fun y -> System.Math.Round (y, 2)
        | _ -> 0.

    let newShipState =  { 
        Latitude = msg.Latitude; 
        Longitude = msg.Longitude; 
        Speed = speed; 
        Heading = heading;
        Time = System.DateTime.UtcNow;
    }

    match msg with
    | _ ->
        { currentModel with Ships = currentModel.Ships.Add (msg.Mmsi, newShipState)}

let view model dispatch =

    let markers = [ for KeyValue(mmsi, ship) in model.Ships 
        -> marker [ 
                    MarkerProps.Position !^ (ship.Latitude, ship.Longitude)
                    MarkerProps.Key (mmsi.ToString()) 
                    ] [ 
                        popup [] [ 
                            str ( "MMSI: " + mmsi.ToString() + " ") 
                            br []
                            str ( "SOG: " + ship.Speed.ToString() + " kt" )
                            br []
                            str ( "HDG: " + ship.Heading.ToString() )
                            ] 
                        ] 
                    ]

    div [] [
        Navbar.navbar [ Navbar.Color IsPrimary ] [
            Navbar.Item.div [] [
                Heading.h2 [] [
                    str ("Showing " + model.Ships.Count.ToString()  + " ships")
                ]
            ]
        ]
        ReactLeaflet.map [
            MapProps.Center !^ (69.6811428, 18.9314564)
            MapProps.SetView true
            MapProps.Zoom (float 8.8)
            MapProps.ZoomSnap 0.1
            MapProps.Id "myMap"
            MapProps.Style [ CSSProp.Height "950px" ]
        ] [
            yield circle [
                CircleProps.Custom ("center", (69.6811428, 18.9314564))
                CircleProps.Color "red"
                CircleProps.Radius 50000.
                CircleProps.Fill false
            ] []

            yield tileLayer [
                TileLayerProps.Attribution "&copy; <a href=\"https://www.openstreetmap.org/copyright\">OpenStreetMap</a> contributors"
                TileLayerProps.Url "https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
            ] []

            yield! markers
          ]
        ]

let query (msgs: IAsyncObservable<Msg>) =
    msgs
    |> msgChannel "ws://localhost:8085/ws" Msg.Encode Msg.Decode
    |> AsyncObservable.delay 1000


#if DEBUG
open Elmish.Debug
open Elmish.HMR
#endif

Program.mkSimple init update view
|> Program.withQuery query
#if DEBUG
|> Program.withConsoleTrace
|> Program.withHMR
#endif
|> Program.withReact "elmish-app"
#if DEBUG
|> Program.withDebugger
#endif
|> Program.run
