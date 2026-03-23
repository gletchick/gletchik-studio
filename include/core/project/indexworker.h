#pragma once

#include <QObject>
#include <QString>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>

namespace gs {

    class IndexWorker : public QObject {
        Q_OBJECT

    public:
        explicit IndexWorker(QObject* parent = nullptr);
        ~IndexWorker() override;

        void enqueueFile(const QString& filePath);
        void clearQueue();

        signals:
            void fileIndexed(const QString& filePath);
        void indexingFinished();

    private:
        void workerLoop();

        std::queue<QString> m_taskQueue;
        std::mutex m_queueMutex;
        std::condition_variable m_condition;

        std::thread m_workerThread;
        std::atomic<bool> m_isRunning;
    };

} // namespace gs