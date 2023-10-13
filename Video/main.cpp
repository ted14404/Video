#include "opencv2/opencv.hpp"
#include <iostream>
#include <cstdlib>
#define n (64)
#define iw 1280
#define ih 720
using namespace std;
using namespace cv;

struct PointInfo {
    double x;
    double y;
    double a;
    long long c;
};

long gr[ih][iw];
long gr2[ih][iw];
bool Mark[6][16];
PointInfo P[n];
double An[n];
int maxAn[4] = { -1,-1,-1,-1 };

void MorePx(double x,double y, long long c){
    gr[(int)y - 1][(int)x] = c;
    gr[(int)y][(int)x - 1] = c;
    gr[(int)y][(int)x + 1] = c;
    gr[(int)y + 1][(int)x] = c;

    gr[(int)y - 1][(int)x - 1] = c;
    gr[(int)y - 1][(int)x + 1] = c;
    gr[(int)y + 1][(int)x - 1] = c;
    gr[(int)y + 1][(int)x + 1] = c;

}
bool getM(Mat & m,int lum, double x, double y) {
    uchar* c = m.ptr(y, x);
    
    uchar* c1 = m.ptr(y-1, x-1);
    uchar* c2 = m.ptr(y-1, x);
    uchar* c3 = m.ptr(y-1, x+1);
    uchar* c4 = m.ptr(y, x-1);   
    
    uchar* c6 = m.ptr(y, x+1);
    uchar* c7 = m.ptr(y+1, x-1);
    uchar* c8 = m.ptr(y+1, x);
    uchar* c9 = m.ptr(y+1, x+1);

    double l = (*c + *c1 + *c2 + *c3 + *c4 + *c6 + *c7 + *c8 + *c9) / 9;
    
    if (l < lum) { return 1; }
    else { return 0; }
}

int main() {
    // Create a VideoCapture object and open the input file
    // If the input is the web camera, pass 0 instead of the video file name
    //VideoCapture cap;
    VideoCapture cap(0);
    cap.set(CAP_PROP_FRAME_HEIGHT, ih);
    cap.set(CAP_PROP_FRAME_WIDTH, iw);
    //cap.open(0);
    // Check if camera opened successfully
    if (!cap.isOpened()) {
        cout << "Error opening video stream or file" << endl;
        return -1;
    }

    namedWindow("Frame", 1);


    int midhue = 100;
    int ranghue = 179;

   // createTrackbar("rangHue", "Frame", &ranghue, 179);
   // createTrackbar("midHue", "Frame", &midhue, 179);

    int midsat = 0;
    int rangsat = 255;

    //createTrackbar("rangsat", "Frame", &rangsat, 255);
    //createTrackbar("midsat", "Frame", &midsat, 255);

    int midlum = 50;
    int ranglum = 0;

    createTrackbar("ranglum", "Frame", &ranglum, 255);
    createTrackbar("midlum", "Frame", &midlum, 255);


    int sh = 179, ss = 128, sl = 128;
    //createTrackbar("H", "Frame", &sh, 179);
    //createTrackbar("S", "Frame", &ss, 128);
    //createTrackbar("L", "Frame", &sl, 128);

    long* xl = new long[ih * iw];
    long* yl = new long[ih * iw];
 
    while (1) {

        Mat frame;
        // Capture frame-by-frame
        cap.read(frame);
        Mat gray;
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        Mat hsv;
        Mat hsv1;
        Mat frame1;
        Mat frame2;
        equalizeHist(gray,frame1);
        cvtColor(frame1, hsv1, COLOR_GRAY2BGR);
        cvtColor(hsv1, hsv, COLOR_BGR2HSV);
        for (int x = 0; x < frame.cols; x++) {
            for (int y = 0; y < frame.rows; y++) {
                uchar* c = frame1.ptr(y, x);
                
                if (*c<midlum) {/* *(c2 + 0) = sh; *(c2 + 1) = ss; *(c2 + 2) = sl;*/ gr[y][x] = 1; }
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
        
        int mc=2,mcc=-1;
        int width = frame.cols; int height = frame.rows;
        for (int y = 0; y < height; y++) {
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

                        if ((y < height - 1) && gr[y+1][x]==1) {
                            gr[y+1][x] = mc;
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
                        bool failed = false;
                        while (from < to) {
                            int x = xl[from];
                            int y = yl[from];
                            from++;
                            if (minx == x || miny == y || maxx==x || maxy==y){failed = true; break;}

                            if ((y > miny) && gr[y - 1][x] !=mc && gr2[y - 1][x] != mc) {
                                gr2[y - 1][x] = mc;
                                xl[to] = x;
                                yl[to] = y - 1;
                                to++;
                                count2++;
                            }
                            if ((x > minx) && gr[y][x - 1] != mc && gr2[y][x - 1] != mc) {
                                gr2[y][x - 1] = mc;
                                xl[to] = x - 1;
                                yl[to] = y;
                                to++;
                                count2++;
                            }

                            if ((y < maxy) && gr[y + 1][x] != mc && gr2[y + 1][x] != mc) {
                                gr2[y + 1][x] = mc;
                                xl[to] = x;
                                yl[to] = y + 1;
                                to++;
                                count2++;
                            }
                            if ((x < maxx) && gr[y][x + 1] != mc && gr2[y][x + 1] != mc) {
                                gr2[y][x + 1] = mc;
                                xl[to] = x + 1;
                                yl[to] = y;
                                to++;
                                count2++;
                            }
                        }
                        if (failed == false) {
                            double k = ((double)count) / count2;
                            if (k > 0.2 && k < 0.7) {
                                mcc = mc;


                                double da = 2 * 3.141 / n;

                                int cnt = 0;
                                for (double a = 0; a < 2 * 3.141 - da / 2; a += da) {
                                    double t = tan(a);
                                    double ct = tan(3.141 / 2 - a);
                                    int fx = -1;
                                    int fy = -1;
                                    int lx = -1;
                                    int ly = -1;

                                    if ((a <= 3.141 / 4) || (a >= 7 * 3.141 / 4)) {
                                        for (int cx = avgx + 1; cx < width; cx++) {
                                            int cy = (int)round(avgy + t * (cx - avgx));
                                            if (cy < 0 || cy >= height) { break; }
                                            if (gr[cy][cx] == mc) {
                                                if (fx < 0) {
                                                    fx = cx;
                                                    fy = cy;
                                                }
                                                lx = cx;
                                                ly = cy;
                                            }
                                            gr[cy][cx] = 10e7;
                                        }
                                    }
                                    else if ((a >= 3 * 3.141 / 4) && (a <= 5 * 3.141 / 4)) {
                                        for (int cx = avgx - 1; cx > 0; cx--) {
                                            int cy = (int)round(avgy + t * (cx - avgx));
                                            if (cy < 0 || cy >= height) { break; }
                                            if (gr[cy][cx] == mc) {
                                                if (fx < 0) {
                                                    fx = cx;
                                                    fy = cy;
                                                }
                                                lx = cx;
                                                ly = cy;
                                            }
                                            gr[cy][cx] = 10e7;
                                        }
                                    }
                                    else if ((a >= 5 * 3.141 / 4) && (a <= 7 * 3.141 / 4)) {
                                        for (int cy = avgy - 1; cy > 0; cy--) {
                                            int cx = (int)round(avgx + ct * (cy - avgy));
                                            if (cx < 0 || cx >= width) { break; }
                                            if (gr[cy][cx] == mc) {
                                                if (fy < 0) {
                                                    fx = cx;
                                                    fy = cy;
                                                }
                                                lx = cx;
                                                ly = cy;
                                            }
                                            gr[cy][cx] = 10e7;
                                        }
                                    }
                                    else {
                                        for (int cy = avgy + 1; cy < height; cy++) {
                                            int cx = (int)round(avgx + ct * (cy - avgy));
                                            if (cx < 0 || cx >= width) { break; }
                                            if (gr[cy][cx] == mc) {
                                                if (fy < 0) {
                                                    fx = cx;
                                                    fy = cy;
                                                }
                                                lx = cx;
                                                ly = cy;
                                            }
                                            gr[cy][cx] = 10e7;
                                        }
                                    }
                                    if (fx >= 0) {
                                        P[cnt].c = 20e7;
                                        gr[(fy + ly) / 2][(fx + lx) / 2] = 20e7;
                                        P[cnt].y = (fy + ly) / 2.0;
                                        P[cnt].x = (fx + lx) / 2.0;
                                    }
                                    cnt++;

                                }
                                for (int i = 0; i < n; i++) {
                                    double Ax = P[i].x - P[(i - 1 + n) % n].x;
                                    double Ay = P[i].y - P[(i - 1 + n) % n].y;
                                    double Bx = P[(i + 1) % n].x - P[i].x;
                                    double By = P[(i + 1) % n].y - P[i].y;

                                    double Anx = Ax / (sqrt(Ax * Ax + Ay * Ay));
                                    double Any = Ay / (sqrt(Ax * Ax + Ay * Ay));
                                    double Bnx = Bx / (sqrt(Bx * Bx + By * By));
                                    double Bny = By / (sqrt(Bx * Bx + By * By));

                                    P[i].a = acos(Anx * Bnx + Any * Bny);
                                }
                                for (int i = 0; i < n; i++) {
                                    An[i] = (P[i].a + P[(i + 1) % n].a) / 2.0;
                                }

                                double maxA[4] = { 0,0,0,0 };
                                for (int i = 0; i < n; i++) {
                                    if (An[i] > maxA[0] && An[(i + 1) % n]) { maxA[0] = An[i]; maxAn[0] = i; }
                                }
                                for (int i = 0; i < n; i++) {
                                    if (i != maxAn[0] && i != maxAn[0] + 1 && i != maxAn[0] - 1 && An[i] > maxA[1]) { maxA[1] = An[i]; maxAn[1] = i; }
                                }
                                for (int i = 0; i < n; i++) {
                                    if (i != maxAn[1] && i != maxAn[1] + 1 && i != maxAn[1] - 1 && i != maxAn[0] && i != maxAn[0] + 1 && i != maxAn[0] - 1 && An[i] > maxA[2]) { maxA[2] = An[i]; maxAn[2] = i; }
                                }
                                for (int i = 0; i < n; i++) {
                                    if (i != maxAn[2] && i != maxAn[2] + 1 && i != maxAn[2] - 1 && i != maxAn[1] && i != maxAn[1] + 1 && i != maxAn[1] - 1 && i != maxAn[0] && i != maxAn[0] + 1 && i != maxAn[0] - 1 && An[i] > maxA[3]) { maxA[3] = An[i]; maxAn[3] = i; }
                                }

                                for (int i = 0; i < 4; i++) {
                                    if (maxAn[0] > maxAn[1]) { swap(maxAn[0], maxAn[1]); swap(maxA[0], maxA[1]); }
                                    if (maxAn[1] > maxAn[2]) { swap(maxAn[1], maxAn[2]); swap(maxA[1], maxA[2]); }
                                    if (maxAn[2] > maxAn[3]) { swap(maxAn[2], maxAn[3]); swap(maxA[2], maxA[3]); }
                                }

                                /*for (int i = 0; i < n; i++) {
                                    if (i > maxAn[0] && i <= maxAn[1]) { P[i].c = 10e7; gr[(int)P[i].y][(int)P[i].x] = P[i].c; MorePx(P[i].x, P[i].y, P[i].c); }//PINK  0 1
                                    if (i > maxAn[1] && i <= maxAn[2]) { P[i].c = 10e7; gr[(int)P[i].y][(int)P[i].x] = P[i].c; MorePx(P[i].x, P[i].y, P[i].c); }//BLACK 1 2
                                    if (i > maxAn[2] && i <= maxAn[3]) { P[i].c = 10e7; gr[(int)P[i].y][(int)P[i].x] = P[i].c; MorePx(P[i].x, P[i].y, P[i].c); }//BLACK 2 3
                                    if (i > maxAn[3] || i <= maxAn[0]) { P[i].c = 10e7; gr[(int)P[i].y][(int)P[i].x] = P[i].c; MorePx(P[i].x, P[i].y, P[i].c); }//RED   3 0
                                }*/


                                double Qx[4], Qy[4];
                                for (int i = 0; i < 4; i++) {

                                    int ip0 = (maxAn[i] + 2) % n;
                                    int ip1 = (maxAn[(i + 1) % 4] - 1) % n;

                                    int ip2 = (maxAn[(i + 1) % 4] + 2) % n;
                                    int ip3 = (maxAn[(i + 2) % 4] - 1) % n;


                                    double v1x = P[ip1].x - P[ip0].x;
                                    double v1y = P[ip1].y - P[ip0].y;

                                    double v2x = P[ip3].x - P[ip2].x;
                                    double v2y = P[ip3].y - P[ip2].y;


                                    double d = (v1x * v2y) - (v1y * v2x);
                                    double z = v2x * (P[ip0].y - P[ip2].y) - v2y * (P[ip0].x - P[ip2].x);
                                    if (fabs(d) > 0.00001) {
                                        double t = z / d;

                                        Qx[i] = P[ip0].x + t * v1x;
                                        Qy[i] = P[ip0].y + t * v1y;

                                        if ((Qx[i] >= 1) && (Qx[i] < width - 1) && (Qy[i] >= 1) && (Qy[i] < height - 1)) {
                                            //gr[(int)Qy[i]][(int)Qx[i]] = (i + 1) * 10e6; MorePx(Qx[i], Qy[i], (i + 1) * 10e6);
                                        }
                                    }
                                }

                                double D[4], d = (95 * iw * 0.35);
                                D[0] = (95 * iw * 0.35) / sqrt(((Qx[0] - Qx[1]) * (Qx[0] - Qx[1])) + ((Qy[0] - Qy[1]) * (Qy[0] - Qy[1])));
                                D[1] = (95 * iw * 0.35) / sqrt(((Qx[2] - Qx[3]) * (Qx[2] - Qx[3])) + ((Qy[2] - Qy[3]) * (Qy[2] - Qy[3])));
                                D[2] = sqrt(((Qx[3] - Qx[0]) * (Qx[3] - Qx[0])) + ((Qy[3] - Qy[0]) * (Qy[3] - Qy[0])));
                                D[3] = sqrt(((Qx[1] - Qx[2]) * (Qx[1] - Qx[2])) + ((Qy[1] - Qy[2]) * (Qy[1] - Qy[2])));

                                //cout << D[0] << " " << D[1] << " " << D[2] / D[3] << endl;
                                /*for (int i = 0; i < 16; i++) {
                                    cout << D[2] + ((0.75 + (i * 0.5) / 9.5) * (D[2] - D[3])) << endl;
                                }*/
///////////////////////////////////////////////////// MARKER READER                                
                                int h = 45, w = 95;
                                double x1 = Qx[0]; double y1 = Qy[0];
                                double x2 = Qx[3]; double y2 = Qy[3];
                                double x3 = Qx[1]; double y3 = Qy[1];
                                double x4 = Qx[2]; double y4 = Qy[2];
                                //cout << "Q0 " << Qx[0] << ", " << Qy[0] << endl;
                                //cout << "Q1 " << Qx[1] << ", " << Qy[1] << endl;
                                //cout << "Q2 " << Qx[2] << ", " << Qy[2] << endl;
                                //cout << "Q3 " << Qx[3] << ", " << Qy[3] << endl;

                                double t = (y2 - y1 + y3 - y4) / (x2 - x1 + x3 - x4);


                                double a23 = 1;
                                    double a13 = a23 * (h * y4 - h * y3 - h * x4 * t + h * x3 * t) / (w * x4 * t - w * x2 * t - w * y4 + w * y2);
                                    double Tz = (a13 * w * x4 + a23 * h * x4 - a13 * w * x2 - a23 * h * x3) / (x2 - x1 + x3 - x4);
                                    double Tx = Tz * x1 / d;
                                    double Ty = Tz * y1 / d;
                                    double a11 = (a13 * w * x2 + Tz * (x2 - x1)) / (w * d);
                                    double a12 = (a13 * w * y2 + Tz * (y2 - y1)) / (w * d);
                                    double a21 = (a23 * h * x3 + Tz * (x3 - x1)) / (h * d);
                                    double a22 = (a23 * h * y3 + Tz * (y3 - y1)) / (h * d);

                                    double distance = Tz;// +a13 * w / 2 + a23 * h / 2;
                                    //cout << "Distance: " << distance << endl;
                                    for (int y = 5; y >= 0; y--) {
                                        for (int x = 0; x < 16; x++) {
                                            double xt = (a11 * ((x * 5) + 10) + a21 * ((y * 5) + 10) + Tx) * d / (a13 * ((x * 5) + 10) + a23 * ((y * 5) + 10) + Tz);
                                            double yt = (a12 * ((x * 5) + 10) + a22 * ((y * 5) + 10) + Ty) * d / (a13 * ((x * 5) + 10) + a23 * ((y * 5) + 10) + Tz);

                                            if ((xt >= 1) && (xt < width - 1) && (yt >= 1) && (yt < height - 1)) {                                                
                                                gr[(int)yt][(int)xt] = 20e7; MorePx(xt, yt, 20e7);                                                
                                                
                                                Mark[y][x] = getM(frame1, midlum, xt, yt);                                               
                                            }
                                            cout << Mark[y][x] << " ";
                                        }
                                        cout << endl;
                                    }
                                    cout << endl;

////////////////////////////////////////////////////// MARKER READER
                            }
                        }
                    }
                    mc++;
                }
            }
        }
        for (int x = 0; x < frame.cols; x++) {
            for (int y = 0; y < frame.rows; y++) {
                uchar* c2 = hsv.ptr(y, x);
               
                if (gr[y][x] == mcc) { *(c2 + 0) = 50; *(c2 + 1) = 255; *(c2 + 2) = 128; }   //GREEN
                if (gr[y][x] == 10e7) { *(c2 + 0) = 128; *(c2 + 1) = 255; *(c2 + 2) = 255; } //BLUE
                if (gr[y][x] == 20e7) { *(c2 + 0) = 0; *(c2 + 1) = 0; *(c2 + 2) = 255; }     //WHITE

                if (gr[y][x] == 10e6) { *(c2 + 0) = 150; *(c2 + 1) = 255; *(c2 + 2) = 255; } //PINK
                if (gr[y][x] == 20e6) { *(c2 + 0) = 0; *(c2 + 1) = 0; *(c2 + 2) = 0; }       //BLACK
                if (gr[y][x] == 30e6) { *(c2 + 0) = 179; *(c2 + 1) = 255; *(c2 + 2) = 255; } //RED
                if (gr[y][x] == 40e6) { *(c2 + 0) = 30; *(c2 + 1) = 255; *(c2 + 2) = 255; }  //YELLOW
                //if (*(c2 + 2)> midlum) {    *(c2 + 2)= midlum; }
               //if ((*(c2 + 0) > 150) && (((*(c2 + 1) > 200)) || ((*(c2 + 1) < 110)))) { *(c + 2) = 0; *(c + 1) = 255; *(c) = 0; }
            }
        }
        
        cvtColor(hsv, frame, COLOR_HSV2BGR);
        cv::resize(frame, frame2, cv::Size(iw, ih));
        // If the frame is empty, break immediately
        if (frame.empty())
            break;

        // Display the resulting frame
        imshow("Frame", frame2);

        // Press  ESC on keyboard to exit
        char c = (char)waitKey(25);
        if (c == 27) {
            break;
        }
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