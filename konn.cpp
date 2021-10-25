void computeContours(cv::Mat& Image, cv::Mat Background, cv::Mat& result, std::vector<cv::Point>& coordinates)
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
            if (contours.at<uchar>(i, j) > 0) {
                coordinates.push_back(cv::Point(i, j));
            }
        }
    }
}


int main() {
    //C:/Users/Konn/Downloads/volvo440.jpg
    std::string kepBelovas;

    std::cout << "Add meg a kep eleresi utvonalat: ";
    std::cin >> kepBelovas;

    int thresh_value;
    std::cout << "Kuszobertek:\n";
    std::cin >> thresh_value;

    int vilagositas;
    std::cout << "Vilagositasi ertek: \n";
    std::cin >> vilagositas;

    cv::Mat img = cv::imread(kepBelovas, 1);
    cv::Mat result;

    /*---2.feladat-----*/
    cv::Mat hsv, splitted[3], merged;
    int channelNumber = 2;
    cv::cvtColor(img, hsv, cv::COLOR_BGR2HSV);
    cv::split(hsv, splitted);
    cv::equalizeHist(splitted[channelNumber], splitted[channelNumber]);
    cv::merge(splitted, 3, merged);
    cv::cvtColor(merged, result, cv::COLOR_HSV2BGR);
    
    cv::Mat blurred;
    cv::medianBlur(result, blurred, 3);

    //elesites
    cv::addWeighted(img, 1.5, blurred, -0.5, 0, result);


    /*----3.feladat----*/

    img += cv::Scalar(vilagositas, vilagositas, vilagositas);


    /*-----4.feladat-------*/


    cv::Mat bw, mask, tripleMask[3], finalMask;

    cv::cvtColor(img, bw, cv::COLOR_RGB2GRAY);
    cv::threshold(bw, mask, thresh_value, 255, cv::THRESH_BINARY);

    for (int i = 0; i < 3; ++i)
        mask.copyTo(tripleMask[i]);

    cv::merge(tripleMask, 3, finalMask);
    cv::bitwise_and(img, finalMask, result);
;

    /*--------5.feladat-------------*/

    std::string path_save;
    std::cout << "Mentesi fajlnev eleressel:\n";
    std::cin >> path_save;

    std::vector<int> formatParameters_jpg;

    formatParameters_jpg.push_back(cv::IMWRITE_JPEG_QUALITY);
    formatParameters_jpg.push_back(90);
    cv::imwrite(path_save, result, formatParameters_jpg);

    /*--------6.feladat-----------*/

    int saturation_value = 100;
    int brightness_value = 100;
    int hue_value;
    std::vector<cv::Point> coordinates;

    cv::Mat background,Img;

    std::cout << "Hue erteke: ";
    std::cin >> hue_value;

    background.create(img.size(), CV_8UC3);
    background.setTo(cv::Scalar(hue_value, saturation_value, brightness_value));
    cv::cvtColor(background, background, cv::COLOR_HSV2BGR);

    /*---------7.feladat-----------*/

    computeContours(result, background, Img, coordinates);

    /*-------8.feladat---------------*/
    for (int i = 0; i < coordinates.size(); ++i) {
        float needClearPoint = (float)rand() / RAND_MAX;
        if (needClearPoint <= 0.65) {
            int row = coordinates.at(i).x;
            int col = coordinates.at(i).y;

            Img.at<cv::Vec3b>(row, col) = cv::Vec3b();
        }
    }

    cv::namedWindow("Img");
    cv::moveWindow("Img", 40, 30);
    cv::imshow("Img", Img);
    cv::waitKey();


}
