
#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>
#include <vector>
#include <filesystem>

#include <db.h>

namespace fs = std::filesystem;

#ifndef PIX_SERVER_H
#define PIX_SERVER_H

int min_x = 186584;
int max_x = 189577;

int min_y = 121099;
int max_y = 125168;

int total_width = (max_x - min_x + 3) * 255;
int total_height = (max_y - min_y + 3) * 255;

std::string real_tiles = "/root/tiles/";
std::string new_tiles = "/root/new_tiles/";

cv::Mat whiteImage(256, 256, CV_8UC3, cv::Scalar(255, 255, 255));

int pMod(int a, int b)
{
    int result = a % b;
    if (result < 0)
    {
        result += b;
    }
    return result;
}

float pDiv(int a, int b)
{
    return (float)a / (float)b;
}

class pix_server
{
public:
    pix_server()
    {
    }

    void test(int x, int y)
    {
        std::vector<int> i = relative_cord(x, y);
        std::cout << i[0] << " " << i[1] << " " << i[2] << " " << i[3] << " x:" << x << " y:" << y << "\n";
    }

    cv::Vec3b get_pix(int x, int y)
    {
        std::vector r_crd = relative_cord(x, y);
        // std::cout << r_crd[0] << " " << r_crd[1] << " " << r_crd[2] << " " << r_crd[3] << " x:" << x << " y:" << y << "\n";

        cv::Mat im = get_img(r_crd[2], r_crd[3]);

        // cv::Mat im = get_img(x, y);

        return im.at<cv::Vec3b>(r_crd[1], r_crd[0]);
    }

    void put_pix(int &x, int &y, const cv::Vec3b &pix)
    {
        std::vector r_crd = relative_cord(x, y);
        get_img(r_crd[2], r_crd[3]).at<cv::Vec3b>(r_crd[1], r_crd[0]) = pix;
    }

    cv::Mat get_img(int &x, int &y)
    {
        std::string st = std::to_string(x) + ',' + std::to_string(y);
        for (int i = 0; i < 5; i++)
        {
            if (tiles_key[i] == st)
                return tiles_val[i];
        }
        return load_img(x, y);
    }

    cv::Mat load_img(int &x, int &y)
    {
        std::string st = std::to_string(x) + ',' + std::to_string(y);
        bool sv = 1;
        cv::Mat img;

        // std::cout << real_tiles + std::to_string(x) + '/' + std::to_string(y) + ".png\n";
        // std::string s;
        // std::cin >> s;

        if (fs::exists(new_tiles + st + ".png"))
        {
            img = cv::imread(new_tiles + st + ".png");
        }
        else if (fs::exists(real_tiles + std::to_string(x) + '/' + std::to_string(y) + ".png"))
        {
            img = cv::imread(real_tiles + std::to_string(x) + '/' + std::to_string(y) + ".png");
        }
        else
        {
            img = whiteImage;
            sv = 0;
        }

        if (img.empty())
        {
            std::cerr << "Error: " << x << "," << y << std::endl;

            MongoDBClient client;
            client.recErr(x,y);
            img = whiteImage;
        }

        if (tiles_sv[0])
            cv::imwrite(new_tiles + tiles_key[0] + ".png", tiles_val[0]);

        tiles_key.erase(tiles_key.begin());
        tiles_key.push_back(st);
        tiles_val.erase(tiles_val.begin());
        tiles_val.push_back(img);
        tiles_sv.erase(tiles_sv.begin());
        tiles_sv.push_back(sv);

        return img;
    }

private:
    std::vector<cv::Mat> tiles_val = {whiteImage, whiteImage, whiteImage, whiteImage, whiteImage};
    std::vector<std::string> tiles_key = {"", "", "", "", ""};
    std::vector<bool> tiles_sv = {0, 0, 0, 0, 0};

    std::vector<int> relative_cord(int &x_, int &y_)
    {

        int tilex = int(pDiv(x_, 256)) + min_x;
        int pixx = pMod((x_ - 256), 256);

        int tiley = int(pDiv(y_, 256)) + min_y;
        int pixy = pMod((y_ - 256), 256);

        return {pixx, pixy, tilex, tiley};
    }
    
};

#endif
