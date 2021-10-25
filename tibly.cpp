#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>
#include <string>


//megnyitjuk azt a kepet, amit a felhasznalo megadott
void LoadImg(std::string& path, cv::Mat& Image)
{
	//szines modban olvassuk be
	Image = cv::imread(path, 1);
	//cv::imshow("I", Image);
}

void ImgEnhancement(cv::Mat& Image,cv::Mat& result)
{
	//-----------hisztogram kiegyenlites-------------------
	//a muveletet legegyszerubb hsv szinmodellben elvegezni, 
	//ehhez eloszor atkonvertaljuk, majd szetszedve a csatornakat
	//kulon csak a vilagossagi csatornara alkalmazzuk.
	//A tobbi csatornat valtozatlanul hagyjuk
	cv::Mat hsv, splitted[3], merged;
	int channelNumber = 2;
	cv::cvtColor(Image, hsv, cv::COLOR_BGR2HSV);
	cv::split(hsv, splitted);
	cv::equalizeHist(splitted[channelNumber], splitted[channelNumber]);
	cv::merge(splitted, 3, merged);
	cv::cvtColor(merged, result, cv::COLOR_HSV2BGR);

	//zajszures 3x3-as median szurovel.
	cv::Mat blurred;
	cv::medianBlur(result, blurred, 3);
	
	//elesites
	cv::addWeighted(Image, 1.5, blurred, -0.5, 0, result);
}

//Vilagositas
void performExposureShift(cv::Mat& Image, int value)
{
	//Leggyorsabban rgb-ben lehet megoldani, ha minden csatornahoz ugyanazt adjuk hozza
	//A cv::Scalar-ral letrehozunk egy haromelemu tombot vagy vectort, ami a megfelelo csatornakhoz hozzadja az ertekeket.
	Image += cv::Scalar(value,value,value);
}

void performThreshold(cv::Mat& Image, cv::Mat& result, unsigned int thresh_value)
{
	cv::Mat bw,mask,tripleMask[3],finalMask;

	cv::cvtColor(Image, bw, cv::COLOR_RGB2GRAY);
	cv::threshold(bw, mask, thresh_value, 255, cv::THRESH_BINARY);

	//hogy mukodjon a 3 csatornas kepre, ezert a sima maszkunkat mindharom csatornahoz elkeszitjuk!
	for (int i = 0; i < 3; ++i)
		mask.copyTo(tripleMask[i]);

	//itt osszevonjuk egy 3 csatornas kepbe
	cv::merge(tripleMask, 3, finalMask);

	//mindig csak megegyezo dimenzioju matrixokat lehet logika es-sel hasznalni!
	cv::bitwise_and(Image, finalMask, result);
}

void writeImgToFile(cv::Mat& Image, std::string path)
{
	std::vector<int> formatParameters_jpg;

	formatParameters_jpg.push_back(cv::IMWRITE_JPEG_QUALITY);
	formatParameters_jpg.push_back(90);
	cv::imwrite(path, Image, formatParameters_jpg);
}

void createImg(cv::Mat& Image,cv::Size shape)
{
	int hue_value = 0;
	int saturation_value = 100;
	int brightness_value = 100;

	std::cout << "Hue erteke: ";
	std::cin >> hue_value;

	Image.create(shape, CV_8UC3);
	Image.setTo(cv::Scalar(hue_value, saturation_value, brightness_value));
	cv::cvtColor(Image, Image, cv::COLOR_HSV2BGR);
}

void computeContours(cv::Mat& Image, cv::Mat Background,cv::Mat& result,std::vector<cv::Point>& coordinates)
{
	cv::Mat bw;
	cv::Mat kernel = cv::Mat::ones(cv::Size(5, 5), CV_8UC1);
	cv::Mat dilated, eroded;

	cv::cvtColor(Image, bw, cv::COLOR_RGB2GRAY);

	cv::dilate(bw, dilated, kernel);
	cv::erode(bw, eroded, kernel);

	cv::Mat contours = dilated - eroded;
	cv::Mat maskContours[3], finalContours;

	for (int i = 0; i < 3; ++i)
		contours.copyTo(maskContours[i]);

	cv::merge(maskContours, 3, finalContours);
	cv::add(Background, finalContours, result);

	for (int i = 0; i < contours.rows; ++i) {
		for (int j = 0; j < contours.cols; ++j) {
			if (contours.at<uchar>(i, j)> 0) {
				coordinates.push_back(cv::Point(i, j));
			}
		}
	}
}

void clearRandomContourPoints(cv::Mat& Image,std::vector<cv::Point>& points)
{
	for (int i = 0; i < points.size(); ++i) {
		float needClearPoint = (float)rand() / RAND_MAX;
		if (needClearPoint <= 0.7) {
			int row = points.at(i).x;
			int col = points.at(i).y;

			Image.at<cv::Vec3b>(row, col) = cv::Vec3b();
		}
	}
}

int main()
{
	//-------------------------------------------------
	cv::Mat Image,improved,result;
	std::string path,path_save;
	int brightness = 0;
	int thresh_value = 0;

	std::cout << " A kep eleresi utvonala:\n";
	std::cin >> path;
	LoadImg(path,Image);
	ImgEnhancement(Image, improved);

	std::cout << "Vilagositasi ertek: \n";
	std::cin >> brightness;
	performExposureShift(improved, brightness);

	std::cout << "Kuszobertek:\n";
	std::cin >> thresh_value;
	performThreshold(improved, result,thresh_value);
	
	std::cout << "Mentesi fajlnev eleressel:\n";
	std::cin >> path_save;
	writeImgToFile(result, path_save);

	cv::namedWindow("result");
	cv::moveWindow("result", 40, 30);
	cv::imshow("result",result);
	cv::waitKey();
	//-------------------------------------------------------

	cv::Mat background, Img;
	std::vector<cv::Point> coordinates;
	createImg(background,Image.size());
	computeContours(result, background, Img,coordinates);
	clearRandomContourPoints(Img, coordinates);

	cv::namedWindow("Img");
	cv::moveWindow("Img", 40, 30);
	cv::imshow("Img", Img);
	cv::waitKey();

	return 0;
}
