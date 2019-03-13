//
// Created by fatcat on 2/21/19.
//

#ifndef MULTITHREADINGTEST_BONETAKER_H
#define MULTITHREADINGTEST_BONETAKER_H

#include <iostream>
#include <cmath>
#include <sstream>
#include <unistd.h>
#include <opencv2/opencv.hpp>
#include "BoneInfo.h"


class Taker{
public:
//    Taker(std::string outwindow){g_outwindow = outwindow;
//    //std::cout<<"workstart"<<std::endl;
//    }
//
//    ~Taker(){}

    void SetComparePositions(float positionxdelta = 25.0,
                             float positionydelta= 100.0,
                             float visionpointx = 340.0,
                             float visionpointy = 400.0){
        g_STANDARD_DISTANCE_X = positionxdelta;
        g_STANDARD_DISTANCE_Y = positionydelta;
        g_NOWPOSITION_X = visionpointx;
        g_NOWPOSITION_Y = visionpointy;
    };

    void SetThresholdDelta(int delta = 10){ thresh_delta_ = delta; };

    int GetThresholdDelta()const{ return thresh_delta_  ;};

    void SetHarrisPointsNeed(int num =20){ harrispoint_num_ = num; };

    int GetHarrisPointsNeed()const{ return harrispoint_num_;};

    void SetStandardDistance(float distance = 165.0 ){standard_distance_ = distance;};

    float GetStandardDistance(){ return standard_distance_;};

    cv::Mat RedChannel (cv::Mat input);

    std::vector<std::vector<cv::Point>> FindContours (cv::Mat input);

    std::vector<cv::Point2f> FindCornerPoint(cv::Mat src);

    cv::Point2f StandardPoint(std::vector<cv::Point2f> cornerpoints,std::vector<float> lineparameters);

    std::vector<float> StandardCross(cv::Point2f standardpoint,std::vector<float> lineparameters,cv::Mat target);

    std::vector<float> FindHoughLinesROI(cv::Mat canny,cv::Mat src,int x,int y);

    std::vector<cv::RotatedRect> GetROIArea(std::vector<std::vector<cv::Point>> contours);

    std::vector<float> ROIRect(std::vector<cv::RotatedRect> minrect);

    std::vector<float> Moveinfo(std::vector<float> sdcross,cv::Mat target);

    std::vector<float> PoseCalibration(cv::Mat src);

    bool getcameratate();


    int operator()(BoneInfo &boneinfo);


private:
    float g_STANDARD_DISTANCE_X ;

    float g_STANDARD_DISTANCE_Y ;

    float g_NOWPOSITION_X ;

    float g_NOWPOSITION_Y ;

    int thresh_delta_;

    int harrispoint_num_;

    float standard_distance_;

    std::string g_outwindow;

    int g_state;





};
#endif //MULTITHREADINGTEST_BONETAKER_H
