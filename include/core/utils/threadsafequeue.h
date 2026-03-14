#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>
#include <optional>

namespace gs {

    class ThreadSafeQueue {
    public:
        ThreadSafeQueue() = default;

        void push(const std::string& message);

        // Забрать сообщение (вызывает UI поток). 
        // Если очередь пуста, возвращает пустой optional (не блокирует поток намертво)
        std::optional<std::string> tryPop();

        // Очистить очередь
        void clear();

        bool isEmpty() const;

    private:
        mutable std::mutex m_mutex;
        std::queue<std::string> m_queue;
    };

} // namespace gs