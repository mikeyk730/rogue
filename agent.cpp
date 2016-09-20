#include "agent.h"
#include "rings.h"
#include "pack.h"

void Agent::set_invisible(bool enable){
    set_flag(IS_INVIS, enable);
}
void Agent::set_found(bool enable){
    set_flag(IS_FOUND, enable);
}
void Agent::set_confused(bool enable){
    set_flag(IS_HUH, enable);
}
void Agent::set_running(bool enable){
    set_flag(IS_RUN, enable);
}
void Agent::set_is_held(bool enable){
    set_flag(IS_HELD, enable);
}
void Agent::set_is_slow(bool enable){
    set_flag(IS_SLOW, enable);
}
void Agent::set_is_fast(bool enable){
    set_flag(IS_HASTE, enable);
}
void Agent::set_can_confuse(bool enable){
    set_flag(CAN_HUH, enable);
}
void Agent::set_cancelled(bool enable){
    set_flag(IS_CANC, enable);
}
void Agent::set_blind(bool enable){
    set_flag(IS_BLIND, enable);
}
void Agent::set_sees_invisible(bool enable){
    set_flag(CAN_SEE, enable);
}
void Agent::set_detects_others(bool enable){
    set_flag(SEE_MONST, enable);
}
void Agent::set_is_mean(bool enable){
    set_flag(IS_MEAN, enable);
}

void Agent::reveal_disguise(){
    disguise = type;
}

//slime specific
void Agent::set_dirty(bool enable){
    value = enable ? 1 : 0;
}
bool Agent::is_dirty(){
    return value == 1;
}

int Agent::Stats::get_hp() const {
    return hp;
}

bool Agent::Stats::decrease_hp(int n, bool can_kill){
    hp -= n;
    if (!can_kill && hp <= 0)
        hp = 1;
    return hp > 0;
}

void Agent::Stats::increase_hp(int n, bool max_bonus, bool second_max_bonus){
    hp += n;

    if (max_bonus && hp > max_hp)
        ++max_hp;
    if (second_max_bonus && hp > max_hp + level + 1)
        ++max_hp;

    if (hp > max_hp) {
        hp = max_hp;
    }
}

int Agent::Stats::drain_hp(){
    hp /= 2;
    return hp;
}

//add_str: Perform the actual add, checking upper and lower bound
void add_str(unsigned int *sp, int amt)
{
    if ((*sp += amt)<3) *sp = 3;
    else if (*sp>31) *sp = 31;
}

void Agent::Stats::adjust_strength(int amt)
{
    unsigned int comp;

    if (amt == 0) return;
    add_str(&str, amt);
    comp = str;
    if (is_ring_on_hand(LEFT, R_ADDSTR))
        add_str(&comp, -get_ring(LEFT)->ring_level);
    if (is_ring_on_hand(RIGHT, R_ADDSTR))
        add_str(&comp, -get_ring(RIGHT)->ring_level);
    if (comp > max_str)
        max_str = comp;
}

void Agent::Stats::restore_strength()
{
    if (is_ring_on_hand(LEFT, R_ADDSTR))
        add_str(&player.stats.str, -get_ring(LEFT)->ring_level);
    if (is_ring_on_hand(RIGHT, R_ADDSTR))
        add_str(&player.stats.str, -get_ring(RIGHT)->ring_level);
    if (player.stats.str < player.stats.max_str)
        player.stats.str = player.stats.max_str;
    if (is_ring_on_hand(LEFT, R_ADDSTR))
        add_str(&player.stats.str, get_ring(LEFT)->ring_level);
    if (is_ring_on_hand(RIGHT, R_ADDSTR))
        add_str(&player.stats.str, get_ring(RIGHT)->ring_level);
}
