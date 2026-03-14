#include "core/utils/threadsafequeue.h"

namespace gs {

    void ThreadSafeQueue::push(const std::string& message) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(message);
    }

    std::optional<std::string> ThreadSafeQueue::tryPop() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_queue.empty()) {
            return std::nullopt;
        }
        
        std::string val = std::move(m_queue.front());
        m_queue.pop();
        return val;
    }

    bool ThreadSafeQueue::isEmpty() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

    void ThreadSafeQueue::clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::queue<std::string> empty;
        std::swap(m_queue, empty);
    }

} // namespace gs