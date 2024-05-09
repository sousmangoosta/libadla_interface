#include <cmath>
#include "nn_sdk.h"
#include "postprocess_util.h"

float logistic_activate(float x)
{
    return (float)1./(float)(1. + std::exp(-x));
}

void postprocess_ssd(nn_output *pout, unsigned int *detNum, dBox *pBox)
{
    float *ssd_buffer[3] = {nullptr};
    ssd_buffer[0] = (float*)pout->out[0].buf;
    ssd_buffer[1] = (float*)pout->out[1].buf;
    ssd_buffer[2] = (float*)pout->out[2].buf;

    struct Anchor {
        double y;
        double x;
        double h;
        double w;
    }anchor{};
    struct BoxEncodings {
        double y;
        double x;
        double h;
        double w;
    }boxEncodings{};
    struct Box {
        float ymin;
        float xmin;
        float ymax;
        float xmax;
    }box{};
    int num_detections = 0;
    for (int i = 0; i < 2034; i++) {
        double y_scale = 10.0;
        double x_scale = 10.0;
        double h_scale = 5.0;
        double w_scale = 5.0;
        boxEncodings.y = (double)ssd_buffer[1][i*4+0];
        boxEncodings.x = (double)ssd_buffer[1][i*4+1];
        boxEncodings.h = (double)ssd_buffer[1][i*4+2];
        boxEncodings.w = (double)ssd_buffer[1][i*4+3];
        anchor.y = (double)ssd_buffer[2][i*4+0];
        anchor.x = (double)ssd_buffer[2][i*4+1];
        anchor.h = (double)ssd_buffer[2][i*4+2];
        anchor.w = (double)ssd_buffer[2][i*4+3];
        auto ycenter = (float)((double)boxEncodings.y / (double)y_scale * (double)anchor.h + (double)anchor.y);
        auto xcenter = (float)((double)boxEncodings.x / (double)x_scale * (double)anchor.w + (double)anchor.x);
        auto half_h = (float)(0.5*(exp((double)boxEncodings.h/ (double)h_scale)) * (double)anchor.h);
        auto half_w = (float)(0.5*(exp((double)boxEncodings.w / (double)w_scale)) * (double)anchor.w);
        box.ymin = ycenter - half_h;
        box.xmin = xcenter - half_w;
        box.ymax = ycenter + half_h;
        box.xmax = xcenter + half_w;
        for (int j = 0; j < 91; j++) {
            float prob = logistic_activate((float) ssd_buffer[0][i*91+j]);
            if (prob > 0.600000023842) {
                //printf("%d : %f %f %f %f\n", i, box.ymin, box.xmin, box.ymax, box.xmax);
                pBox[num_detections].ymin = box.ymin;
                pBox[num_detections].xmin = box.xmin;
                pBox[num_detections].ymax = box.ymax;
                pBox[num_detections].xmax = box.xmax;
                pBox[num_detections].score = prob;
                pBox[num_detections].objectClass = (float)j-1;
                num_detections++;
            }
        }
    }
    *detNum = num_detections;
    //printf("found %d elements\n", num_detections);
}
