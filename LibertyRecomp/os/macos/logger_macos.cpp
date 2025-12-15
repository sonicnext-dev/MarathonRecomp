#include <os/logger.h>

#include <cstdio>
 #include <atomic>
 #include <chrono>
 #include <ctime>
 #include <filesystem>
 #include <string>
 #include <thread>
 #include <unistd.h>
 #include <fcntl.h>
 
 #include <os/process.h>
 
 namespace
 {
     struct TeeState
     {
         int logFd = -1;
         int origStdoutFd = -1;
         int origStderrFd = -1;
 
         int stdoutPipe[2] = { -1, -1 };
         int stderrPipe[2] = { -1, -1 };
 
         std::thread stdoutThread;
         std::thread stderrThread;
         std::atomic<bool> running = false;
     };
 
     static TeeState g_tee;
 
     static std::filesystem::path MakeLogPath()
     {
         const auto now = std::chrono::system_clock::now();
         const std::time_t t = std::chrono::system_clock::to_time_t(now);
 
         std::tm tm{};
         localtime_r(&t, &tm);
 
         char buf[64] = {};
         std::snprintf(buf, sizeof(buf), "run_log_%04d%02d%02d_%02d%02d%02d.txt",
             tm.tm_year + 1900,
             tm.tm_mon + 1,
             tm.tm_mday,
             tm.tm_hour,
             tm.tm_min,
             tm.tm_sec);

         std::error_code ec;
         auto dir = os::process::GetExecutableRoot() / "run_logs";
         std::filesystem::create_directories(dir, ec);
         if (!ec)
             return dir / buf;

         return os::process::GetWorkingDirectory() / buf;
     }
 
     static void TeePump(int readFd, int origFd, int logFd, std::atomic<bool>* running)
     {
         char buffer[4096];
         while (running->load(std::memory_order_relaxed))
         {
             const ssize_t n = ::read(readFd, buffer, sizeof(buffer));
             if (n <= 0)
                 break;
 
             if (origFd >= 0)
                 (void)::write(origFd, buffer, static_cast<size_t>(n));
             if (logFd >= 0)
                 (void)::write(logFd, buffer, static_cast<size_t>(n));
         }
 
         (void)::close(readFd);
     }
 }

void os::logger::Init()
{
     if (g_tee.running.exchange(true))
         return;
 
     const auto logPath = MakeLogPath();
     g_tee.logFd = ::open(logPath.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
 
     g_tee.origStdoutFd = ::dup(STDOUT_FILENO);
     g_tee.origStderrFd = ::dup(STDERR_FILENO);
 
     if (::pipe(g_tee.stdoutPipe) == 0)
     {
         ::dup2(g_tee.stdoutPipe[1], STDOUT_FILENO);
         ::close(g_tee.stdoutPipe[1]);
         g_tee.stdoutPipe[1] = -1;
 
         setvbuf(stdout, nullptr, _IONBF, 0);
 
         g_tee.stdoutThread = std::thread(TeePump, g_tee.stdoutPipe[0], g_tee.origStdoutFd, g_tee.logFd, &g_tee.running);
         g_tee.stdoutThread.detach();
     }
 
     if (::pipe(g_tee.stderrPipe) == 0)
     {
         ::dup2(g_tee.stderrPipe[1], STDERR_FILENO);
         ::close(g_tee.stderrPipe[1]);
         g_tee.stderrPipe[1] = -1;
 
         setvbuf(stderr, nullptr, _IONBF, 0);
 
         g_tee.stderrThread = std::thread(TeePump, g_tee.stderrPipe[0], g_tee.origStderrFd, g_tee.logFd, &g_tee.running);
         g_tee.stderrThread.detach();
     }
}

void os::logger::Log(const std::string_view str, ELogType type, const char* func)
{
    if (func)
    {
        fmt::println("[{}] {}", func, str);
    }
    else
    {
        fmt::println("{}", str);
    }

    // On macOS, this app frequently exits via std::_Exit(), which bypasses
    // normal stdio flushing. Flushing here ensures logs are visible when
    // stdout is not a TTY (e.g. piped through a test harness).
    std::fflush(stdout);
}
