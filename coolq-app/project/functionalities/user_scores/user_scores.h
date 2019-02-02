#pragma once

#include <atomic>
#include <string>
#include <thread>
#include <memory>

#include "../../utility/utility.h"
#include "../../utility/dictionary.h"
#include "../../utility/synchronizer.h"

class UserScore final
{
private:
    float score_ = 0.0f;
    time_t last_update_ = 0;
public:
    UserScore();
    UserScore(const float score, const time_t last_update) :score_(score), last_update_(last_update) {}
    UserScore& operator=(const UserScore&) = default;
    float score();
    void set_score(float value);
    void read_from(std::ifstream& stream);
    void write_to(std::ofstream& stream) const;
};

class UserScores final
{
private:
    const std::string file_path_ = utility::data_path + "user_scores.dat";
    std::atomic_bool is_active_ = false;
    std::thread auto_save_thread_;
    Dictionary<UserScore> scores_;
    std::unique_ptr<Synchronizer> file_io_sync_;
    void auto_save();
public:
    static UserScores& instance();
    UserScores();
    void terminate();
    void load_data();
    void save_data();
    float score_of(int64_t user_id);
    void set_score_of(int64_t user_id, float score);
};
