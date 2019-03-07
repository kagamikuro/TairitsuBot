#pragma once

#include "tasks/multiplayer_game/multiplayer_game.h"
#include "core/singleton.h"
#include "uno.h"

class UnoGame final :
    public MultiplayerGame<Uno>, public Singleton<UnoGame>
{
public:
    const char* task_name() override { return "UnoGame"; }
    void save_data() const override;
    void load_data() override;
protected:
    bool on_group_msg(int64_t group, int64_t user, const std::string& msg) override;
    bool on_private_msg(int64_t user, const std::string& msg) override;
public:
    ~UnoGame() override;
};
