#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <opencv2/opencv.hpp>
#include <stack>
#include <filesystem>

namespace py = pybind11;
namespace fs = std::filesystem;

std::string tiles = "/root/tiles/";

cv::Mat whiteImage(256, 256, CV_8UC3, cv::Scalar(255, 255, 255));

cv::Mat load_img(int x, int y)
{
    try
    {
        cv::Mat img = cv::imread(tiles + std::to_string(x) + "/" + std::to_string(y) + ".png", cv::IMREAD_UNCHANGED);
        if (img.empty())
        {
            return whiteImage;
        }
        else
            return img;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return whiteImage;
    }
}

class image_server
{
public:
    image_server(int x, int y)
    {
        realX = x;
        realY = y;
    }

    cv::Vec3b get_pix(int pixX, int pixY)
    {
        if (pixX < 0)
            if (pixY < 0)
            {
                static cv::Mat top_left = load_img(realX - 1, realY - 1);
                return top_left.at<cv::Vec3b>(256 + pixY, 256 + pixX);
            }
            else if (pixY > 255)
            {
                static cv::Mat btm_left = load_img(realX - 1, realY + 1);
                return btm_left.at<cv::Vec3b>(256 - pixY, 256 + pixX);
            }
            else
            {
                static cv::Mat cnt_left = load_img(realX - 1, realY);
                return cnt_left.at<cv::Vec3b>(pixY, 256 + pixX);
            }
        else if (pixX > 255)
            if (pixY < 0)
            {
                static cv::Mat top_right = load_img(realX + 1, realY - 1);
                return top_right.at<cv::Vec3b>(256 + pixY, 256 - pixX);
            }
            else if (pixY > 255)
            {
                static cv::Mat btm_right = load_img(realX + 1, realY + 1);
                return btm_right.at<cv::Vec3b>(256 - pixY, 256 - pixX);
            }
            else
            {
                static cv::Mat cnt_right = load_img(realX + 1, realY);
                return cnt_right.at<cv::Vec3b>(pixY, 256 - pixX);
            }
        else if (pixY < 0)
        {
            static cv::Mat top = load_img(realX, realY - 1);
            return top.at<cv::Vec3b>(256 + pixY, pixX);
        }
        else if (pixY > 255)
        {
            static cv::Mat btm = load_img(realX, realY + 1);
            return btm.at<cv::Vec3b>(256 - pixY, pixX);
        }

        return cv::Vec3b(255, 255, 255);
    }
    int realX;
    int realY;

private:
    // cv::Mat top_left;
    // cv::Mat top;
    // cv::Mat top_right;
    // cv::Mat cnt_left;
    // cv::Mat cnt_right;
    // cv::Mat btm_right;
    // cv::Mat btm;
    // cv::Mat btm_left;
};

bool isSimilarToPurple(const cv::Vec3b &pixel)
{
    int blue = pixel[0];
    int green = pixel[1];
    int red = pixel[2];

    // Define the range for purple (you may need to tweak these values based on your needs)
    int lower_bound = 50;  // Lower bound for blue and red
    int upper_bound = 255; // Upper bound for blue and red
    int green_max = 50;    // Max value for green to avoid green dominance

    return (blue >= lower_bound && blue <= upper_bound) &&
           (red >= lower_bound && red <= upper_bound) &&
           (green <= green_max);
}

bool verify_pix(int x, int y, std::vector<cv::Point> &visited, cv::Mat &img, image_server &ser)
{
    cv::Point p_ = cv::Point(x, y);
    for (int i = 0; i < static_cast<int>(visited.size()); i++)
    {
        if (visited[i] == p_)
            return false;
    }

    cv::Vec3b pixel;

    // if (x < 0 || x > 255 || y < 0 || y > 255)
    // {
    //     std::cout << "getting: " << x << " " << y << std::endl;
    //     pixel = ser.get_pix(x, y);
    // }
    // else
    // {
    pixel = img.at<cv::Vec3b>(y, x);
    // }

    if (isSimilarToPurple(pixel))
    {
        return true;
    }
    return false;
}

int get_teritory(int seed_x, int seed_y, cv::Mat &img, image_server &ser)
{
    std::stack<std::pair<int, int>> to_visit;
    std::vector<cv::Point> visited;
    to_visit.push({seed_x, seed_y});
    visited.push_back(cv::Point(seed_x, seed_y));

    while (!to_visit.empty())
    {
        std::pair<int, int> current = to_visit.top();
        to_visit.pop();
        int cx = current.first;
        int cy = current.second;
        visited.push_back(cv::Point(cx, cy));

        if (verify_pix(cx - 1, cy, visited, img, ser))
        {
            to_visit.push({cx - 1, cy});
        }
        if (verify_pix(cx, cy + 1, visited, img, ser))
        {
            to_visit.push({cx, cy + 1});
        }
        if (verify_pix(cx, cy - 1, visited, img, ser))
        {
            to_visit.push({cx, cy - 1});
        }
        if (verify_pix(cx + 1, cy, visited, img, ser))
        {
            to_visit.push({cx + 1, cy});
            to_visit.push({cx + 2, cy});
            to_visit.push({cx + 3, cy});
        }
    }

    cv::Rect boundingBox = cv::boundingRect(visited);
    cv::Mat blankImage = cv::Mat::zeros(boundingBox.size(), CV_8UC3);

    for (int y = boundingBox.y; y < boundingBox.y + boundingBox.height; ++y)
    {
        for (int x = boundingBox.x; x < boundingBox.x + boundingBox.width; ++x)
        {
            blankImage.at<cv::Vec3b>(y - boundingBox.y, x - boundingBox.x) = img.at<cv::Vec3b>(y, x);
            img.at<cv::Vec3b>(y, x) = cv::Vec3b(255, 255, 255);
        }
    }

    // int data_x = boundingBox.x;
    // int data_y = boundingBox.y;
    // int root_x = seed_x;
    // int root_y = seed_y;

    int num_x = boundingBox.x;
    int num_y = boundingBox.y;

    if (boundingBox.x < 0)
    {
        num_x = 256 + boundingBox.x;
        ser.realX--;
    }
    else if (boundingBox.y < 0)
    {
        num_y = 256 + boundingBox.y;
        ser.realY--;
    }

    std::string path = "/root/nodes/"+std::to_string(ser.realX) + "/" + std::to_string(ser.realY);

    if (!fs::exists(path))
    {
        fs::create_directories(path);
    }

    cv::imwrite(path + "/i" + std::to_string(num_x) + "_" + std::to_string(num_y) + ".bmp", blankImage);

    return boundingBox.x + boundingBox.width;
}

void process(const py::array_t<uint8_t> &input_image, int tlX, int tlY)
{
    std::vector<int> line_ind;
    py::buffer_info buf = input_image.request();
    cv::Mat image(buf.shape[0], buf.shape[1], CV_8UC3, (uint8_t *)buf.ptr);

    image_server ser(tlX, tlY);

    std::string res;

    for (int y = 0; y < 256; y = y + 8)
    {
        for (int x = 0; x < 256; x++)
        {
            if (isSimilarToPurple(image.at<cv::Vec3b>(y, x)))
            {
                x = get_teritory(x, y, image, ser);
            }
        }
    }

    if (!fs::exists("/root/bnk_tiles/"+std::to_string(tlX)))
    {
        fs::create_directories("/root/bnk_tiles/"+std::to_string(tlX));
    }
    cv::imwrite("/root/bnk_tiles/"+std::to_string(tlX)+"/"+std::to_string(tlY)+".png",image);
}

PYBIND11_MODULE(pullNumbers, m)
{
    m.doc() = "Image processing module using OpenCV";
    m.def("process", &process, "process the tile and return json with number cords");
}