#include "pch.h"
#include "Logger.h"
#include <fstream>

Logger::Logger()
{
    loggingThread = std::thread(&Logger::ProcessLogs, this);
}

Logger::~Logger()
{
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        stopping = true;
    }

    condition.notify_one();

    if (loggingThread.joinable())
    {
        loggingThread.join();
    }
}

void Logger::Log(const std::wstring& message)
{
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        logQueue.push(message);
    }

    condition.notify_one();
}

void Logger::ProcessLogs()
{
    std::wofstream logFile(L"chat_log.txt", std::ios::app);

    while (true)
    {
        std::wstring message;

        {
            std::unique_lock<std::mutex> lock(queueMutex);

            condition.wait(lock, [this]()
                {
                    return stopping || !logQueue.empty();
                });

            if (stopping && logQueue.empty())
            {
                break;
            }

            message = logQueue.front();
            logQueue.pop();
        }

        if (logFile.is_open())
        {
            logFile << message << std::endl;
            logFile.flush();
        }
    }
}