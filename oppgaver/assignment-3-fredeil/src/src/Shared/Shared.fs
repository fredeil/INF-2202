namespace Shared
open System
#if FABLE_COMPILER
open Thoth.Json
#else
open Thoth.Json.Net
#endif

type Msg = {
    Mmsi: int;
    Time: DateTime;
    Latitude: float;
    Longitude: float;
}

type Ship = {
    Latitude: float
    Longitude: float
    Speed: float
    Heading: string
    Time: DateTime // hack
}

type Mmsi = int

type Msg with
    static member Encode (msg: Msg) : string =
        let encoder = Encode.object [
              "mmsi", Encode.int msg.Mmsi
              "time", Encode.datetime msg.Time
              "lat", Encode.float msg.Latitude
              "lon", Encode.float msg.Longitude
            ]

        Encode.toString 4 encoder

    static member Decode (json: string) : Msg option =
        let decoder = Decode.object (fun get ->
                {
                  Mmsi = get.Required.Field "mmsi" Decode.int
                  Time = get.Required.Field "time" Decode.datetime
                  Latitude = get.Required.Field "lat" Decode.float
                  Longitude = get.Required.Field "lon" Decode.float
                }
            )

        let result = Decode.fromString decoder json
        match result with
        | Ok msg -> Some msg
        | Error _ -> None


module Utils =

    /// Converts degrees to radians
    let deg2rad (deg : float) : float =
            deg * System.Math.PI / 180.

    /// Converts radians to degrees
    let rad2deg (rad : float) : float =
            rad * 180. / System.Math.PI

    /// Calculates spherical distance between two points
    let calculateDistance (pos1, pos2) =
        
        let (lat1, lng1) = pos1
        let (lat2, lng2) = pos2

        let phi1 = (90. - lat1) |> deg2rad 
        let phi2 = (90. - lat2) |> deg2rad

        let theta1 = lng1 |> deg2rad
        let theta2 = lng2 |> deg2rad

        // Stolen from https://www.johndcook.com/blog/fsharp_longitude_latitude/
        // Compute spherical distance from spherical coordinates in KM
        6371.0 * acos(sin(float(phi1)) * sin(float(phi2)) *
            cos(float(theta1 - theta2)) + 
            cos(float(phi1)) * cos(float(phi2)))

    /// Calculates the bearing between two points (aka azimuth)
    let bearing (pos1, pos2) : float  =

        let (lat1, lng1) = pos1
        let (lat2, lng2) = pos2

        let y = sin(deg2rad lng2) - (deg2rad lng1) * cos(deg2rad lat2)
        let x =  cos(deg2rad lat1) * sin(deg2rad lat2) - sin(deg2rad lat1) 
                * cos(deg2rad lat2) * cos(deg2rad lng2 - deg2rad lng1)
         
        let theta = atan2 y x

        Math.Round ((rad2deg theta) + 360.0 % 360.0, 2)