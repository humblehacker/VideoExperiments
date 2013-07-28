//
//  rotate.cc
//  VideoExperiments
//
//  Created by david on 7/28/13.
//  Copyright (c) 2013 David Whetstone All rights reserved.
//

#include "rotate.hh"

void rotateYUV420spNV21(ByteVector const & input, int inputWidth, int inputHeight, ByteVector& output);
void rotateMatrix(ByteVector::iterator outputIterator, ByteVector::const_iterator &inputIterator, int inputWidth, int inputHeight, int itemSize);

ByteVectorPtr rotateYUV420spNV21(ByteVector const & input, int inputWidth, int inputHeight)
{
    ByteVectorPtr output = ByteVectorPtr(new ByteVector(input.size()));

    rotateYUV420spNV21(input, inputWidth, inputHeight, *output);

    return output;
}

void rotateYUV420spNV21(ByteVector const & input, int inputWidth, int inputHeight, ByteVector & output)
{
    ByteVector::iterator outputIterator = output.begin();
    ByteVector::const_iterator inputIterator = input.begin();

    // Rotate Y values
    rotateMatrix(outputIterator, inputIterator, inputWidth, inputHeight, 1);

    // Rotate VU values
    const int offset = inputWidth * inputHeight;
    inputIterator  = input.begin() + offset;
    outputIterator = output.begin() + offset;

    rotateMatrix(outputIterator, inputIterator, inputWidth / 2, inputHeight / 2, 2);
}

void rotateMatrix(ByteVector::iterator outputIterator, ByteVector::const_iterator &inputIterator, int inputWidth, int inputHeight, int itemSize)
{
    int const outputWidth = inputHeight;

    for (int inputRow = 0; inputRow < inputHeight; ++inputRow)
    {
        for (int inputCol = 0; inputCol < inputWidth; ++inputCol)
        {
            int const outputCol = inputRow;
            int const outputRow = inputWidth - inputCol - 1;
            int outputIndex = itemSize * (outputCol + outputWidth * outputRow);
            int inputIndex = itemSize * (inputCol + inputWidth * inputRow);

            for (int i = 0; i < itemSize; ++i)
            {
                *(outputIterator+outputIndex) = *(inputIterator+inputIndex);
                ++outputIndex;
                ++inputIndex;
            }
        }
    }
}

