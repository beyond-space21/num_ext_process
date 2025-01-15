
#include <cmath>
#include <opencv2/opencv.hpp>
#include <stack>
#include <appUtil.h>
#include <db.h>
#include <relative.h>


class process
{
public:
    process(int x_temp)
    {
        x = x_temp;

        y = findDocumentWithLargestRootY(x);
        trk = get_track(x);

        if (y == -1)
        {
            if (trk == -1)
            {
                y = 0;
                start_track(x, 0);
            }
            else
                std::cout << "y is -1 but get_track(" << x << ") exists" << '\n';
        }
        if (trk == total_height)
            y = total_height;

        y = 0;
    };

    void start()
    {
        int count = 0;
        for (y=y; y < total_height; y++)
        {
            cv::Vec3b pixel = pix.get_pix(x, y);
            // std::cout << static_cast<int>(pixel[0]) <<  " " << static_cast<int>(pixel[2]) << " " << static_cast<int>(pixel[3]) << "\n";
            if(isSimilarToPurple(pixel)){
                count++;
                // std::vector<int> a = relative_cord(x,y);
                // std::cout << a[0] << " " << a[1] << "\n";
                // std::string h = "";
                // std::cin >> h;
                std::vector<int> nxt_sd = get_teritory_and_push(x,y);
                y = nxt_sd[1];
            }
        }

        std::cout << "count: " << count << std::endl;

        end_track(x,y);
    }

private:
    bool verify_pix(int x, int y, std::vector<cv::Point> &visited)
    {

        cv::Point p_ = cv::Point(x, y);
        for (int i = 0; i < visited.size(); i++)
        {
            if (visited[i] == p_)
                return false;
        }

        cv::Vec3b pixel = pix.get_pix(x, y);

        if (isSimilarToPurple(pixel))
        {
            return true;
        }
        return false;
    }
    std::vector<int> get_teritory_and_push(int seed_x, int seed_y)
    {
        std::stack<std::pair<int, int>> to_visit;
        std::vector<cv::Point> visited;
        to_visit.push({seed_x, seed_y});
        visited.push_back(cv::Point(seed_x, seed_y));

        prt("purple: " + std::to_string(seed_x) + " " + std::to_string(seed_y));

        while (!to_visit.empty())
        {
            std::pair<int, int> current = to_visit.top();
            to_visit.pop();

            int cx = current.first;
            int cy = current.second;
            visited.push_back(cv::Point(cx, cy));

            if (verify_pix(cx - 1, cy, visited))
                to_visit.push({cx - 1, cy});
            if (verify_pix(cx, cy + 1, visited))
                to_visit.push({cx, cy + 1});
            if (verify_pix(cx, cy - 1, visited))
                to_visit.push({cx, cy - 1});
            if (verify_pix(cx + 1, cy, visited))
                to_visit.push({cx + 1, cy});
        }

        cv::Rect boundingBox = cv::boundingRect(visited);
        cv::Mat blankImage = cv::Mat::zeros(boundingBox.size(), CV_8UC3);

        for (int y = boundingBox.y; y < boundingBox.y + boundingBox.height; ++y)
        {
            for (int x = boundingBox.x; x < boundingBox.x + boundingBox.width; ++x)
            {
                    blankImage.at<cv::Vec3b>(y - boundingBox.y, x - boundingBox.x) = pix.get_pix(x, y);
                    pix.put_pix(x, y, cv::Vec3b(255, 255, 255));
            }
        }

        std::string data = image_to_data_url(blankImage);
        int data_x = boundingBox.x;
        int data_y = boundingBox.y;
        int root_x = seed_x;
        int root_y = seed_y;

        // insertDocument(data, data_x, data_y, root_x, root_y);

        int new_seed_x = boundingBox.x + boundingBox.width;
        int new_seed_y = boundingBox.y + boundingBox.height;

        return {new_seed_x, new_seed_y};
    }

    pix_server pix;
    int x;
    int y;
    int trk;
};

