#include <sstream>

#include "loop_task.h"
#include "../../safety_check/logging.h"

void LoopTask::start_loop()
{
    if (running)
    {
        cqc::logging::warning(u8"Thread exception", u8"Loop is currently running already");
        return;
    }
    terminate = false;
    loop_thread = std::thread([this]()
    {
        std::ostringstream thread_id;
        thread_id << std::this_thread::get_id();
        const std::string thread_id_string(thread_id.str());
        cqc::logging::debug(u8"Loop thread starting", u8"Thread ID: " + thread_id_string);
        int loop_count = 0;
        try
        {
            while (!terminate)
            {
                loop_count++;
                cqc::logging::debug(u8"Loop thread", u8"Thread ID: " + thread_id_string + ", Loop: " + std::to_string(loop_count));
                task();
                std::this_thread::sleep_for(loop_period);
            }
        }
        catch (const std::exception&) {}
        running = false;
    });
    running = true;
}
