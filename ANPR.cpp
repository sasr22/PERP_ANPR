#include <iostream>
#include <math.h>
#include <string>
#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"

int main() {
    cv::Mat originalFrame;
    cv::Mat frame;
    cv::Mat frame2;

    cv::Mat1b img(5000, 5000);
    cv::randu(img, cv::Scalar(0), cv::Scalar(256));
    img = img > 127;

    cv::Point2f vtx[4];

    cv::RotatedRect box;

    std::vector<std::vector<cv::Point>> contours;

    cv::Mat structureElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(16, 4));
    cv::VideoCapture cap("rtsp://localhost:9768/live", cv::CAP_FFMPEG);

    if (!cap.isOpened()) {
        std::cout << "Couldn't find stream!" << std::endl;
        return -1;
    }

    std::cout << "Found stream!" << std::endl;

    for (;;) {
        cap >> originalFrame;

        if (originalFrame.empty()) {
            std::cout << "Grabbed empty frame!" << std::endl;
            return -1;
        }

        cv::cvtColor(originalFrame, frame2, cv::COLOR_BGR2GRAY);

        cv::GaussianBlur(frame2, frame, cv::Size(5, 5), 0);

        cv::Mat grad_x, grad_y;
        cv::Mat abs_grad_x, abs_grad_y;
        cv::Sobel(frame, grad_x, CV_16S, 1, 0, 1, 1, 0, cv::BORDER_DEFAULT);
        cv::Sobel(frame, grad_y, CV_16S, 0, 1, 1, 1, 0, cv::BORDER_DEFAULT);

        cv::convertScaleAbs(grad_x, abs_grad_x);
        cv::convertScaleAbs(grad_y, abs_grad_y);

        cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, frame);

        cv::threshold(frame, frame2, 1, 128, cv::THRESH_OTSU);

        cv::morphologyEx(frame2, frame, cv::MORPH_CLOSE, structureElement);

        cv::findContours(frame, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

        for (size_t i = 0; i < contours.size(); i++) {
            box = cv::minAreaRect(contours[i]);
            box.points(vtx);

            unsigned int size = box.boundingRect().height * box.boundingRect().width;

            if (1'000 < size && size < 20'000 && box.angle < 45) {
                try {
                    cv::Rect roi = box.boundingRect();

                    cv::Mat1b sub_img = img(roi);

                    cv::Mat1b mask(roi.size(), uchar(0));
                    std::vector<cv::Point> points_in_sub_image(4);
                    for (int i = 0; i < 4; ++i) {
                        points_in_sub_image[i] = cv::Point(vtx[i]) - roi.tl();
                    }
                    cv::fillConvexPoly(mask, points_in_sub_image, cv::Scalar(255));

                    cv::Mat1b inside_roi = sub_img & mask;

                    if (0.5 > cv::countNonZero(inside_roi) / inside_roi.total()) {
                        cv::line(originalFrame, vtx[0], vtx[1], cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
                        cv::line(originalFrame, vtx[1], vtx[2], cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
                        cv::line(originalFrame, vtx[2], vtx[3], cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
                        cv::line(originalFrame, vtx[3], vtx[0], cv::Scalar(0, 0, 255), 1, cv::LINE_AA);

                        cv::imwrite(std::string("img/hit_") + std::to_string(i) + std::string(".png"), originalFrame(box.boundingRect()));
                    }
                } catch (const std::exception& e) {
                }
            }
        }
        cv::imshow("ANPR", originalFrame);
        cv::waitKey(1);
    }

    cap.release();
    cv::destroyAllWindows();

    return 0;
}