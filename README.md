Kowalski
========

What is this?
-------------

Kowalski consists of two main components, both written in C for maximum portability:
* A data driven, light weight audio engine.
* Tools to build, validate and view data for the Kowalski engine.

The primary audience is developers of games and other
interactive applications where audio plays an important role.

**Please note that Kowalski is still in development.** 

Why another audio engine?
-------------------------
There are plenty of audio engines around, but Kowalski 
is the only option that is

* Highly portable
* Well documented
* Released under a permissive license
* Data driven, using well defined and documented XML and binary formats

External dependencies have been kept to a minimum; the Kowalski engine only relies
on platform specific libraries to pass buffers to the audio hardware and the Kowalski tools
only depend on libxml2.

Features
--------
Key features include

* Positional audio in the form of distance attenuation, directional cone attenuation, doppler shift and positional panning.
* Mix buses allowing better control over complex mixes.
* Mix presets for easy switching between different predefined mixes.
* Real time decoding of IMA ADPCM (IMA4) and Ogg Vorbis streams and support for using said streams as positional audio sources.
* Dynamic playback rate (i.e pitch) control for both uncompressed and compressed audio.
* Audio level metering making it possible to keep track of the output levels and detect clipping.
* 32 bit floating point internal processing.
* An approximate sample clock API making it possible to synchronize audio and for example visuals with a granularity dependent on the buffer size.
* Psychoacoustically correct gain control designed to give a linear change in perceived loudness for linearly varying gain values.
* iOS only: Playback of MP3 and AAC audio files using the hardware decoder.

The Kowalski engine is not designed to handle

* Complex DSP networks and audio routing.
* Decoding a wide range of audio formats.
* Playback of MIDI files.
* Playback of tracker files (.MOD etc).


