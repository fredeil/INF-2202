
# INF-2202 - fei003 - assignment 3

Reactive async programming with streams.

## **Background**

From the assignment:

> The task is to build a web service that streams live AIS data from the Norwegian Costal Administration (Kystverket). We will use reactive programming to augment the AIS data with computed insights and make these insights avail

### Data source

Automatic identification system (AIS) is a system established by the International Maritime Organization (IMO) to increase the safety for ships (anti collision) and traffic control (search and rescue).

AIS consists of three types of information:

* Dynamic (position, course, speed)
* Static (identity, vessel type, hull dimensions)
* Voyage details (destination, ETA, carriage etc.)

The AIS messages from Kystverket has a signal coverage of 40-60 nautical miles from the coastal line of Norway, and they are using the high-end of the VHF-FM band (162 Mhz) for transmitting the data between ship/shore.

The format of an AIS stream (AIVDM/AIVDO protocol) is hard to interpret manually (a typical AIVDM packet):

```bash
!AIVDM,1,1,,A,177KQJ5000G?tO`K>RA1wUbN0TKH,0*5C
```

Without going much into the [details](http://catb.org/gpsd/AIVDM.html), this is a two-layer protocol wrapped inside the NMEA 0183 protocol, whic is the de-facto standard for marine navigational systems.

Splitting the data on "," we have:

* `!AIVDM` The NMEA message type (AIVDM - from other ships / AIVDO - from your ship)
* `1` is the number of sentences (can be multple)
* `1` is the sentence number (can be multiple),
* `blank` is the sequential message id
* `A` is the AIS channel (A/B)
* `177KQ..` is the encoded AIS data
* `0*` is the end of the data
* `5c*` is the NMEA checksum

Happily for us, all of this is handled inside the [NaisParser](https://github.com/dbrattli/NAisParser) library made by Dag Brattli. NaisParser decodes all the incoming AIS messages from the stream and converts them to workable in-memory objects for us to use.

### Web client

~~The web client is a mess, we are using the SAFE stack (but not really), and we are using React mixed with a Elmish architecture which sort of breaks Facebooks (Reacts) paradigms, and then mixing this with Fable for creating JavaScript code out of F# code (with half arsed bindings between the JS/F# world), this kind of development is also so niche, so in the case when you get stuck, you have no way out.~~

The web client is built on top of the SAFE stack (much like LAMP, WAMP, etc. in the good old days), which is a functional-first (prescriptive) stack for creating web apps. The components are:

* Saturn (server-side framework)
* Azure (hosting, could be any cloud provider)
* Fable (for running F# in the browser)
* Elmish (client-side architecture)

Since this is only a prescriptive stack we can change out any piece we want. Our solution is made up of ASP.NET Core (with Giraffe as the middleware for making the ASP.NET Core pipeline more functional), as the server side framework, Fable for transpiling F# to JavaScript and Elmish that defines the Model-View-Update (MVU) architecture used on the client side UI code.

The glue between our server and client is the Reaction library which is an asynchronous and reactive library for creating push based event streams (event streams become first-class objects). Reaction lets us use the same code on the server and the client. Our server is reponsible for providing these push based events (AIS messages) and the web client subscribes to these events (via websockets) so we can display the ships on a map.

## **Implementation**

The code is scaffolded with a dotnet template (SAFE.Template with Giraffe as a parameter), this gives us all bits and pieces of a SAFE based project to work with. Project dependencies are handled with npm / yarn for within the JavaScript ecosystem, and Paket / NuGet for the .NET ecosystem. The mix of these ecosystems generates alot of noise in the project structure, the interesting pieces are inside Client/Client.fs, Server/Server.fs and Shared/Shared.fs.

The server is responsible for opening a TCP connection to the live AIS stream from Kystverket, it filters out all AIS messages that are outside a 50km radius of Tromsø and then emits the data with an AsyncObservable by using websockets. The code for calculating the distance between two spherical points was kindly provided by [John D. Cook](https://www.johndcook.com/blog/fsharp_longitude_latitude/).

When a client is opened in a web browser it subscribes to the stream of data created by the server, and then decodes it. This provides us with a message in the update function that we can use to do calculations for the speed and the heading of each ship coming from the stream. On each update we add the ship in a map with the ships MMSI as the key value, the next time the same MMSI number comes in the update function we can do the calculations.

## Conclusion

The application works as the assignment requested, fun little project. The contest part could have been mentioned earlier.
