# esp-libopus

This repository creates an ESP32 component which provides the original Opus code
for the ESP32. It includes the original opus library as a submodule.

### Usage
This repository can be added as submodule to any valid ESP32 component directory.
The easiest way to do this is to create a `component/` directory in the root
of an ESP project, using `git submodule add` to clone this repo into there,
and to ensure that this new component is recognized via `make list-components`.

After adding this component to the build configuration, the "opus.h" file
can be included. All *fixed point* Opus decoder and encoder methods are
available, consult the Opus documentation for examples.

### Demo
To build a demo application with the ESP32, simply:
- Clone this repository
- Open an ESP-IDF console
- `cd` to `./esp-libopus/example`
- Run `idf.py build`
- Flash the resulting code to your processor and observer.

#### Important considerations
Opus is a fairly heavy codec, and it might not always work smoothly on the ESP32.
The following things have to be taken into account:

- Encoding and Decoding heavily utilize Stack via `alloca`. It is recommended
to have at least 30kB of Stack reserved for the thread that is running the Opus
encoding/decoding (the stack is freed after the Opus call, so a single
thread can encode and decode).
- Decoding data on the ESP32 is fairly lightweight, even at 48kHz samplerate.
- *Encoding* however is more intensive. When the ESP32 is clocking at 240MHz, at
48kSps, a mono 16-bit fixed point data stream comprised of random data at
encoding complexity 1 will use 48% CPU.

#### Portability notes
*Note:* Fixed point is recommended for the ESP32.

