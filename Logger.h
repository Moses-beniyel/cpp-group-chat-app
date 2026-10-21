#pragma once

#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>

class Logger
{
public:
    Logger();
    ~Logger();

    void Log(const std::wstring& message);

private:
    void ProcessLogs();

    std::queue<std::wstring> logQueue;
    std::mutex queueMutex;
    std::condition_variable condition;
    std::thread loggingThread;
    bool stopping = false;
};