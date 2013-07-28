//
//  types.hh
//  VideoExperiments
//
//  Created by David Whetstone on 7/27/13.
//  Copyright (c) 2013 David Whetstone. All rights reserved.
//

#ifndef __TYPES_HH__
#define __TYPES_HH__

#include <memory>
#include <vector>

typedef uint8_t byte;
typedef std::vector<byte> ByteVector;
typedef std::unique_ptr<ByteVector> ByteVectorPtr;

struct RGBA
{
    byte red;
    byte green;
    byte blue;
    byte alpha;
};

typedef std::vector<RGBA> RGBAVector;
typedef std::unique_ptr<RGBAVector> RGBAVectorPtr;

#endif // __TYPES_HH__
