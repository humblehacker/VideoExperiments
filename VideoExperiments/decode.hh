//
//  decode
//  VideoExperiments
//
//  Created by david on 7/28/13.
//  Copyright (c) 2013 David Whetstone All rights reserved.
//


#include "types.hh"

#ifndef __DECODE_HH__
#define __DECODE_HH__

RGBAVectorPtr decodeYUV420spNV21ToRGBA_optimized(ByteVector const & input, int width, int height);
void decodeYUV420spNV21ToRGBA_optimized(ByteVector const & input, int width, int height, RGBAVector &rgb);

RGBAVectorPtr decodeYUV420spNV21ToRGBA(ByteVector const & input, int width, int height);
void decodeYUV420spNV21ToRGBA(ByteVector const & input, int width, int height, RGBAVector &rgb);

#endif // __DECODE_H__
