# PlayStation 2 Development Journals

Here is my (mis)adventures of PS2 development! Writing down whatever I can discover! :D

## Docker container deployment
For the start you need to get Docker, and [pull that container](https://hub.docker.com/r/nyhworkshop/ps2dev) containing the latest PS2SDK (and the other accessories) into your folder.

Then create a workspace folder (for example, E:/ps2dev/workspace for your container, and launch it at the `cmd`:
```
docker run -it --mount type=bind,src=E:/ps2dev/workspace,dst=/workspace nyhworkshop/ps2dev
```

## PS2 Emulator
Of course you need to have the PS2 emulator to test the compiled binaries! Get the [PCSX2](https://pcsx2.net/).

## Documentations and youtube tutorials
Main PS2 related documentations can be obtained from [https://github.com/DarrenRainey/PS2-Programming-Docs](there). Be sure to get the *GS Users Manual* and *EE Users Manual*. Also, be warned that the documentations are *not easy to follow* and you need other supporting materials available online!

Since the PS2 is a more complex gaming console compared to the other older architectures, the primary focus is not much on writing generic 2D apps (which this console can happily run on it without breaking a sweat!), but more towards drawing some 3D stuff, which is what made the console very famous at the first place.

### Tutorials (frequently updated)
- [A struggle to draw a triangle!](https://www.youtube.com/watch?v=1RYoi7yh8iU) Tom Marks had to spend a few hours squeezing a triangle out, and this is an effort I have to thank him on! Here's the `helloTriangle` which demonstrates this but using a `packet2` instead of `packet`. It's a little red triangle on the screen! Maybe if I combine a few more, and more, it'll become another bigger thing? :D

### Documents and websites of interest
- [Mirrored page](http://lukasz.dk/mirror/hsfortuna/) of Dr. HS Fortuna's PS2 development.
- [Mike Kohn's page on drawing a rotating triangle and other things.](https://www.mikekohn.net/software/playstation2.php)
- An [attempt to put Quake 2 inside the console](https://glampert.com/2015/02-27/ps2-homebrew-setting-up-the-environment/). Some tutorial on deploying the PS2SDK. Also his another [introductory article](https://glampert.com/2015/03-23/ps2-homebrew-hardware-and-ps2dev-sdk/).
- Some [introduction on PS2 programming](http://lukasz.dk/playstation-2-programming/an-introduction-to-ps2dev/).

## Something to keep an eye on!
- Arranging and sending the packets to the GS is a tricky affair! I spent a lot of time trying to do this! And yes, you **need** to use them a lot if you intend to draw 3D things on the screen! Please start with a `helloTriangle` to see how to send one triangle to the screen.
- A `qword` is 128-bits long. Vital component in a packet!
