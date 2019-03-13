//
// Created by fatcat on 2/21/19.
//


#include "bonetaker.h"
using namespace std;
using namespace cv;

Mat Taker::RedChannel(Mat input) {
    Mat output;
    vector<Mat> channels;
    vector<Mat> mbgr(3);
    split(input, channels);
    Mat hidechannel(input.size(), CV_8UC1, Scalar(0));
    Mat redchannel(input.size(), CV_8UC3);
    mbgr[0] = hidechannel;
    mbgr[1] = hidechannel;
    mbgr[2] = channels[2];
    merge(mbgr, redchannel);
    //imshow("imageR-红色通道", redchannel);
    Mat gray;
    cvtColor(redchannel,gray,CV_BGR2GRAY);

    Scalar tempVal = cv::mean(gray);
    const int delta = Taker::GetThresholdDelta();
    float matMean = tempVal.val[0] +delta <255 ?tempVal.val[0]+delta:255;
    Mat thresh,result_er;
    threshold(gray,thresh,matMean,255, CV_THRESH_BINARY);
    Mat element_er = getStructuringElement(MORPH_RECT, Size(5, 5));
    erode(thresh, result_er, element_er);
    Mat element = getStructuringElement(MORPH_RECT, Size(7, 7));
    morphologyEx(result_er, output, MORPH_OPEN, element);

    return output;
}

vector<vector<Point>> Taker::FindContours(Mat input) {
    Mat image = input.clone();
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(image, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE, Point());
    if(contours.empty()){
        string error = "Find NO Contours! MinRect Do Not Exist!";
        cout<<error<<endl;
        int font_face = cv::FONT_HERSHEY_COMPLEX;
        putText(image, error,Point(image.cols*0.5,image.rows*0.5) , font_face, 0.5,
                cv::Scalar(255, 255, 255), 1, 8, 0);
    }
    return contours;
}

vector<RotatedRect> Taker::GetROIArea(vector<vector<Point>> contours){
    vector<RotatedRect> box(contours.size());
    if(!contours.empty()){
        for (int i = 0; i < contours.size(); i++) {//绘制轮廓的最小外结矩形
            box[i] = minAreaRect(Mat(contours[i]));
        }
    }else{
        RotatedRect zerorect(Point2f(100, 100), Size2f(1, 1), 0);
        box.push_back(zerorect);
    }

    return box;
}

vector<float> Taker::ROIRect(vector<RotatedRect> minrect){
    Point2f rect[4];
    vector<float> area;
    int max = 1;
    float point_x = 0;
    float point_y = 0;
    float width = 1;
    float height = 1;
    int maxdelta = 250;
    for(int i = 0;i<minrect.size();i++){
        if(minrect[i].size.area()>max){
            max = minrect[i].size.area();
            minrect[i].points(rect);

            point_x = rect[1].x > 0 ?rect[1].x : 0;
            point_y = rect[2].y > 0 ?rect[2].y : 0;
            width = rect[3].x - rect[1].x < 640?rect[3].x - rect[1].x:640;

            height = rect[0].y - rect[2].y < 480 ?rect[0].y - rect[2].y:480;
            if(point_y + height >480){
                height = 480 - point_y;
            }
            if(point_x + width + maxdelta > 640){
                width = 640 - point_x;
            } else{
                width = width + maxdelta;
            }
//            if(point_x + width + maxdelta<640){
//                width = width + maxdelta;
//            }else{
//                for(int i = 0;point_x + width + maxdelta>640;i++){
//                    maxdelta = maxdelta -i-1;
//                }
//            }

        }
    }
    area.push_back(point_x);
    area.push_back(point_y);
    area.push_back(width);
    area.push_back(height);

    return area;
}

vector<float> Taker::FindHoughLinesROI(Mat canny,Mat src,int x,int y) {

    vector<float> lineparameter;
    vector<Vec2f> lines;
    HoughLines(canny, lines, 1, CV_PI / 180, 60, 0, 0);

    if(lines.size() == 0 ){
        cerr<<"Find NO HoughLines!! Please Check threshold"<<endl;
    }
    vector<float> t_average;
    vector<float> theta_average;
    vector<Point2f> firstpoints;

    Point2f firstpoint_average;
    Point2f secondpoint;

    float t_line = 0;
    float t_all = 0;
    float taverage = 0;
    float b_line = 0;
    float b_average = 0;
    float firstpoint_x = 0;
    float firstpoint_y = 0;
    float theta_all = 0;
    float thetaaverage = 0;

    for (size_t i = 0; i < lines.size(); i++) {
        float rho = lines[i][0]; //就是圆的半径r
        float theta = lines[i][1]; //就是直线的角度
        Point2f pt1, pt2;
        float a = cos(theta), b = sin(theta);
        float x0 = a * rho, y0 = b * rho;
        pt1.x = cvRound(x0 + 1000 * (-b)) + x;
        pt1.y = cvRound(y0 + 1000 * (a)) + y;
        pt2.x = cvRound(x0 - 1000 * (-b)) + x;
        pt2.y = cvRound(y0 - 1000 * (a)) + y;

        t_line =  (pt2.x - pt1.x)/(pt2.y - pt1.y);

        if (abs(t_line) > 0 && abs(t_line) < 1) {           //abs(pt1.x - pt2.x) > 2

            float theta_hough = theta < CV_PI - theta ? CV_PI / 2 - (theta) : CV_PI / 2 - (CV_PI - theta);

            b_line = pt1.x - t_line*pt1.y;


            if (theta_hough > CV_PI / 6) {

                Point2f pointk;

                pointk.y = 0;
                pointk.x = t_line*pointk.y + b_line;
                firstpoint_x += pointk.x;
                firstpoint_y += pointk.y;

                t_all += t_line;
                theta_all += theta_hough;
                t_average.push_back(t_line);
                theta_average.push_back(theta_hough);
                firstpoints.push_back(pointk);

            }

        }

    }
    if (!t_average.empty()) {

        taverage = t_all / t_average.size();
        thetaaverage = theta_all / theta_average.size();

        firstpoint_average.x = firstpoint_x / firstpoints.size();
        firstpoint_average.y = firstpoint_y / firstpoints.size();

        b_average = firstpoint_average.x - taverage * firstpoint_average.y;

        secondpoint.y = 1000;
        secondpoint.x = taverage*secondpoint.y + b_average;

        line(src, firstpoint_average, secondpoint, Scalar(210, 0, 0), 1, LINE_AA);

        lineparameter.push_back(taverage);
        lineparameter.push_back(b_average);
        lineparameter.push_back(thetaaverage);

    }
    return lineparameter;
}

vector<Point2f> Taker::FindCornerPoint(Mat src) {


    vector<Point2f> roicorners;
    Mat mid = src.clone();
    cvtColor(src,mid,CV_BGR2GRAY);
    //imshow("gray",mid);
    vector<Point2f> corners;
    const int maxcorners = Taker::GetHarrisPointsNeed();
    float qualityLevel = 0.01;
    float minDistance = 10;
    int blockSize = 3;
    bool useHarrisDetector = false;
    float k = 0.04;

    goodFeaturesToTrack(mid,corners,maxcorners,qualityLevel,minDistance,Mat(),blockSize,useHarrisDetector,k);


    //cout << " Number of corners detected: " << corners.size() << endl;
    int r = 4;
    for (int i = 0; i < corners.size(); i++) {
        circle(src, corners[i], r, Scalar(255/(i+1),255/(i+1),255/(i+1)), -1, 8, 0);
        roicorners.push_back(corners[i]);
    }

    return roicorners;
}

Point2f Taker::StandardPoint(vector<Point2f> cornerpoints, vector<float> lineparameters) {

    Point2f standardpoint;

    float line_t = lineparameters[0];
    float line_b = lineparameters[1];

    float point_x = 0;
    float point_y = 0;
    float compare_x = 0;
    float maxpoint_y = 0;
    float denominator = sqrt(1+line_t*line_t);
    float distance;
    const float sddistance = Taker::GetStandardDistance();

    for(int i=0;i<cornerpoints.size();i++){
        point_x = cornerpoints[i].x;
        point_y = cornerpoints[i].y;
        distance = abs(point_x - line_t*point_y - line_b)/denominator;
        //cout<<"distance "<<i<<"is"<<distance<<endl;

        if(distance > sddistance +10 || distance < sddistance -10){
            cornerpoints[i].x = 0;
            cornerpoints[i].y = 0;
        }

        //cout<<"distance "<<i<<"is"<<distance<<endl;
    }
    //cout<<"after"<<cornerpoints.size()<<endl;

    for(int i=0;i<cornerpoints.size();i++){
        point_y = cornerpoints[i].y;
        point_x = cornerpoints[i].x;
        compare_x = line_t*point_y +line_b;
        if(point_y > maxpoint_y && point_y < 300 && point_x < compare_x){ //&& point_x < comparepoint.x
            maxpoint_y = point_y;
            standardpoint = cornerpoints[i];
            point_x = standardpoint.x;
            point_y = standardpoint.y;
            distance = abs(point_x - line_t*point_y - line_b)/denominator;
            //cout<<distance<<endl;

        }
    }
    if(standardpoint.x == 0 ){
        cerr<<"Can Not Find STANDARDPOINT!! Please Check!"<<endl;
    }
    return standardpoint;
}

vector<float> Taker::StandardCross(Point2f standardpoint, vector<float> lineparameters,Mat target) {

    vector<float> standcross;
    Point2f crosscenter;
    Point2f cross_up;
    Point2f cross_down;
    Point2f cross_left;
    Point2f cross_right;
    Point2f midpoint;

    float cross_b_y;
    float cross_b_x;

    float standardx = standardpoint.x;
    float standardy = standardpoint.y;
    float standard_deltax_x = 0;
    float standard_deltax_y = 0;
    float standard_deltay_x = 0;
    float standard_deltay_y = 0;
    float sd_line_t = lineparameters[0];
    float sd_line_theta = lineparameters[2];
    //float sd_line_k = 1/sd_line_t;
    //float sd_line_theta_y = atan(sd_line_k);
    //float sd_line_theta_x = atan(-sd_line_t);
    float crosstheta;

    //cout<<"夹角"<<sd_line_theta_y<<endl;
    //cout<<"垂直线夹角"<<sd_line_theta_x<<endl;
    //和直线平行方向
    if(sd_line_t>0){
        standard_deltax_x = g_STANDARD_DISTANCE_Y*cos(sd_line_theta);
        standard_deltax_y = g_STANDARD_DISTANCE_Y*sin(sd_line_theta);
        //和直线垂直方向
        standard_deltay_x = g_STANDARD_DISTANCE_X*sin(sd_line_theta);
        standard_deltay_y = - g_STANDARD_DISTANCE_X*cos(sd_line_theta);

        crosstheta = 90-180*sd_line_theta/CV_PI;
    }else{
        standard_deltax_x = - g_STANDARD_DISTANCE_Y*cos(sd_line_theta);
        standard_deltax_y = g_STANDARD_DISTANCE_Y*sin(sd_line_theta);
        //和直线垂直方向
        standard_deltay_x = g_STANDARD_DISTANCE_X*sin(sd_line_theta);
        standard_deltay_y = g_STANDARD_DISTANCE_X*cos(sd_line_theta);

        crosstheta = 180*sd_line_theta/CV_PI -90;
    }


    crosscenter.x = standardx + standard_deltax_x + standard_deltay_x;
    crosscenter.y = standardy + standard_deltax_y + standard_deltay_y;
    circle(target, crosscenter, 4, Scalar(0,210,210), -1, 8, 0);

    midpoint.x = standardx + standard_deltax_x;
    midpoint.y = standardy + standard_deltax_y;

    //circle(target, midpoint, 4, Scalar(0,210,255), -1, 8, 0);
    //line(target, standardpoint, midpoint, Scalar(100, 0, 0), 1, LINE_AA);
    //line(target, midpoint, crosscenter, Scalar(100, 0, 0), 1, LINE_AA);

    cross_b_y = crosscenter.x - sd_line_t*crosscenter.y;
    cross_b_x = crosscenter.y - (-sd_line_t)*crosscenter.x;
    //y方向画线
    cross_up.y = crosscenter.y - 20;
    cross_up.x = sd_line_t*cross_up.y+cross_b_y;
    cross_down.y = crosscenter.y + 20;
    cross_down.x = sd_line_t*cross_down.y+cross_b_y;
    //x方向画线
    cross_left.x = crosscenter.x - 20;
    cross_left.y = (-sd_line_t)*cross_left.x + cross_b_x;
    cross_right.x = crosscenter.x + 20;
    cross_right.y = (-sd_line_t)*cross_right.x + cross_b_x;

    line(target, cross_up, cross_down, Scalar(0, 0, 255), 1, LINE_AA);
    line(target, cross_left, cross_right, Scalar(0, 0, 255), 1, LINE_AA);

    standcross.push_back(crosscenter.x);
    standcross.push_back(crosscenter.y);
    standcross.push_back(crosstheta);
    return  standcross;
}

vector<float> Taker::Moveinfo(vector<float> sdcross,Mat target) {

    vector<float> moveinfo;
    Point2f nowpoint;
    Point2f targetpoint;
    float deltax;
    float deltay;
    float distance;
    float angle2move;
    float angle2rotate;
    int font_face = cv::FONT_HERSHEY_COMPLEX;
    string nowposition ="nowpositon";
    string targetpoistion = "targetpoistion";
    stringstream so;
    stringstream st;
    stringstream str;
    string dis;
    string ang;
    string rot;

    nowpoint.x = g_NOWPOSITION_X;
    nowpoint.y = g_NOWPOSITION_Y;
    circle(target, nowpoint, 4, Scalar(0,210,210), -1, 8, 0);
    putText(target, nowposition,nowpoint, font_face, 0.5, cv::Scalar(255, 255, 255), 1, 8, 0);

    targetpoint.x = sdcross[0];
    targetpoint.y = sdcross[1];
    putText(target, targetpoistion,targetpoint , font_face, 0.5, cv::Scalar(255, 255, 255), 1, 8, 0);
    line(target, nowpoint, targetpoint, Scalar(0, 0, 255), 1, LINE_AA);

    deltax = targetpoint.x - nowpoint.x;
    deltay = targetpoint.y - nowpoint.y;

    distance = sqrt(deltax*deltax + deltay*deltay);
    so<<distance;
    so>>dis;
    putText(target, "Distance to move"+dis,Point(0,30) , font_face, 1, cv::Scalar(0, 0,0), 1, 8, 0);


    angle2move = atan(deltay/deltax);//为正值时，在右上角，为负值时，在左上角。
    st<<angle2move;
    st>>ang;
    putText(target, "Offset angle"+ang,Point(0,65) , font_face, 1, cv::Scalar(0, 0,0), 1, 8, 0);

    angle2rotate = sdcross[2];
    string direction2rotate;

    str<<angle2rotate;
    str>>rot;
    if(angle2rotate > 0){
        direction2rotate = "Anti-Clockwise";
        putText(target, "Rotate angle"+direction2rotate+rot,Point(0,105) , font_face, 1, cv::Scalar(0, 0,0), 1, 8, 0);

    }else{
        direction2rotate = "Clockwise";
        angle2rotate = -angle2rotate;
        putText(target, "Rotate angle"+direction2rotate+rot,Point(0,100) , font_face, 1, cv::Scalar(0, 0,0), 1, 8, 0);

    }


    //cout<<"需要移动的距离"<<distance<<endl;
    //cout<<"移动方向角度"<<angle2move<<endl;
    //cout<<"MR1需"<<direction2rotate<<"旋转"<<abs(angle2rotate)<<"度"<<endl;
    moveinfo.push_back(distance);
    moveinfo.push_back(angle2move);
    moveinfo.push_back(angle2rotate);

    return moveinfo;
}

vector<float> Taker::PoseCalibration(Mat src) {

    Mat mid = src(Rect(0,src.rows*0, src.cols,src.rows));
    Mat output,canny,redchannel;
    output = mid.clone();
    redchannel = RedChannel(mid);
    //imshow("redchannel",redchannel);
    Canny(redchannel,canny,20,30,3);
    //imshow("canny",canny);
    vector<vector<Point>> contours;
    contours = FindContours(canny);
    vector<RotatedRect> roiarea = GetROIArea(contours);
    vector<float> roirect = ROIRect(roiarea);
    vector<float> lineparameter;
    vector<float> moveinfo;
    Point2f firstpoint;
    firstpoint.x = roirect.at(0);
    firstpoint.y = roirect.at(1);
    float roiwidth = roirect.at(2);
    float roiheight = roirect.at(3);
    Point2f sdpoint;
    int font_face = cv::FONT_HERSHEY_COMPLEX;

    vector<Point2f> harrispoint;
    vector<float> crossinfo;
    if(!contours.empty() && firstpoint.x<canny.cols && firstpoint.y<canny.rows){
        Mat roi_fin = canny(Rect(firstpoint.x,firstpoint.y, roiwidth,roiheight));
        //imshow("ROI",roi_fin);

        lineparameter = FindHoughLinesROI(roi_fin,output,firstpoint.x,firstpoint.y);
        if(!lineparameter.empty()){
            harrispoint = FindCornerPoint(mid);
            sdpoint = StandardPoint(harrispoint,lineparameter);
            crossinfo = StandardCross(sdpoint,lineparameter,output);
            moveinfo = Moveinfo(crossinfo,output);
        }
        circle(output, sdpoint, 4, Scalar(0,0,210), -1, 8, 0);
    }
    //imshow(g_outwindow,output);
    return moveinfo;
}

//int Taker::operator()(BoneInfo &boneinfo,Mat src) {
//    vector<float> moveinfo;
//    moveinfo = PoseCalibration(src);
//
//    float movedistance = moveinfo[0];
//    float moveangel = moveinfo[1];
//    float rotateangle = moveinfo[2];
//
//    int flag = 0;
//    boneinfo.set(rotateangle,moveangel,movedistance,flag);
//
//    boneinfo.setThreadState(false);
//
//    return 0;
//}

//int Taker::operator()(BoneInfo &boneinfo, cv::Mat src) { //join
//
//    Mat mid = src(Rect(0,src.rows*0, src.cols,src.rows));
//    Mat output,canny,redchannel;
//    output = mid.clone();
//    redchannel = RedChannel(mid);
//    //imshow("redchannel",redchannel);
//    Canny(redchannel,canny,20,30,3);
//    //imshow("canny",canny);
//    vector<vector<Point>> contours;
//    contours = FindContours(canny);
//    vector<RotatedRect> roiarea = GetROIArea(contours);
//    vector<float> roirect = ROIRect(roiarea);
//    vector<float> lineparameter;
//    vector<float> moveinfo;
//    Point2f firstpoint;
//    firstpoint.x = roirect.at(0);
//    firstpoint.y = roirect.at(1);
//    float roiwidth = roirect.at(2);
//    float roiheight = roirect.at(3);
//    Point2f sdpoint;
//    int font_face = cv::FONT_HERSHEY_COMPLEX;
//
//    vector<Point2f> harrispoint;
//    vector<float> crossinfo;
//
//    float movedistance;
//    float moveangel;
//    float rotateangle;
//
//    if(!contours.empty() && firstpoint.x<canny.cols && firstpoint.y<canny.rows){
//        Mat roi_fin = canny(Rect(firstpoint.x,firstpoint.y, roiwidth,roiheight));
//        //imshow("ROI",roi_fin);
//
//        lineparameter = FindHoughLinesROI(roi_fin,output,firstpoint.x,firstpoint.y);
//        if(!lineparameter.empty()){
//            harrispoint = FindCornerPoint(mid);
//            sdpoint = StandardPoint(harrispoint,lineparameter);
//            crossinfo = StandardCross(sdpoint,lineparameter,output);
//            moveinfo = Moveinfo(crossinfo,output);
//            movedistance = moveinfo[0];
//            moveangel = moveinfo[1];
//            rotateangle = moveinfo[2];
//        }else{
//            movedistance = 0;
//            moveangel = 0;
//            rotateangle = 0;
//            cerr<<"NO BONE EXIST,STAY"<<endl;
//        }
//        //circle(output, sdpoint, 4, Scalar(0,0,210), -1, 8, 0);
//    }else{
//        movedistance = 0;
//        moveangel = 0;
//        rotateangle = 0;
//        cerr<<"NO BONE EXIST,STAY"<<endl;
//    }
//
//
//
//    int flag = 0;
//    boneinfo.set(rotateangle,moveangel,movedistance,flag);
//    //imshow(g_outwindow,output);
//}
bool Taker::getcameratate() {
    bool tVideo1 = (access("/dev/video1", R_OK) != -1);
    if(tVideo1)
        return true;
    else
        return false;
}

int Taker::operator()(BoneInfo &boneinfo) {  //detach
    VideoCapture capture("/home/fatcat/robcon2019/rtl/bone identification/camera/3.avi");

    SetComparePositions();
    SetThresholdDelta();
    SetHarrisPointsNeed();
    SetStandardDistance();

    int flag = 0;

    while (1){
//        bool tVideo1 = (access("/dev/video1", R_OK) != -1);
//        if(!tVideo1){
//            flag = 1;
//            break;
//        }
//        clock_t start,finish;
//        double time_length;
//        start = clock();
        Mat src;
        capture>>src;
        Mat mid = src(Rect(0,src.rows*0, src.cols,src.rows));
        Mat output,canny,redchannel;
        output = mid.clone();
        redchannel = RedChannel(mid);
        //imshow("redchannel",redchannel);
        Canny(redchannel,canny,20,30,3);
        //imshow("canny",canny);
        vector<vector<Point>> contours;
        contours = FindContours(canny);
        vector<RotatedRect> roiarea = GetROIArea(contours);
        vector<float> roirect = ROIRect(roiarea);
        vector<float> lineparameter;
        vector<float> moveinfo;
        Point2f firstpoint;
        firstpoint.x = roirect.at(0);
        firstpoint.y = roirect.at(1);
        float roiwidth = roirect.at(2);
        float roiheight = roirect.at(3);
        Point2f sdpoint;
        int font_face = cv::FONT_HERSHEY_COMPLEX;

        vector<Point2f> harrispoint;
        vector<float> crossinfo;

        float movedistance;
        float moveangel;
        float rotateangle;

        if(!contours.empty() && firstpoint.x<canny.cols && firstpoint.y<canny.rows){
            Mat roi_fin = canny(Rect(firstpoint.x,firstpoint.y, roiwidth,roiheight));
            //imshow("ROI",roi_fin);

            lineparameter = FindHoughLinesROI(roi_fin,output,firstpoint.x,firstpoint.y);
            if(!lineparameter.empty()){
                harrispoint = FindCornerPoint(mid);
                sdpoint = StandardPoint(harrispoint,lineparameter);
                crossinfo = StandardCross(sdpoint,lineparameter,output);
                moveinfo = Moveinfo(crossinfo,output);
                movedistance = moveinfo[0];
                moveangel = moveinfo[1];
                rotateangle = moveinfo[2];
            }else{
                movedistance = 0;
                moveangel = 0;
                rotateangle = 0;
                cerr<<"NO BONE EXIST,STAY"<<endl;
            }
            //circle(output, sdpoint, 4, Scalar(0,0,210), -1, 8, 0);
        }else{
            movedistance = 0;
            moveangel = 0;
            rotateangle = 0;
            cerr<<"NO BONE EXIST,STAY"<<endl;
        }
//        cout<<"1"<<endl;
        boneinfo.set(rotateangle,moveangel,movedistance,flag);
        imshow("result",output);
        //waitKey(20);
//        finish=clock();
//        time_length = (double)(finish - start) / CLOCKS_PER_SEC; //根据两个时刻的差，计算出运行的时间
//        cout<<"time is"<<time_length<<endl;
    }
    capture.release();
    boneinfo.setThreadState(false);
}