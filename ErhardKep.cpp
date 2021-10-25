#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>
#include <string>

void LoadImage(cv::Mat& image, std::string& path, bool isColored = 1) {

	image = cv::imread(path, isColored);
}

void ShowImage(cv::Mat& image, std::string name = "Image") {

	cv::imshow(name, image);
}

// Ha nem 0, akkor a filterSize páratlan kell, hogy legyen.

void FilterImage(cv::Mat& image, cv::Mat& new_image, int filterMode, int filterSize = 5, int sigmaSize = 5) {

	switch (filterMode) {

		case 0:
			cv::boxFilter(image, new_image, -1, cv::Size(filterSize, filterSize));
			break;
		case 1:
			cv::GaussianBlur(image, new_image, cv::Size(filterSize, filterSize), sigmaSize);
			break;
		default:
			cv::medianBlur(image, new_image, filterSize);
	}
}

void WeightImage(cv::Mat& image, cv::Mat& blurred_image, cv::Mat& new_image) {

	cv::addWeighted(image, 1.5, blurred_image, -0.5, 0, new_image);
}

void EqualizeImage(cv::Mat& image, cv::Mat& new_image) {

	cv::Mat hsv_image, splitted[3];
	cv::cvtColor(image, hsv_image, cv::COLOR_RGB2HSV);
	cv::split(hsv_image, splitted);
	cv::equalizeHist(splitted[2], splitted[2]);
	cv::merge(splitted, 3, hsv_image);
	cv::cvtColor(hsv_image, new_image, cv::COLOR_HSV2RGB);
}

void ExposureImage(cv::Mat& image, cv::Mat& new_image, int exposureSize = 10) {

	image.copyTo(new_image);
	new_image += cv::Scalar(exposureSize, exposureSize, exposureSize);
}

void ThresholdImage(cv::Mat& image, cv::Mat& new_image, unsigned int thresholdSize = 10) {

	cv::Mat gray_image, mask_image, triple_mask[3];
	cv::cvtColor(image, gray_image, cv::COLOR_RGB2GRAY);
	cv::threshold(gray_image, mask_image, thresholdSize, 255, cv::THRESH_BINARY);

	for (int i = 0; i < 3; i++)
		mask_image.copyTo(triple_mask[i]);

	cv::merge(triple_mask, 3, mask_image);
	cv::bitwise_and(image, mask_image, new_image);
}

void SaveImage(cv::Mat& image, std::string& path, cv::ImwriteFlags flgas, int quality) {

	std::vector<int> parameters;
	parameters.push_back(flgas);
	parameters.push_back(quality);
	cv::imwrite(path, image, parameters);
}

void CreateImage(cv::Mat& image, cv::Size shape, unsigned int hue, unsigned int saturation, unsigned int value) {

	image.create(shape, CV_8UC3);
	image.setTo(cv::Scalar(hue, saturation, value));
	cv::cvtColor(image, image, cv::COLOR_HSV2RGB);
}

void ComputeContours(cv::Mat& image, cv::Mat& background_image, cv::Mat& new_image, std::vector<cv::Point>& coordinates) {

	cv::Mat gray_image;
	cv::cvtColor(image, gray_image, cv::COLOR_RGB2GRAY);

	cv::Mat kernel;
	kernel = cv::Mat::ones(cv::Size(5, 5), CV_8UC1);

	cv::Mat dilated;
	cv::dilate(gray_image, dilated, kernel);

	cv::Mat eroded;
	cv::erode(gray_image, eroded, kernel);

	cv::Mat contours, triple_contours[3];
	contours = dilated - eroded;

	for (int i = 0; i < 3; i++)
		contours.copyTo(triple_contours[i]);

	cv::merge(triple_contours, 3, contours);
	cv::add(background_image, contours, new_image);

	for (int i = 0; i < contours.rows; i++) {

		for (int j = 0; j < contours.cols; j++) {

			if (contours.at<uchar>(i, j) > 0)
				coordinates.push_back(cv::Point(i, j));
		}
	}
}

void ClearRandomContourPoints(cv::Mat& image, std::vector<cv::Point>& points) {

	for (int i = 0; i < points.size(); i++) {

		float needClearPoint = (float)rand() / RAND_MAX;

		if (needClearPoint <= 0.35) {

			int row = points.at(i).x;
			int col = points.at(i).y;

			image.at<cv::Vec3b>(row, col) = cv::Vec3b();
		}
	}
}

int main() {

	std::string path; // D://me.png
	std::cout << "Adja meg az Utvonalat: " << std::endl;
	std::cin >> path;

	cv::Mat image;
	LoadImage(image, path);
	ShowImage(image, "Original Image");

	cv::Mat filtered_image;
	FilterImage(image, filtered_image, 2, 15);
	// ShowImage(filtered_image, "Filtered Image");

	cv::Mat weighted_image;
	WeightImage(image, filtered_image, weighted_image);
	// ShowImage(weighted_image, "Weighted Image");

	cv::Mat equalized_image;
	EqualizeImage(image, equalized_image);
	// ShowImage(equalized_image, "Equalized Image");

	cv::Mat exposured_image;
	ExposureImage(image, exposured_image, 50);
	// ShowImage(exposured_image, "Exposured Image");

	cv::Mat thresholded_image;
	ThresholdImage(image, thresholded_image, 25);
	// ShowImage(thresholded_image, "Thresholded Image");

	// std::cout << "Adja meg az Utvonalat: " << std::endl;
	// std::cin >> path;
	// SaveImage(thresholded_image, path, cv::IMWRITE_JPEG_QUALITY, 92);

	cv::Mat background_image;
	CreateImage(background_image, image.size(), 90, 100, 100);
	// ShowImage(background_image, "Background Image");

	cv::Mat contour_image;
	std::vector<cv::Point> coordinates;
	ComputeContours(image, background_image, contour_image, coordinates);
	// ShowImage(contour_image, "Contour Image");

	ClearRandomContourPoints(contour_image, coordinates);
	// ShowImage(contour_image, "Contour Image 2");

	cv::waitKey();

	return 0;
}
