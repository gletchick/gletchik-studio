#include "core/project/indexworker.h"
#include "core/project/projectmanager.h"

namespace gs {

    IndexWorker::IndexWorker(QObject* parent)
        : QObject(parent), m_isRunning(true) {
        m_workerThread = std::thread(&IndexWorker::workerLoop, this);
    }

    IndexWorker::~IndexWorker() {
        m_isRunning = false;
        m_condition.notify_all();

        if (m_workerThread.joinable()) {
            m_workerThread.join();
        }
    }

    void IndexWorker::enqueueFile(const QString& filePath) {
        {
            std::lock_guard<std::mutex> lock(m_queueMutex);
            m_taskQueue.push(filePath);
        }
        m_condition.notify_one();
    }

    void IndexWorker::clearQueue() {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        std::queue<QString> empty;
        std::swap(m_taskQueue, empty);
    }

    void IndexWorker::workerLoop() {
        while (m_isRunning) {
            QString currentFilePath;

            {
                std::unique_lock<std::mutex> lock(m_queueMutex);
                m_condition.wait(lock, [this]() {
                    return !m_taskQueue.empty() || !m_isRunning;
                });

                if (!m_isRunning && m_taskQueue.empty()) {
                    break;
                }

                currentFilePath = m_taskQueue.front();
                m_taskQueue.pop();
            }

            if (!currentFilePath.isEmpty()) {
                ProjectManager::instance().processFileInternal(currentFilePath);
                emit fileIndexed(currentFilePath);
            }

            bool empty = false;
            {
                std::lock_guard<std::mutex> lock(m_queueMutex);
                empty = m_taskQueue.empty();
            }

            if (empty) {
                ProjectManager::instance().saveProject();
                emit indexingFinished();
            }
        }
    }

} // namespace gs