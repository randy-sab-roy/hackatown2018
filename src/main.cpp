#include "../arapaho/arapaho.hpp"
#include "../include/entity.hpp"
#include "../include/arduino-serial-lib.h"
#include <string.h>
#include "opencv2/core/core.hpp"
#include <opencv2/imgproc.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "../arapaho/arapaho.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <chrono>
#include <vector>
#include <iostream>

#include <restclient-cpp/restclient.h>

#define TARGET_SHOW_FPS (120)
#define MOVEMENT_THRESHOLD (50000)

using namespace cv;
using namespace std;

static char INPUT_DATA_FILE[] = "input/input.data";
static char INPUT_CFG_FILE[] = "input/input.cfg";
static char INPUT_WEIGHTS_FILE[] = "input/input.weights";
static char INPUT_AV_FILE[] = "input/input.mp4";

vector<Entity> entities;
int currentFrame = 0;
int oldRisk = 0;

Vector2D centerPoint(box b, int imageWidthPixels, int imageHeightPixels)
{
    return Vector2D(b.x * imageWidthPixels, (1.0 - b.y) * imageHeightPixels);
}

bool isGoodLabel(const string& label)
{
    return (label == "car") || (label == "truck") ||
            (label == "bus") || (label == "person") ||
            (label == "bicycle") || (label == "motorbike");
}

void displayBoxes(box *boxes, int numObjects, int imageWidthPixels, int imageHeightPixels, Mat image, string * labels)
{
    int leftTopX = 0, leftTopY = 0, rightBotX = 0, rightBotY = 0;
    for (int objId = 0; objId < numObjects; objId++)
    {
        if(!isGoodLabel(labels[objId]))
        {
            continue;
        }
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
            putText(image, labels[objId].c_str(), cvPoint(leftTopX, leftTopY),
                    FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200, 200, 250), 1, CV_AA);
        }
    }
}

void updateEntities(box* boxes, string* labels, int numObjects, int imageWidthPixels, int imageHeightPixels)
{
    Vector2D * centers = new Vector2D[numObjects];
    bool * enabled = new bool[numObjects];

    // Calculate center points
    for(int i = 0; i < numObjects; i++)
    {
        centers[i] = centerPoint(boxes[i], imageWidthPixels, imageHeightPixels);
        enabled[i] = true;
    }

    // Loop over existing entities
    for(int i = 0; i < entities.size(); i++)
    {
        int closestIndex = -1;
        int closestDistance = -1;

        // Loop over new boxes
        for(int j = 0; j < numObjects; j++)
        {
            if(enabled[j] && isGoodLabel(labels[j]))
            {
                int tmpDistance = entities[i].lastPosition().distance(centers[j]);
                if((closestDistance == -1) || tmpDistance < closestDistance)
                {
                    closestDistance = tmpDistance;
                    closestIndex = j;
                }
            }
        }

        if(closestDistance > -1 && closestDistance < MOVEMENT_THRESHOLD)
        {
            entities[i].addPosition(centers[closestIndex]);
//            std::cout << entities[i].getVelocity().x << ", " << entities[i].getVelocity().y << endl;
            enabled[closestIndex] = false;
            entities[i].lastFrame = currentFrame;
        }
    }

    // Erase unused entities
    for (int i = 0; i < entities.size(); ++i) {
        if(entities[i].lastFrame != currentFrame)
        {
            entities.erase(entities.begin() + i);
        }

    }

    // Create new entities
    for(int i = 0; i < numObjects; i++)
    {
        if(enabled[i] && isGoodLabel(labels[i]))
        {
            Entity ent = Entity();
            ent.addPosition(centers[i]);
            ent.entityType = labels[i];
            entities.push_back(ent);
            ent.lastFrame = currentFrame;
//            cout << "N'importe quoi" << endl;
        }
    }
}

int getRiskLevel()
{
    int riskLevel = 0;
    for (int i = 0; i < entities.size(); ++i) {
        for (int j = i+1; j < entities.size(); ++j) {
            int tmpRisk = entities[i].calculateRisk(entities[j]);
            riskLevel = tmpRisk > riskLevel ? tmpRisk : riskLevel;
        }
    }
    if(oldRisk > riskLevel)
    {
        riskLevel = (0.9*oldRisk + 0.1*riskLevel);
    }
    oldRisk = riskLevel;
    return riskLevel;
}

int main()
{

    int serialSocket = serialport_init("/dev/cu.usbmodem1411", 9600);
    serialport_write(serialSocket, "0");


    ArapahoV2 *darknet = new ArapahoV2();
    ArapahoV2Params params;

    params.datacfg = INPUT_DATA_FILE;
    params.cfgfile = INPUT_CFG_FILE;
    params.weightfile = INPUT_WEIGHTS_FILE;
    params.nms = 0.4;
    params.maxClasses = 4;

    int expectedWidth = 0;
    int expectedHeight = 0;

    box *boxes = 0;
    std::string *labels;

    bool isSuccessful = darknet->Setup(params, expectedWidth, expectedHeight);
    if (!isSuccessful)
    {
        return -1;
    }

    ArapahoV2ImageBuff imageBuffer;
    Mat image;
    int lastRisk = 0;

    namedWindow("Guardius", CV_WINDOW_KEEPRATIO);
    VideoCapture cap(INPUT_AV_FILE);
    while (1)
    {
        currentFrame++;
        bool success = cap.read(image);
        int imageWidthPixels = 0, imageHeightPixels = 0;

        if (!success || image.empty())
        {
            cap.set(CV_CAP_PROP_POS_FRAMES, 0);
            continue;

            //if (darknet)
            //    delete darknet;
            //darknet = nullptr;
            //waitKey();
            //return -1;
        }

        imageWidthPixels = image.size().width;
        imageHeightPixels = image.size().height;

        // Process the image
        imageBuffer.bgr = image.data;
        imageBuffer.w = imageWidthPixels;
        imageBuffer.h = imageHeightPixels;
        imageBuffer.channels = 3;

        int numObjects = 0;

        // Detect the objects in the image
        darknet->Detect(image, 0.24, 0.5, numObjects);

        if (numObjects > 0)
        {
            boxes = new box[numObjects];
            labels = new std::string[numObjects];

            // Get boxes and labels
            darknet->GetBoxes(boxes, labels, numObjects);
            displayBoxes(boxes, numObjects, imageWidthPixels, imageHeightPixels, image, labels);
            updateEntities(boxes, labels, numObjects, imageWidthPixels, imageHeightPixels);

            if (boxes)
            {
                delete[] boxes;
                boxes = nullptr;
            }
            if (labels)
            {
                delete[] labels;
                labels = nullptr;
            }
        }



        // This is where magic happens
        int risk = getRiskLevel();

        rectangle(image,
                  cvPoint(0, imageHeightPixels - 30),
                  cvPoint(60, imageHeightPixels),
                  CV_RGB(0, 0, 0), CV_FILLED, 8, 0);

        putText(image, to_string(risk), cvPoint(7, imageHeightPixels - 7),
                FONT_HERSHEY_PLAIN, 1, risk > 160 ? CV_RGB(255, 0, 0) : CV_RGB(0, 255, 0), 1, CV_AA);

        serialport_write(serialSocket, std::to_string(constrain(0, 16, risk / 20) + 1).c_str());

        Mat dst;
        resize(image, dst, Size(), 4, 4, INTER_LINEAR);
        imshow("Guardius", dst);
        waitKey((1000 / TARGET_SHOW_FPS));
    }

    serialport_close(serialSocket);
    return 0;
}