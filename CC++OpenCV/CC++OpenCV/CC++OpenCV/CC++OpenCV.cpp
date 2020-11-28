// ProjektZaliczenie.cpp: Okreœla punkt wejœcia dla aplikacji konsoli.
//
#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include <opencv2\opencv.hpp>

using namespace cv;
using namespace std;



int wait(int time = 0)
{
	int zn = waitKey(time);
	if (zn == 'q') exit(0);
	return zn;
}

void kolor(Mat img, int sr, int d) {

}

void pomiar(const vector<vector<Point>> &cont,
	const vector <Vec4i> &hierarchy,
	Mat gray, Mat img)
{
	double sr;
	int a = 0, b = 0, c = 0, e = 0;
	for (int i = 0; i < hierarchy.size(); ++i)
	{
		Mat gr = gray;
		if (hierarchy[i][3] < 0)
		{
			int dziur = 0;
			for (int j = 0; j < hierarchy.size(); ++j)
			{
				if (hierarchy[j][3] == i) dziur++;
			}

			double pow = contourArea(cont[i]);
			double len = arcLength(cont[i], true);
			double wsp = len * len / pow;
			double d1, d2, d3;
			if (dziur == 1)
				if (wsp < 14.5) {
					Mat tmp = gray.clone();
					Point2f p[4];
					RotatedRect rect = minAreaRect(cont[i]);//findelipse
					rect.points(p);
					for (int j = 0; j < 4; ++j)
					{
						line(tmp, p[j], p[(j + 1) % 4], 255, 2);

					}

					int o = (norm(p[1] - p[0]) > norm(p[2] - p[1])) ? 1 : 0;
					Point2f delta = 0.5*(p[1 + o] - p[0 + o]);
					Point ps, pe;
					ps = p[1 + o] - delta;
					pe = p[2 + o] - delta;

					line(tmp, (p[1 + o] - delta), (p[2 + o] - delta), 0, 2);

					LineIterator li(gr, ps, pe);
					int old_jasnosc = -1;
					Point old_point;
					Point poczatek, koniec;
					for (int k = 0; k < li.count; ++k, ++li)
					{
						if (li.ptr[0] > 250 && (old_jasnosc < 220 || old_jasnosc == -1))
						{
							circle(tmp, li.pos(), 5, 0, 3);
							old_jasnosc = li.ptr[0];
							double dist = norm(li.pos() - old_point);
							//printf("Odleglosc1: %2f\n", dist);
							d1 = dist;
							old_point = li.pos();
							poczatek = li.pos();
						}
						else if ((old_jasnosc > 250 || old_jasnosc == -1) && li.ptr[0] < 220)
						{
							circle(tmp, li.pos(), 5, 0, 3);
							old_jasnosc = li.ptr[0];
							double dist = norm(li.pos() - old_point);
							//printf("Odleglosc2: %2f\n", dist);
							d2 = dist;
							old_point = li.pos();

						}
					}
					vector<Vec3f> circles;
					if (gr.at<uchar>(pe.y, pe.x) < 500)
					{
						double dist = norm(pe - old_point);
						//if (dist > 0) printf("Odleglosc3: %2f\n", dist);
						d3 = dist;
						circle(tmp, pe, 5, 0, 3);
						sr = d1 + d2 + d3;
						printf("srednica %f\n", sr);
						koniec = pe;
					}
					double y = poczatek.y + ((poczatek.y - koniec.y) / 2);
					double x = poczatek.x + ((poczatek.x - koniec.x) / 2);
					Point2d srodek(x*7.8, y*7.8);
					if (sr <= 145) {
						a++;
						circle(img, srodek, sr * 3, Scalar(0, 0, 255), 3, 1, 3);
						imshow("test", img);

					}
					else if (sr > 145 && sr <= 175) {
						b++;
						circle(img, srodek, sr * 3, Scalar(0, 255, 0), 3, 1, 3);
						imshow("test", img);
					}
					else if (sr > 175) {
						if (d1 > 95) {
							c++;
							circle(img, srodek, sr * 3, Scalar(255, 0, 0), 3, 1, 3);
							imshow("test", img);
						}
						else {
							e++;
							circle(img, srodek, sr * 3, Scalar(120, 50, 200), 3, 1, 3);
							imshow("test", img);
						}
					}
					imshow("Test3", tmp);
					wait(0);
				}
		}
	}
	printf("jest %d podkladek malych \n jest %d podkladek srednich \n jest %d podkladek duzych \n jest %d podkladek duzych z mniejszym otworem \n", a, b, c, e);
}



void segmentacja(Mat img)
{
	Mat gray;
	cvtColor(img, gray, COLOR_BGR2GRAY);
	threshold(gray, gray, 160, 255, THRESH_BINARY_INV);
	vector<vector<Point>> contours;
	vector <Vec4i> hierarchy;
	findContours(gray.clone(), contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE);
	pomiar(contours, hierarchy, gray, img);

}

int _tmain(int arg, _TCHAR* argv[])
{
	Mat kernel = getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	Mat img;
	char path[_MAX_PATH];
	for (int i = 0; i <= 9; ++i)
	{
		sprintf(path, "F:\\kody\\CC++OpenCV\\%d.jpg", i);
		Mat img = imread(path);
		Mat gray;
		cvtColor(img, gray, COLOR_BGR2GRAY);
		morphologyEx(img, img, MORPH_CLOSE, kernel);
		imshow("test", img);
		segmentacja(img);
		wait();
	}
	return 0;
}

