#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <future>
#include <fstream>

#include <process.h>

class ThreadPool
{
public:
    explicit ThreadPool(size_t threadCount)
        : stopFlag(false)
    {
        for (size_t i = 0; i < threadCount; ++i)
        {
            workers.emplace_back([this]
                                 {
                while (true) {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(queueMutex);
                        condition.wait(lock, [this] {
                            return stopFlag || !tasks.empty();
                        });

                        if (stopFlag && tasks.empty()) {
                            return;
                        }

                        task = std::move(tasks.front());
                        tasks.pop();
                    }

                    task();  // Execute the task
                } });
        }
    }

    template <typename F, typename... Args>
    auto enqueue(F &&f, Args &&...args)
        -> std::future<typename std::result_of<F(Args...)>::type>
    {
        using returnType = typename std::result_of<F(Args...)>::type;

        auto task = std::make_shared<std::packaged_task<returnType()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        std::future<returnType> result = task->get_future();

        {
            std::unique_lock<std::mutex> lock(queueMutex);

            if (stopFlag)
            {
                throw std::runtime_error("Enqueue on stopped ThreadPool");
            }

            tasks.emplace([task]()
                          { (*task)(); });
        }

        condition.notify_one();
        return result;
    }

    ~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stopFlag = true;
        }

        condition.notify_all();

        for (std::thread &worker : workers)
        {
            worker.join();
        }
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<bool> stopFlag;
};

void prs(int x, int y)
{
    try
    {
        cv::Mat image = cv::imread("tiles/" + std::to_string(x) + "/" + std::to_string(y) + ".png");
        process(image, x, y);
        std::cout << "completed: " << x << " | " << y << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "func:prs | " << e.what() << '\n';
    }
}

int main()
{

    ThreadPool pool(20);

    std::ifstream MyReadFile("p1.json");
    std::string prs;
    while (getline(MyReadFile, prs))
        ;
    auto data = json::parse(prs)["data"];

    std::cout << data.size() << std::endl;

    int x;
    int y;
    for (int i = 0; i < data.size(); i++)
    {
        x = data[i][0];
        y = data[i][1];
        pool.enqueue([x, y]
                     { prs(x, y); });
    }

    return 0;
}