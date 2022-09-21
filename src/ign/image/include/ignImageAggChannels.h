#ifndef __AGGREGATE_CHANNELS_H__
#define __AGGREGATE_CHANNELS_H__

#include <ignBufferImage.h>

namespace ign {
    namespace image {

        BufferImage<float>* aggregateCannels0(BufferImage<float> &modDer);
        BufferImage<float>* aggregateCannelsSum(BufferImage<float> &modDer);
        BufferImage<float>* aggregateCannelsSumPerNumBand(BufferImage<float> &modDer);
        BufferImage<float>* aggregateCannelsSumSquare(BufferImage<float> &modDer);
        BufferImage<float>* aggregateCannelsMax(BufferImage<float> &modDer);

    }
}

#endif //__AGGREGATE_CHANNELS_H__
