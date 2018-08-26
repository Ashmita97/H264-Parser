# H264-Parser

WHAT IS H.264?
H.264 is a video compression technology, or codec.
The terms H.264 and AVC are interchangeable. 
As a video codec, H.264 can be incorporated into multiple container formats, such as .MP4,.MOV .F4V, .3GP, .ts.
 
THE H.264 BITSTREAM
An h.264 bitstream contains a sequence of Network Abstraction Layer (NAL) units.
The coded video data is organized into NAL units, each of which is effectively a packet that contains an integer number of bytes.
The SPS and PPS are both types of NAL units.
 
WHAT IS A PARSER ?
In computer technology, a parser is a program, that receives input and breaks it down into simplified parts.
 
GOAL :
To provide a complete set of functions to parse video bitstreams conforming to the ITU-T H.264 | ISO/IEC 14496-10 AVC video standard.
 
ADVANTAGES:
Though a significant number of parsers are available online, this parser stands out as it is flexible enough to be implemented in numerous ways.
 
This parser consists of multiple classes:
·         The Bitstream class : Reads data from any binary bitstream.                            
                                   Can extract specific bits from any available bitstream.
·        The VideoParser class : Possesses the function of parsing data common to all video codecs.
·        The H264 class : Is specifically useful to parse data present in H.264 encoded videos.
 
Though each of these classes have definite functions, they carry out fundamental operations and thus can easily be adapted to be used in other programs too, if the need arises.
This parser does not function as an encoder or a decoder, however it is versatile enough to comprise portions that can be modified and utilised while writing an encoder or decoder.
 
