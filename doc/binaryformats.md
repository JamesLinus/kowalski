Project XML Data
===

Described by XSD schema and a number of additional constraints.

TODO



Binary File Structure
===

There are two kinds of Kowalski binary data: engine data and wave bank data. Engine
data binaries contain non-audio data such as event definitions and sound definitions
whereas wave bank binaries contain actual audio waveforms and are typically much larger
than engine data files. The different data types allowed to occur in these binary
files are given in the table below. 

Data types
----

* ``int32`` - An unsiged 32 bit integer. Big endian byte order.
* ``float32`` - A 32 bit IEEE floating point number. Big endian byte order.
* ``unsigned char[i]`` - A sequence of i unsigned chars.
* ``string`` - An ``int32`` followed by that many unsigned chars representing the ASCII characters of the string. 

Both wave bank and engine data files begin with a file identifier byte sequence. 
This sequence is designed to catch a number of common file transfer errors and was 
inspired by (or stolen from, actually) the <a href="http://jcp.org/en/jsr/detail?id=184">M3G</a> binary format. The anatomy of the identifiers is detailed in the following table.

---

* **byte 1**
  * ``0xAB``, Chosen to be a non-ASCII value catch attempts to read text files. Also catches bad file transfers that clear bit 7.
* **bytes 2-4**
  * A human readable format description. ``K`` ``W`` ``L`` for engine data and ``K`` ``W`` ``B`` for wave banks.
* **byte 5**
  * ``0xBB``
* **bytes 6-7**
  * ``0x0D`` ``0x0A``; A CR-LF sequence which catches bad file transfers that alter newline sequences.
* **byte 8**
  * ``0x1A``, a control-Z character, which stops file display under MS-DOS, and
          further reduces the risk that a text file will be falsely recognised.
* **byte 9**
  * ``0x0A``, a final line feed, which checks for the inverse of the CR-LF translation problem.

---

The Engine Data Binary Format
----
A Kowalski engine data file starts with a file identifier followed by
five chunks, each beginning with an ``int32`` ID
and an ``int32`` specifying the following number of bytes in the chunk. The chunk IDs are magic
numbers chosen so that the bytes (read from high to low) spell out a human readable description.
For example, the event definitions chunk ID is ``0x73747665`` or equivalently ``'evts'``. The structure of an engine data binary file is defined in the following table (the chunks always occur in this order).

---

* **File identifier**
  * ``unsigned char[9]`` : ``0xAB``, ``K``, ``W``, ``L``, ``0xBB``, ``0x0D``, ``0x0A``, ``0x1A``, ``0x0A``
* **Wave Bank Chunk**
  * ``int32``: Chunk ID (``0x736b6277``, i.e ``'wbks'``)  
  * ``int32``: Chunk size in bytes
  * ``int32``: Total number of audio data entries.
  * ``int32``: The number of wave banks
    * ``string``: ID of wave bank _i_
    * ``int32``: The number of audio data entries in wave bank _i_
      * ``string``: File name of entry _j_ in wave bank _i_* 
* **Mix Bus Chunk**
  * ``int32``: Chunk ID (``0x7362786d``, i.e ``'mxbs'``)  
  * ``int32``: Chunk size in bytes
  * ``int32``: Total number of mix buses.
    * ``string``: ID of mix bus _i_
    * ``int32``: The number of sub buses of mix bus _i_
      * ``int32``: The index of sub bus _j_ of mix bus _i_
* **Mix Preset Chunk**
  * ``int32``: Chunk ID (``0x7270786d``, i.e ``'mxpr'``)  
  * ``int32``: Chunk size in bytes
  * ``int32``: Total number of mix presets.
    * ``string``: ID of mix preset _i_
    * ``int32``: Default flag (0 or 1) for mix preset _i_
      * ``int32``: Mix bus index of mix preset _i_
      * ``float32``: Left gain of mix preset _i_
      * ``float32``: Right gain of mix preset _i_
      * ``float32``: Pitch of mix preset _i_
* **Sound Chunk**
  * ``int32``: Chunk ID (``0x73646e73``, i.e ``'snds'``)  
  * ``int32``: Chunk size in bytes
  * ``int32``: Total number of sound definitions.
    * ``int32``: Playback count of sound definition _i_
    * ``int32``: Defer stop flag of sound definition _i_
    * ``float32``: Gain of sound definition _i_
    * ``float32``: Gain variation of sound definition _i_
    * ``float32``: Pitch of sound definition _i_
    * ``float32``: Pitch of sound definition _i_
    * ``int32``: Playback mode of sound definition _i_
    * ``int32``: The number of audio data items of sound definition _i_
      * ``int32``: The wave bank index of audio data item _j_ of sound definition _i_
      * ``int32``: The audio data index (into the wave bank) of audio data item _j_ of sound definition _i_
* **Event Chunk**
  * ``int32``: Chunk ID (``0x73747665``, i.e ``'evts'``)  
  * ``int32``: Chunk size in bytes
  * ``int32``: Total number of event definitions.
    * ``string``: ID of event definition _i_
    * ``int32``: Instance count of event definition _i_
    * ``float32``: Gain of event definition _i_
    * ``float32``: Pitch of event definition _i_
    * ``float32``: Inner cone angle of event definition _i_
    * ``float32``: Inner cone gain of event definition _i_
    * ``float32``: Outer cone angle of event definition _i_
    * ``float32``: Outer cone gain of event definition _i_
    * ``int32``: Mix bus index of event definition _i_
    * ``int32``: Positional flag (0 or 1) of event definition _i_
    * ``int32``: Sound definition index of event definition _i_ (non-streaming events only)
    * ``int32``: Retrigger mode of event definition _i_ (non-streaming events only)
 	* ``int32``: Wave bank index of event definition _i_ (streaming events only)
 	* ``int32``: Audio data item index (into the wave bank) of event definition _i_ (streaming events only)
 	* ``int32``: Loop flag of event definition _i_ (streaming events only)
 	* ``int32``: The number of wave banks referenced by event definition _i_ 
   	  * ``int32``: The index of referenced wave bank _j_ of event definition _i_ 

---

The Wave Bank Binary Format
---
---

* **File identifier**
  * ``unsigned char[9]`` : ``0xAB``, ``K``, ``W``, ``B``, ``0xBB``, ``0x0D``, ``0x0A``, ``0x1A``, ``0x0A``
* **Wave Bank Data**
  * ``string``: The ID of the wave bank
  * ``int32``: The number of audio data items.
    * ``string``: File name of entry _i_
    * ``int32``: Encoding of entry _i_
    * ``int32``: Stream flag of entry _i_
    * ``int32``: The number of channels of entry _i_ (non-zero for PCM, 0 otherwise)
    * ``int32``: The number, ``N``, of audio data bytes for entry _i_
	* ``unsigned char[N]``: Audio data. If the encoding is PCM, this is just the interleaved 16 bit samples. For non-PCM data, this is the entire audio file.
---