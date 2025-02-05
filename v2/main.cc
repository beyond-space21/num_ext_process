#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <opencv2/opencv.hpp>
#include <stack>
#include <nlohmann/json.hpp>

namespace py = pybind11;
using json = nlohmann::json;

std::string tiles = "tilues/";

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

std::string base64_encode(const std::vector<uchar>& data) {

    if (data.empty()) {
    throw std::invalid_argument("Input data for base64 encoding is empty.");
}

    static const char s_encoding[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    std::string encoded;
    int val = 0, valb = -6;
    for (uchar c : data) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            encoded.push_back(s_encoding[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) encoded.push_back(s_encoding[((val << 8) >> valb) & 0x3F]);
    while (encoded.size() % 4) encoded.push_back('=');
    return encoded;
}

std::string image_to_data_url(const cv::Mat& image) {
    // if (image.empty()) {
    //     throw std::invalid_argument("Image is empty or not loaded correctly.");
    // }

    // Encode the image to PNG format
    std::vector<uchar> buffer;
    std::vector<int> params = { cv::IMWRITE_PNG_COMPRESSION, 9 }; // PNG compression level
    cv::imencode(".png", image, buffer, params);

    if (buffer.empty()) {
    throw std::runtime_error("Failed to encode image to PNG format.");
}

    // Encode buffer to base64
    std::string base64_data = base64_encode(buffer);

    // Create Data URL
    base64_data = "data:image/png;base64," + base64_data;

    return base64_data;
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

private:
    int realX;
    int realY;

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

    if (x < 0 || x > 255 || y < 0 || y > 255)
    {
        std::cout << "getting: " << x << " " << y << std::endl;
        pixel = ser.get_pix(x, y);
    }
    else
    {
        pixel = img.at<cv::Vec3b>(y, x);
    }

    if (isSimilarToPurple(pixel))
    {
        return true;
    }
    return false;
}

std::vector<int> get_teritory(int seed_x, int seed_y, cv::Mat &img, image_server &ser)
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

    json response;
    response["next_x"] = boundingBox.x + boundingBox.width;
    response["image"] = image_to_data_url(img);
    response["pos_x"] = boundingBox.x;
    response["pos_y"] = boundingBox.y;

    return {boundingBox.x + boundingBox.width};
}

std::vector<int> process(const py::array_t<uint8_t> &input_image, int tlX, int tlY)
{
    std::vector<int> line_ind;
    py::buffer_info buf = input_image.request();
    cv::Mat image(buf.shape[0], buf.shape[1], CV_8UC3, (uint8_t *)buf.ptr);

    image_server ser(tlX, tlY);

    for (int y = 0; y < 256; y = y + 8)
    {
        for (int x = 0; x < 256; x++)
        {
            if (isSimilarToPurple(image.at<cv::Vec3b>(y, x)))
            {
                x = get_teritory(x, y, image, ser)[0];
            }
        }
    }

    return line_ind;
}

PYBIND11_MODULE(pullNumbers, m)
{
    m.doc() = "Image processing module using OpenCV";
    m.def("process", &process, "process the tile and return json with number cords");
}