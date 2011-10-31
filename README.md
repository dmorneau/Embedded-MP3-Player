Embedded MP3 downloader/decoder
===============================

This code implements an MP3 decoder that runs on a single soft
processor (Xilinx MicroBlaze). The MP3 decoder fetches music files
from a server on the network via a simple custom protocol, and plays
back the music in real-time, while responding to user input to control
the volume. In order to do this we use queues, circular buffers, and
tasks coordinated by an embedded real-time OS (Xilkernel).

On the hardware side, some I/O device is required. We used a custom
peripheral. The VHDL code is included.

I wrote the following modules: audiobuffer, downloader, main,
mp3decoder, scaling, and queues, and helped implement the C layer for
the interface module via bitfields. I'm especially happy with the
audio buffer performance; feel free to take inspiration from or reuse
the code. Things that I wrote are under the beerware license.

The general architecture is drawn in architecture.png (please forgive
the French). DIO4_AFFICH is the custom I/O module.