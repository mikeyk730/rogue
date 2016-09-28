#pragma once

#include "agent.h"

const int EX_DIVIDES = 0x0001;
const int EX_SHOOTS_ICE = 0x0002;
const int EX_SHOOTS_FIRE = 0x0004;
const int EX_RUSTS_ARMOR = 0x0008;
const int EX_HOLDS = 0x0010;
const int EX_STATIONARY = 0x0020;
const int EX_MIMICS = 0x0040;
const int EX_CONFUSES = 0x0080;
const int EX_STEALS_GOLD = 0x0100;
const int EX_STEALS_MAGIC = 0x0200;
const int EX_DRAINS_STR = 0x0400;
const int EX_DRAINS_EXP = 0x0800;
const int EX_DRAINS_MAXHP = 0x1000;
const int EX_HOLD_ATTACKS = 0x2000;
const int EX_SUICIDES = 0x4000;
const int EX_DROPS_GOLD = 0x8000;

struct Monster : public Agent
{
    //create_monster: Pick a new monster and add it to the list
    static Monster* CreateMonster(byte type, Coord *cp, int level);

    virtual std::string get_name();

    //special features
    bool is_stationary() const;
    bool can_hold() const;
    bool hold_attacks() const;
    bool can_divide() const;
    bool shoots_fire() const;
    bool immune_to_fire() const;
    bool shoots_ice() const;
    bool causes_confusion() const;
    bool is_mimic() const;
    bool is_disguised() const;
    bool steals_gold() const;
    bool drops_gold() const;
    bool steals_magic() const;
    bool drains_life() const;
    bool drains_exp() const;
    bool drains_strength() const;
    bool rusts_armor() const;
    bool dies_from_attack() const;

    bool is_monster_confused_this_turn() const;

    //do_chase: Make one thing chase another.  Returns pointer to monster that may have been invalidated.  //todo:nix return value
    Monster* do_chase();

    //attack: The monster attacks the player.  Returns pointer to monster that may have been invalidated.  //todo:nix return value
    Monster* attack_player();

    int get_carry_probability() const;

    void reveal_disguise();

    //chase: Find the spot for the chaser(er) to move closer to the chasee(ee). Returns true if we want to keep on chasing later. false if we reach the goal.
    void chase(Coord *chasee_pos);

    //slime specific
    void set_dirty(bool enable);
    bool is_dirty();

    //find_dest: find the proper destination for the monster
    Coord *find_dest();
    void set_destination();

    //start_run: Set a monster running after something
    void start_run();

    //give_pack: Give a pack to a monster if it deserves one
    void give_pack();

    bool is_seeking(Item* obj);


    byte disguise = 0;                //What mimic looks like
    int exflags = 0;                  //More state;
    int value = 0;                    //
    Coord *dest = 0;                  //Where it is running to
};