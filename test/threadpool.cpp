#include "../ThreadPool.h"
#include <iostream>
#include <vector>

int main() {
    // 获取线程池的单例实例
    ThreadPool& pool = ThreadPool::getInstance(4);

    // 提交一些任务到线程池
    std::vector<std::future<int>> results;

    for (int i = 0; i < 8; ++i) {
        results.emplace_back(
            pool.submit([i] {
                std::cout << "Task " << i << " is processing\n";
                std::this_thread::sleep_for(std::chrono::seconds(1));
                std::cout << "Task " << i << " is done\n";
                return i * i;
            })
        );
    }

    // 获取结果
    for (auto && result : results) {
        std::cout << result.get() << std::endl;
    }

    return 0;
}

