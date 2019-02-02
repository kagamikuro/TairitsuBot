#include <chrono>
#include <fstream>

#include "user_scores.h"

UserScore::UserScore() { last_update_ = std::time(nullptr); }

float UserScore::score()
{
    const float ratio = 1e-4f;
    const time_t now = std::time(nullptr);
    if (score_ > 0.0f)
    {
        last_update_ = now;
        return score_;
    }
    score_ += ratio * (now - last_update_);
    if (score_ > 0.0f) score_ = 0.0f;
    last_update_ = now;
    return score_;
}

void UserScore::set_score(const float value) { score_ = value; }

void UserScore::read_from(std::ifstream& stream)
{
    stream.read(reinterpret_cast<char*>(&score_), sizeof(float));
    stream.read(reinterpret_cast<char*>(&last_update_), sizeof(time_t));
}

void UserScore::write_to(std::ofstream& stream) const
{
    stream.write(reinterpret_cast<const char*>(&score_), sizeof(float));
    stream.write(reinterpret_cast<const char*>(&last_update_), sizeof(time_t));
}

void UserScores::auto_save()
{
    using namespace std::literals;
    namespace chr = std::chrono;
    using Clock = chr::system_clock;
    Clock::time_point last_save = Clock::now();
    while (is_active_)
    {
        std::this_thread::sleep_for(5s);
        if (Clock::now() - last_save > 15min)
        {
            last_save = Clock::now();
            save_data();
        }
    }
}

UserScores& UserScores::instance()
{
    static UserScores instance;
    return instance;
}

UserScores::UserScores()
{
    is_active_ = true;
    file_io_sync_ = std::make_unique<Synchronizer>();
    auto_save_thread_ = std::thread(&UserScores::auto_save, this);
}

void UserScores::terminate()
{
    is_active_ = false;
    auto_save_thread_.join();
    file_io_sync_.reset();
}

void UserScores::load_data()
{
    file_io_sync_->produce([&]()
        {
            std::ifstream stream(file_path_, std::ios::in | std::ios::binary);
            if (!stream.good()) return;
            while (true)
            {
                int64_t user_id;
                stream.read(reinterpret_cast<char*>(&user_id), sizeof(int64_t));
                if (!stream.good()) break;
                UserScore score;
                score.read_from(stream);
                scores_.set(user_id, score);
            }
            stream.close();
        });
}

void UserScores::save_data()
{
    file_io_sync_->produce([&]()
        {
            std::ofstream stream(file_path_, std::ios::out | std::ios::binary);
            if (!stream.good()) return;
            scores_.for_each([&stream](const int64_t key, const UserScore& value)
                {
                    stream.write(reinterpret_cast<const char*>(&key), sizeof(int64_t));
                    value.write_to(stream);
                });
            stream.close();
        });
}

float UserScores::score_of(const int64_t user_id) { return scores_.get(user_id).score(); }

void UserScores::set_score_of(const int64_t user_id, const float score)
{
    scores_.manipulate(user_id,
        [score](UserScore& value) { value.set_score(score); });
}
