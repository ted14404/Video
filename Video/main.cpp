#include "opencv2/opencv.hpp"
#include <iostream>
#include <cstdlib>
using namespace std;
using namespace cv;

long gr[480][640];
long gr2[480][640];
int main() {

    // Create a VideoCapture object and open the input file
    // If the input is the web camera, pass 0 instead of the video file name
    VideoCapture cap;


    // Check if camera opened successfully
    if (!cap.open(0)) {
        cout << "Error opening video stream or file" << endl;
        return -1;
    }

    namedWindow("Frame", 1);


    int midhue = 100;
    int ranghue = 179;

    createTrackbar("rangHue", "Frame", &ranghue, 179);
    createTrackbar("midHue", "Frame", &midhue, 179);

    int midsat = 0;
    int rangsat = 255;

    createTrackbar("rangsat", "Frame", &rangsat, 255);
    createTrackbar("midsat", "Frame", &midsat, 255);

    int midlum = 0;
    int ranglum = 100;

    createTrackbar("ranglum", "Frame", &ranglum, 255);
    createTrackbar("midlum", "Frame", &midlum, 255);


    int sh = 179, ss = 128, sl = 128;
    createTrackbar("H", "Frame", &sh, 179);
    createTrackbar("S", "Frame", &ss, 128);
    createTrackbar("L", "Frame", &sl, 128);

    long* xl = new long[480 * 640];
    long* yl = new long[480 * 640];


    while (1) {

        Mat frame;
        // Capture frame-by-frame
        cap >> frame;
        Mat hsv;
        Mat hsv1;
        cvtColor(frame, hsv, COLOR_BGR2HSV);
        //frame.
        for (int x = 0; x < frame.cols; x++) {
            for (int y = 0; y < frame.rows; y++) {
                uchar* c2 = hsv.ptr(y, x);
                uchar* c = frame.ptr(y, x);
                /**(c) = *(c2 + 2);
                *(c + 1) = *(c2 + 2);
                *(c + 2) = *(c2 + 2);
                continue;
                int diff = abs(*(c + 2) - myColorR) +
                    abs(*(c + 1) - myColorG) +
                    abs(*(c) - myColorB);*/
                    //if (abs(*(c + 2)-200) < 50&& abs(*(c + 1)-88)<50 && abs((*c)-100)<50) { *(c + 2) = 0; *(c + 1) = 255; *(c) = 0;}



                int h = abs(*(c2 + 0) - midhue);
                if (h > 90) { h = 179 - h; }

                int s = abs(*(c2 + 1) - midsat);
                int l = abs(*(c2 + 2) - midlum);

                if (h < ranghue && s < rangsat && l < ranglum) { *(c2 + 0) = sh; *(c2 + 1) = ss; *(c2 + 2) = sl; gr[y][x] = 1; }
                else { gr[y][x] = 0; }
                gr2[y][x] = 0;
                //if (*(c2 + 2)> midlum) {    *(c2 + 2)= midlum; }
               //if ((*(c2 + 0) > 150) && (((*(c2 + 1) > 200)) || ((*(c2 + 1) < 110)))) { *(c + 2) = 0; *(c + 1) = 255; *(c) = 0; }
            }
        }
        //cvtColor(hsv, frame, COLOR_HSV2BGR);
        //cvtColor(frame, hsv, COLOR_BGR2HSV);


        int to = 0;
        int from = 0;
        
        int mc=2;
        int width = frame.cols; int heigth = frame.rows;
        for (int y = 0; y < heigth; y++) {
            for (int x = 1; x < width; x++) {

                if (gr[y][x]==1) {
                    from = 0;
                    to = 0;

                    gr[y][x] = mc; 
                    xl[to] = x;
                    yl[to] = y;
                    to++;
                    int maxx = x, minx = x, maxy = y, miny = y, count = 1;
                    while (from < to) {
                        int x = xl[from];
                        int y = yl[from];
                        from++;
         

                        if ((y > 0) && gr[y-1][x] == 1 ) {
                            gr[y-1][x] = mc;
                            xl[to] = x;
                            yl[to] = y - 1;
                            to++;
                            count++;
                            if (miny > y - 1) { miny = y - 1; }
                        }
                        if ((x > 0) && gr[y][x-1]==1) {
                            gr[y][x-1] = mc;
                            xl[to] = x - 1;
                            yl[to] = y;
                            to++;
                            count++;
                            if (minx > x - 1) { minx = x - 1; }
                        }

                        if ((y < heigth - 1) && gr[y+1][x]==1) {
                            gr[y+1[x] = mc;
                            xl[to] = x;
                            yl[to] = y + 1;
                            to++;
                            count++;
                            if (maxy < y + 1) { maxy = y + 1; }
                        }
                        if ((x < width - 1) && gr[y][x+1]==1) {
                            gr[y][x+1] = mc;
                            xl[to] = x + 1;
                            yl[to] = y;
                            to++;
                            count++;
                            if (maxx < x + 1) { maxx = x + 1; }
                        }
                    }
                    
                    int avgx = (minx + maxx) / 2;
                    int avgy = (miny + maxy) / 2;
                    if(gr[avgy][avgx]!=mc){
                        
                        gr2[avgy][avgx] = mc;
                        xl[to] = avgx;
                        yl[to] = avgy;
                        to++;
                        int count2 = 1;
                        while (from < to) {
                            int x = xl[from];
                            int y = yl[from];
                            from++;


                            if ((y > 0) && gr[y - 1][x] !=mc && gr2[y - 1][x] != mc) {
                                gr2[y - 1][x] = mc;
                                xl[to] = x;
                                yl[to] = y - 1;
                                to++;
                                count2++;
                            }
                            if ((x > 0) && gr[y][x - 1] != mc && gr2[y][x - 1] != mc) {
                                gr2[y][x - 1] = mc;
                                xl[to] = x - 1;
                                yl[to] = y;
                                to++;
                                count2++;
                            }

                            if ((y < heigth - 1) && gr[y + 1][x] != mc && gr2[y + 1][x] != mc) {
                                gr2[y + 1[x] = mc;
                                xl[to] = x;
                                yl[to] = y + 1;
                                to++;
                                count2++;
                            }
                            if ((x < width - 1) && gr[y][x + 1] != mc && gr2[y][x + 1] != mc) {
                                gr2[y][x + 1] = mc;
                                xl[to] = x + 1;
                                yl[to] = y;
                                to++;
                                count2++;
                            }
                        }
                    
                    }
                    mc++;
                }
            }
        }

        cvtColor(hsv, frame, COLOR_HSV2BGR);


        // If the frame is empty, break immediately
        if (frame.empty())
            break;

        // Display the resulting frame
        imshow("Frame", frame);

        // Press  ESC on keyboard to exit
        char c = (char)waitKey(25);
        if (c == 27)
            break;
    }

    // When everything done, release the video capture object
    cap.release();

    // Closes all the frames
    destroyAllWindows();

    return 0;
}
/*#include "opencv2/opencv.hpp"
#include <iostream>
#include <cstdlib>
using namespace std;
using namespace cv;

long gr[480][640];
long gr2[480][640];
int main() {

    // Create a VideoCapture object and open the input file
    // If the input is the web camera, pass 0 instead of the video file name
    VideoCapture cap;


    // Check if camera opened successfully
    if (!cap.open(0)) {
        cout << "Error opening video stream or file" << endl;
        return -1;
    }

    namedWindow("Frame", 1);


    int midhue = 100;
    int ranghue = 179;

    createTrackbar("rangHue", "Frame", &ranghue, 179);
    createTrackbar("midHue", "Frame", &midhue, 179);

    int midsat = 0;
    int rangsat = 255;

    createTrackbar("rangsat", "Frame", &rangsat, 255);
    createTrackbar("midsat", "Frame", &midsat, 255);

    int midlum = 0;
    int ranglum = 100;

    createTrackbar("ranglum", "Frame", &ranglum, 255);
    createTrackbar("midlum", "Frame", &midlum, 255);


    int sh = 179, ss = 128, sl = 128;
    createTrackbar("H", "Frame", &sh, 179);
    createTrackbar("S", "Frame", &ss, 128);
    createTrackbar("L", "Frame", &sl, 128);
    
    long* xl = new long[480 * 640];
    long* yl = new long[480 * 640];

   
    while (1) {

        Mat frame;
        // Capture frame-by-frame
        cap >> frame;
        Mat hsv;
        Mat hsv1;
        cvtColor(frame, hsv, COLOR_BGR2HSV);
        //frame.
        for (int x = 0; x < frame.cols; x++) {
            for (int y = 0; y < frame.rows; y++) {
                uchar* c2 = hsv.ptr(y, x);
                uchar* c = frame.ptr(y, x);
                /**(c) = *(c2 + 2);
                *(c + 1) = *(c2 + 2);
                *(c + 2) = *(c2 + 2);
                continue;
                int diff = abs(*(c + 2) - myColorR) +
                    abs(*(c + 1) - myColorG) +
                    abs(*(c) - myColorB);
                //if (abs(*(c + 2)-200) < 50&& abs(*(c + 1)-88)<50 && abs((*c)-100)<50) { *(c + 2) = 0; *(c + 1) = 255; *(c) = 0;}

                

                int h = abs(*(c2 + 0) - midhue);
                if (h > 90) { h = 179 - h;}

                int s = abs(*(c2 + 1) - midsat);
                int l = abs(*(c2 + 2) - midlum);
                
                if (h < ranghue && s < rangsat && l < ranglum) { *(c2 + 0) = sh; *(c2 + 1) = ss; *(c2 + 2) = sl; gr[y][x] = 1; }
                else { gr[y][x] = 0; }
                gr2[y][x] = 0;
                //if (*(c2 + 2)> midlum) {    *(c2 + 2)= midlum; }
               //if ((*(c2 + 0) > 150) && (((*(c2 + 1) > 200)) || ((*(c2 + 1) < 110)))) { *(c + 2) = 0; *(c + 1) = 255; *(c) = 0; }
            }
        }
        //cvtColor(hsv, frame, COLOR_HSV2BGR);
        //cvtColor(frame, hsv, COLOR_BGR2HSV);
        
 
        int to = 0;
        int from = 0;

        int mc[3] = {20,255,255};
        for (int y = 1; y < frame.rows-1; y++) {
            for (int x = 1; x < frame.cols-1; x++) {
                
                 
                 uchar* c2 = hsv.ptr(y, x);
                 
                    if (*(c2 + 0)==sh && *(c2 + 1)==ss && *(c2 + 2)==sl) {
                        from = 0;
                        to = 0;

                        *(c2 + 0)=mc[0]; *(c2 + 1)=mc[1]; *(c2 + 2)=mc[2];
                        xl[to] = x;
                        yl[to] = y;
                        to++;
                        int maxx = x, minx = x, maxy = y, miny = y,count=1;
                        while (from < to) {
                            int x = xl[from];
                            int y = yl[from];
                            from++;
                            if (y<1 || x<1 || x>frame.cols - 2 || y>frame.rows - 2) { continue; }
                            uchar* c = hsv.ptr(y - 1, x);
                            uchar* c1 = hsv.ptr(y, x - 1);
                            uchar* c3 = hsv.ptr(y + 1, x);
                            uchar* c4 = hsv.ptr(y, x + 1);

                            if ((y > 0) && *(c + 0) == sh && *(c + 1) == ss && *(c + 2) == sl) {
                                *(c + 0) = mc[0]; *(c + 1) = mc[1]; *(c + 2) = mc[2];
                                xl[to] = x;
                                yl[to] = y - 1;
                                to++;
                                count++;
                                if (miny > y - 1) { miny = y - 1; }
                            }
                            if ((x > 0) && *(c1 + 0) == sh && *(c1 + 1) == ss && *(c1 + 2) == sl) {
                                *(c1 + 0) = mc[0]; *(c1 + 1) = mc[1]; *(c1 + 2) = mc[2];
                                xl[to] = x - 1;
                                yl[to] = y;
                                to++;
                                count++;
                                if (minx > x - 1) { minx = x - 1; }
                            }

                            if ((y < frame.rows - 1) && *(c3 + 0) == sh && *(c3 + 1) == ss && *(c3 + 2) == sl) {
                                *(c3 + 0) = mc[0]; *(c3 + 1) = mc[1]; *(c3 + 2) = mc[2];
                                xl[to] = x;
                                yl[to] = y + 1;
                                to++;
                                count++;
                                if (maxy < y + 1) { maxy = y + 1; }
                            }
                             if ((x < frame.cols - 1) && *(c4 + 0) == sh && *(c4 + 1) == ss && *(c4 + 2) == sl) {
                                *(c4 + 0) = mc[0]; *(c4 + 1) = mc[1]; *(c4 + 2) = mc[2];
                                xl[to] = x + 1;
                                yl[to] = y;
                                to++;
                                count++;
                                if (maxx < x + 1) { maxx = x + 1; }
                            }
                        }
                        if(mc[0])(minx + maxx) / 2;
                        if (mc[0] < 179) { mc[0]= mc[0]+10; }
                        else { mc[0] = 20; }
                }
            }
        }
        
         cvtColor(hsv, frame, COLOR_HSV2BGR);


        // If the frame is empty, break immediately
        if (frame.empty())
            break;

        // Display the resulting frame
        imshow("Frame", frame);

        // Press  ESC on keyboard to exit
        char c = (char)waitKey(25);
        if (c == 27)
            break;
    }

    // When everything done, release the video capture object
    cap.release();

    // Closes all the frames
    destroyAllWindows();

    return 0;
}*/

/*
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
    // Read original image 
    Mat src = imread("D:\\Robot\\IMG_20210308_141746.JPG");

    //if fail to read the image
    if (!src.data)
    {
        cout << "Error loading the image" << endl;
        return -1;
    }

    // Create a window
    namedWindow("My Window", 1);

    //Create trackbar to change brightness
    int iSliderValue1 = 50;
    createTrackbar("Brightness", "My Window", &iSliderValue1, 100);

    //Create trackbar to change contrast
    int iSliderValue2 = 50;
    createTrackbar("Contrast", "My Window", &iSliderValue2, 100);

    while (true)
    {
        //Change the brightness and contrast of the image (For more infomation http://opencv-srf.blogspot.com/2013/07/change-contrast-of-image-or-video.html)
        Mat dst;
        int iBrightness = iSliderValue1 - 50;
        double dContrast = iSliderValue2 / 50.0;
        src.convertTo(dst, -1, dContrast, iBrightness);

        //show the brightness and contrast adjusted image
        imshow("My Window", dst);

        // Wait until user press some key for 50ms
        int iKey = waitKey(50);

        //if user press 'ESC' key
        if (iKey == 27)
        {
            break;
        }
    }

    return 0;
}*/