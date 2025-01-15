#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    // Load the image
    cv::Mat image = cv::imread("TN_BORDER.png", cv::IMREAD_COLOR);
    
    // Check if the image was loaded successfully
    if (image.empty()) {
        std::cerr << "Error: Could not load image!" << std::endl;
        return -1;
    }

    std::cout << "Original Image Size: " << image.rows << "x" << image.cols << std::endl;

    // Remove the 0th y-axis (first row)
    cv::Mat modifiedImage = image(cv::Range(10, image.rows-17), cv::Range(10, image.cols-94));

    std::cout << "Modified Image Size: " << modifiedImage.rows << "x" << modifiedImage.cols << std::endl;

    // Save or display the result
    // cv::imshow("Modified Image", modifiedImage);
    // cv::waitKey(0);

    // Optionally save the modified image
    cv::imwrite("tn.png", modifiedImage);

    return 0;
}
