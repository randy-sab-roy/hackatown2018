#include "../arapaho/arapaho.hpp"
#include <string>
#include "opencv2/core/core.hpp"
#include <opencv2/imgproc.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "../arapaho/arapaho.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <chrono>

#define _ENABLE_OPENCV_SCALING
#define TARGET_SHOW_FPS (60)

using namespace cv;

static char INPUT_DATA_FILE[] = "input/input.data";
static char INPUT_CFG_FILE[] = "input/input.cfg";
static char INPUT_WEIGHTS_FILE[] = "input/input.weights";
static char INPUT_AV_FILE[] = "input/input.mp4";

int main()
{
    ArapahoV2 *darknet = new ArapahoV2();
    ArapahoV2Params params;

    params.datacfg = INPUT_DATA_FILE;
    params.cfgfile = INPUT_CFG_FILE;
    params.weightfile = INPUT_WEIGHTS_FILE;
    params.nms = 0.4;
    params.maxClasses = 4;

    int expectedWidth = 0;
    int expectedHeight = 0;

    bool isSuccessful = darknet->Setup(params, expectedWidth, expectedHeight);
    if (!isSuccessful)
    {
        return -1;
    }

    ArapahoV2ImageBuff imageBuffer;
    Mat image;

    namedWindow("Guardius", CV_WINDOW_AUTOSIZE);
    VideoCapture cap ( INPUT_AV_FILE );
    while (1)
    {
        bool success = cap.read(image);
        int imageWidthPixels = 0, imageHeightPixels = 0;

        if (!success || image.empty())
        {
            if (darknet)
                delete darknet;
            darknet = nullptr;
            waitKey();
            return -1;
        }

        imageWidthPixels = image.size().width;
        imageHeightPixels = image.size().height;

        // Remember the time
        auto detectionStartTime = std::chrono::system_clock::now();

        // Process the image
        imageBuffer.bgr = image.data;
        imageBuffer.w = imageWidthPixels;
        imageBuffer.h = imageHeightPixels;
        imageBuffer.channels = 3;

        int numObjects = 0;

        // Detect the objects in the image
        darknet->Detect(image, 0.24, 0.5, numObjects);

        std::chrono::duration<double> detectionTime = (std::chrono::system_clock::now() - detectionStartTime);

        if (numObjects > 0)
        {
            box* boxes = new box[numObjects];
            std::string* labels = new std::string[numObjects];

            // Get boxes and labels
            darknet->GetBoxes(boxes, labels, numObjects);

            int leftTopX = 0, leftTopY = 0, rightBotX = 0, rightBotY = 0;
            for (int objId = 0; objId < numObjects; objId++)
            {
                leftTopX = 1 + imageWidthPixels * (boxes[objId].x - boxes[objId].w / 2);
                leftTopY = 1 + imageHeightPixels * (boxes[objId].y - boxes[objId].h / 2);
                rightBotX = 1 + imageWidthPixels * (boxes[objId].x + boxes[objId].w / 2);
                rightBotY = 1 + imageHeightPixels * (boxes[objId].y + boxes[objId].h / 2);

                // Show image and overlay using OpenCV
                rectangle(image,
                          cvPoint(leftTopX, leftTopY),
                          cvPoint(rightBotX, rightBotY),
                          CV_RGB(255, 0, 0), 1, 8, 0);
                // Show labels
                if (labels[objId].c_str())
                {
                    DPRINTF("Label:%s\n\n", labels[objId].c_str());
                    putText(image, labels[objId].c_str(), cvPoint(leftTopX, leftTopY),
                            FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200, 200, 250), 1, CV_AA);
                }
            }

            if (boxes)
            {
                delete[] boxes;
                boxes = NULL;
            }
            if (labels)
            {
                delete[] labels;
                labels = NULL;
            }
        }
        imshow("Guardius", image);
        waitKey((1000 / TARGET_SHOW_FPS));
    }

    return 0;
}