
#include <math.h>
#include <ignImageAggChannels.h>

#include <ignImageComposeChannelsOp.h>
#include <ignImageRadioTransform.h>

namespace ign {
    namespace image {

        BufferImage<float>* aggregateCannels0(BufferImage<float> &modDer){

            BufferImage<float> *aggregatedModDer = new BufferImage<float>();
            aggregatedModDer->initialize(modDer.numCols(), modDer.numLines(), 1);
            for (size_t j = 0; j < aggregatedModDer->numCols(); j++) {
                for (size_t i = 0; i < aggregatedModDer->numLines(); i++){

                    aggregatedModDer->set(j, i, 0, modDer(j, i ,0) );
                }
            }
            aggregatedModDer->close();

            return aggregatedModDer;

        }


        BufferImage<float>* aggregateCannelsSum(BufferImage<float> &modDer){

            BufferImage<float> *test = new BufferImage<float>();
            ign::transform::Transform *transform = new ign::image::SumComposeChannelTransform<float, float>();

            ign::image::ComposeChannelOp<float, float>::Compute(modDer, *transform, *test);

            return test;
        }


        BufferImage<float>* aggregateCannelsSumPerNumBand(BufferImage<float> &modDer){

            BufferImage<float> *test = new BufferImage<float>();
            ign::transform::Transform *transform = new ign::image::SumPerNumBandComposeChannelTransform<float, float>();

            ign::image::ComposeChannelOp<float, float>::Compute(modDer, *transform, *test);

            return test;
        }



        BufferImage<float>* aggregateCannelsSumSquare(BufferImage<float> &modDer){

            BufferImage<float> *test = new BufferImage<float>();
            ign::transform::Transform *transform = new ign::image::EuclidianNormComposeChannelTransform<float, float>();

            ign::image::ComposeChannelOp<float, float>::Compute(modDer, *transform, *test);

            return test;
        }


        BufferImage<float>* aggregateCannelsMax(BufferImage<float> &modDer){

            BufferImage<float> *test = new BufferImage<float>();
            ign::transform::Transform *transform = new ign::image::MaximumComposeChannelTransform<float, float>();

            ign::image::ComposeChannelOp<float, float>::Compute(modDer, *transform, *test);

            return test;
        }

    }
}
