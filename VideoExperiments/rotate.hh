//
//  rotate
//  VideoExperiments
//
//  Created by david on 7/28/13.
//  Copyright (c) 2013 David Whetstone All rights reserved.
//


#ifndef __ROTATE_HH__
#define __ROTATE_HH__

#include "types.hh"


ByteVectorPtr rotateYUV420spNV21(ByteVector const & input, int inputWidth, int inputHeight);
void rotateYUV420spNV21(ByteVector const & input, int inputWidth, int inputHeight, ByteVector & output);


#endif //__ROTATE_HH__
