#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <future>

#include <app.h>

void prs(int i)
{
    process thd(i);
    thd.start();
}


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

int main()
{
    size_t cores = std::thread::hardware_concurrency();
    std::cout << "Using " << cores << " cores for the thread pool." << std::endl;

    ThreadPool pool(22);

    std::vector<int> trk;

    for (int e = 0; e < 256; e = e + 1)
        for (int i = e; i < (383232); i = i + 256) //766464
        {
            trk.push_back(i);
            std::cout << i / 256 << std::endl;
            pool.enqueue([i] {
                prs(i);
                std::cout << "completed: " << i << std::endl;
            });
        }


// pix_server pix;
// pix.get_pix(186653,122646);

    return 0;
}
