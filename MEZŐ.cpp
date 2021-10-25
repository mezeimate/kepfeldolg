#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <iomanip>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;

int main() {
	//bekeres, alapok
	string forras;
	string mentes;
	int brightness = 0;
	int kuszob = 0;
	int huev = 0;

	cout << "\nAhonnan a kepet olvassam be: "; 
	cin >> forras;

	cout << "\nVilagositasi ertek: ";
	cin >> brightness;

	cout << "\Kuszob ertek: ";
	cin >> kuszob;

	cout << "\HUE ertek: ";
	cin >> huev;

	cout << "\Mentesi fajlnev eleressel: ";
	cin >> mentes;

	Mat img = imread(forras, 1);
	imshow("original", img);
	

	//kontraszt 
	Mat knt;
	Mat hsv, splitted[3], mrg;
	cvtColor(img, hsv, COLOR_RGB2HSV);
	split(hsv, splitted);
	equalizeHist(splitted[2], splitted[2]);
	merge(splitted, 3, mrg);
	cvtColor(mrg, knt, COLOR_HSV2RGB);


	//zajszures
	Mat filtered;
	medianBlur(knt, filtered, 3);
	//imshow("filtered", filtered);


	//elesit
	addWeighted(img, 1.5, filtered, -0.5, 0, knt);
	//imshow("sharpened", sharpened);
	imshow("kont", knt);


	//vilagositas
	Mat vilagos;
	img.copyTo(vilagos);
	vilagos += Scalar(brightness, brightness, brightness);


	//Tresh
	Mat treshresult;
	Mat bw, mask, tripleMask[3], finalMask;
	cvtColor(img, bw, COLOR_RGB2GRAY);
	threshold(bw, mask, kuszob, 255, THRESH_BINARY); //maszk

		//hogy mukodjon a 3 csatornas kepre, ezert a sima maszkunkat mindharom csatornahoz elkeszitjuk!
	for (int i = 0; i < 3; ++i) {
		mask.copyTo(tripleMask[i]);
	}
		//itt osszevonjuk egy 3 csatornas kepbe
	merge(tripleMask, 3, finalMask);
		//mindig csak megegyezo dimenzioju matrixokat lehet logika es-sel hasznalni!
	bitwise_and(img, finalMask, treshresult);


	//92%os aranyba menti
	vector<int> formatP_jpg;
	formatP_jpg.push_back(IMWRITE_JPEG_QUALITY);
	formatP_jpg.push_back(92); // százalék
	imwrite(mentes, treshresult, formatP_jpg);


	// Harmadik képet hoz létre
	int saturation_value = 100;
	int brightness_value = 100;
	vector<Point> coordinates;
	Mat background, Img;
	background.create(img.size(), CV_8UC3);
	background.setTo(Scalar(huev, saturation_value, brightness_value));
	cvtColor(background, background, COLOR_HSV2BGR);

	//

	waitKey();
	return 0;
}
