# INF-2202 Assignment #3: Reactive Programming

This assignment will introduce you to the very cutting edge of modern cross platform full stack software development. We will use some very interesting tech and libraries to filter, transform, group, aggregate and merge live streams of data.

## Tasks

The task is to build a web service that streams live [AIS](https://en.wikipedia.org/wiki/Automatic_identification_system) data from the Norwegian Costal Administration (Kystverket). We will use reactive programming to augment the AIS data with computed insights and make these insights available to be displayed in real-time for any connected web clients (browsers).

In addition:

1. Server side Geo filtering, e.g only display ships that are within the area (box) of Troms County. The AIS data rate can be quite high, so to reduce the load we want to filter the date stream.

2. Calculate the speed and heading of each ship. Although the data might already be available in some of the AIS frames, we want to double check that the data is correct and so we have to calculate the values ourselves. This calculation should be done on the client side to reduce the load on the server.

3. The total number of ships should at all times be displayed as a counter on the web page.

4. (Optional) Implement a collision detection mechanism that will warn (e.g toastr) if two ships gets too close. Ships don't usually collide that often, so make the distance configurable e.g 1000 meters or more.

5. (Optional) Can you create the Geo fence in (1) by selecting an area on the map?

## Requirements

The solution must be coded in F#, not only for the server side, but also for the the client side.

- Server Side: The service should be a .NET Core application running ASP.NET Core with an F# web framework such as [Giraffe](https://github.com/giraffe-fsharp/Giraffe). For reactive programming you need to use the [Reaction](https://github.com/dbrattli/Reaction) library.

- Client Side: The web client must be written using [Elmish](https://elmish.github.io/). To use F# client side you need to make use of the [Fable](http://fable.io/) F# to JavaScript transpiler. For reactive programming you need to use [Fable.Reaction](https://github.com/dbrattli/Fable.Reaction) library. The map needs to be generated using Leaflet or Google Maps. Both are available for Fable/Elmish using [Fable.React](https://github.com/fable-compiler/fable-react).

- The task must be solved using Reactive Programming and Async Observables, for both the client and the server side. You will need to make the AIS stream available as an Async Observable, and do your analytics on the stream. Thus "normal" imperative code will not be accepted for calculating the insights.

- The tooling is cross platform so you may use any platform and browser you like. Linux, Mac, or Windows. For the brave it's even possible to run the server side without ASP.NET Core using Fable and Node.js, but then you won't get the Reaction Websocket stuff for free.

## Hints and Pre-Code

- Take a close look at the [WebSocket example](https://github.com/dbrattli/Fable.Reaction/tree/master/examples/Websocket) in the examples folder of Fable.Reaction. This is the basic scaffolding you will need for your application.

- Also look at this example using [NAisParser](https://github.com/dbrattli/NAisParser/tree/master/Examples/NcaFeed) on how you can parse the AIS data asynchronously using F#. You will need to wrap this as an Async Observable.

- Reaction is an implementation of Async Observables. Observables are well documented at [IntroToRx](http://www.introtorx.com/) and [ReactiveX](http://reactivex.io/). There are also many books available on Rx.

- An example using [Fable.ReactLeaflet](https://github.com/fable-compiler/fable-react/tree/master/src/Fable.ReactLeaflet) is provided in the Fable.Reaction [Leaflet](https://github.com/dbrattli/Fable.Reaction/tree/master/examples/Leaflet) example.

- It's always a good idea to be at the lectures that will be given on the topic.

## Pre-Requisites

You'll need to install the following pre-requisites in order to build SAFE applications. SAFE in the context of this assingment means the combination .NET Core, Giraffe, Fable as well as the Elmish tools and libraries.

- The [.NET Core](https://www.microsoft.com/net/download) SDK 2.1.
- [FAKE 5](https://fake.build/) installed as a [global tool](https://fake.build/fake-gettingstarted.html#Install-FAKE)
- The [Yarn](https://yarnpkg.com/lang/en/docs/install/) package manager.
- [Node 8.x](https://nodejs.org/en/download/) installed for the front end components.
- If you're running on OSX or Linux, you'll also need to install [Mono](https://www.mono-project.com/docs/getting-started/install/).

All of the pre-requisites are installed on the lab computers with the exception of FAKE 5, which you can install yourselves.

### Install an F# code editor

You'll also want an IDE to create F# applications. We recommend one of the following great IDEs.

- [VS Code](https://code.visualstudio.com/) + [Ionide](https://github.com/ionide/ionide-vscode-fsharp) extension (recommended)
- [Visual Studio 2017](https://www.visualstudio.com/downloads/)
- [Jetbrains Rider](https://www.jetbrains.com/rider/)

## References

- [The Norwegian Coastal Administration AIS feed](http://www.kystverket.no/Maritime-tjenester/Meldings--og-informasjonstjenester/AIS/Brukartilgang-til-AIS-Norge/) - The data source for this assignment
- [SAFE stack](https://safe-stack.github.io/) - Make sure you read the [intro](https://safe-stack.github.io/docs/intro/) and the [SAFE stack overview](https://safe-stack.github.io/docs/overview/)
- [Elmish](https://elmish.github.io/) - Fable applications following "model view update" architecture.
- [Fable](http://fable.io/) - The compiler that emits JavaScript you can be proud of.
- [Fable.React](https://github.com/fable-compiler/fable-react) - Fable bindings and helpers for React and React Native
- [React](https://reactjs.org/) - A JavaScript library for building user interfaces.
- [Fable.Reaction](https://github.com/dbrattli/Fable.Reaction) - Reactive (Rx) Elmish for F# and Fable .
- [Reaction](https://github.com/dbrattli/Reaction) - Async Reactive (Rx) for F# and Fable.
- [Giraffe](https://github.com/giraffe-fsharp/Giraffe) - A native functional ASP.NET Core web framework for F# developers.
- [NaisParser](https://github.com/dbrattli/NAisParser) - A native functional ASP.NET Core web framework for F# developers.
- [Paket](https://fsprojects.github.io/Paket/) - Dependency manager for .NET and mono projects,
- [F# for fun and profit](https://fsharpforfunandprofit.com/) - Do you want to understand what all the fuss about functional programming is about?
- [Yarn](https://yarnpkg.com/lang/en/) - Yarn is a package manager for your JS code.
- [Leaflet](https://leafletjs.com/) and [React Leaflet](https://react-leaflet.js.org/)- JavaScript library and React components for interactive maps.
