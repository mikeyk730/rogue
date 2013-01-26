//Create the layout for the new level
//rooms.c     1.4 (A.I. Design)       12/16/84

#include <ctype.h>

#include "rogue.h"
#include "main.h"
#include "rooms.h"
#include "monsters.h"
#include "list.h"
#include "curses.h"
#include "new_leve.h"
#include "maze.h"
#include "move.h"
#include "chase.h"
#include "misc.h"
#include "io.h"
#include "level.h"
#include "thing.h"

#define GOLDGRP  1

//do_rooms: Create rooms and corridors with a connectivity graph
void do_rooms()
{
  int i, rm;
  struct Room *rp;
  AGENT *tp;
  int left_out;
  Coord top;
  Coord bsze;
  Coord mp;
  int old_lev;
  int endline;

  endline = maxrow+1;
  old_lev = level;
  //bsze is the maximum room size
  bsze.x = COLS/3;
  bsze.y = endline/3;
  //Clear things for a new level
  for (rp = rooms; rp<&rooms[MAXROOMS]; rp++) rp->r_goldval = rp->r_nexits = rp->r_flags = 0;
  //Put the gone rooms, if any, on the level
  left_out = rnd(4);
  for (i = 0; i<left_out; i++)
  {
    do rp = &rooms[(rm = rnd_room())]; while (rp->r_flags&ISMAZE);
    rp->r_flags |= ISGONE;
    if (rm>2 && level>10 && rnd(20)<level-9)
      rp->r_flags |= ISMAZE;
  }
  //dig and populate all the rooms on the level
  for (i = 0, rp = rooms; i<MAXROOMS; rp++, i++)
  {
    //Find upper left corner of box that this room goes in
    top.x = (i%3)*bsze.x+1;
    top.y = i/3*bsze.y;
    if (rp->r_flags&ISGONE)
    {
      //If the gone room is a maze room, draw the maze and set the size equal to the maximum possible.
      if (rp->r_flags&ISMAZE) {rp->r_pos.x = top.x; rp->r_pos.y = top.y; draw_maze(rp);}
      else
      {
        //Place a gone room.  Make certain that there is a blank line for passage drawing.
        do
        {
          rp->r_pos.x = top.x+rnd(bsze.x-2)+1;
          rp->r_pos.y = top.y+rnd(bsze.y-2)+1;
          rp->r_max.x = -COLS;
          rp->r_max.x = -endline;
        } while (!(rp->r_pos.y>0 && rp->r_pos.y<endline-1));
      }
      continue;
    }
    if (rnd(10)<(level-1)) rp->r_flags |= ISDARK;
    //Find a place and size for a random room
    do
    {
      rp->r_max.x = rnd(bsze.x-4)+4;
      rp->r_max.y = rnd(bsze.y-4)+4;
      rp->r_pos.x = top.x+rnd(bsze.x-rp->r_max.x);
      rp->r_pos.y = top.y+rnd(bsze.y-rp->r_max.y);
    } while (rp->r_pos.y==0);
    draw_room(rp);
    //Put the gold in
    if ((rnd(2)==0) && (!saw_amulet || (level>=max_level)))
    {
      ITEM *gold;

      if ((gold = create_item())!=NULL)
      {
        gold->gold_value = rp->r_goldval = GOLDCALC;
        while (1)
        {
          byte gch;

          rnd_pos(rp, &rp->r_gold);
          gch = get_tile(rp->r_gold.y, rp->r_gold.x);
          if (isfloor(gch)) break;
        }
        bcopy(gold->pos, rp->r_gold);
        gold->flags = ISMANY;
        gold->group = GOLDGRP;
        gold->type = GOLD;
        attach_item(&lvl_obj, gold);
        set_tile(rp->r_gold.y, rp->r_gold.x, GOLD);
      }
    }
    //Put the monster in
    if (rnd(100)<(rp->r_goldval>0?80:25))
    {
      if ((tp = create_agent())!=NULL)
      {
        byte mch;

        do {
          rnd_pos(rp, &mp); 
          mch = display_character(mp.y, mp.x);
        } while (!isfloor(mch));
        new_monster(tp, randmonster(FALSE), &mp);
        give_pack(tp);
      }
    }
  }
}

//draw_room: Draw a box around a room and lay down the floor
void draw_room(struct Room *rp)
{
  int y, x;

  //Here we draw normal rooms, one side at a time
  vert(rp, rp->r_pos.x); //Draw left side
  vert(rp, rp->r_pos.x+rp->r_max.x-1); //Draw right side
  horiz(rp, rp->r_pos.y); //Draw top
  horiz(rp, rp->r_pos.y+rp->r_max.y-1); //Draw bottom
  set_tile(rp->r_pos.y, rp->r_pos.x, ULWALL);
  set_tile(rp->r_pos.y, rp->r_pos.x+rp->r_max.x-1, URWALL);
  set_tile(rp->r_pos.y+rp->r_max.y-1, rp->r_pos.x, LLWALL);
  set_tile(rp->r_pos.y+rp->r_max.y-1, rp->r_pos.x+rp->r_max.x-1, LRWALL);
  //Put the floor down
  for (y = rp->r_pos.y+1; y<rp->r_pos.y+rp->r_max.y-1; y++)
    for (x = rp->r_pos.x+1; x<rp->r_pos.x+rp->r_max.x-1; x++)
      set_tile(y, x, FLOOR);
}

//vert: Draw a vertical line
void vert(struct Room *rp, int startx)
{
  int y;

  for (y = rp->r_pos.y+1; y<=rp->r_max.y+rp->r_pos.y-1; y++)
    set_tile(y, startx, VWALL);
}

//horiz: Draw a horizontal line
void horiz(struct Room *rp, int starty)
{
  int x;

  for (x = rp->r_pos.x; x<=rp->r_pos.x+rp->r_max.x-1; x++) 
    set_tile(starty, x, HWALL);
}

//rnd_pos: Pick a random spot in a room
void rnd_pos(struct Room *rp, Coord *cp)
{
  cp->x = rp->r_pos.x+rnd(rp->r_max.x-2)+1;
  cp->y = rp->r_pos.y+rnd(rp->r_max.y-2)+1;
}

//enter_room: Code that is executed whenever you appear in a room
void enter_room(Coord *cp)
{
  struct Room *rp;
  int y, x;
  AGENT *tp;

  rp = player.t_room = roomin(cp);
  if (bailout || (rp->r_flags&ISGONE && (rp->r_flags&ISMAZE)==0))
  {
    debug("in a gone room");
    return;
  }
  door_open(rp);
  if (!(rp->r_flags&ISDARK) && !on(player, ISBLIND) && !(rp->r_flags&ISMAZE))
    for (y = rp->r_pos.y; y<rp->r_max.y+rp->r_pos.y; y++)
    {
      move(y, rp->r_pos.x);
      for (x = rp->r_pos.x; x<rp->r_max.x+rp->r_pos.x; x++)
      {
        //Displaying monsters is all handled in the chase code now
        tp = monster_at(y, x);
        if (tp==NULL || !can_see_monst(tp)) 
          addch(get_tile(y, x));
        else {
          tp->t_oldch = get_tile(y,x); 
          addch(tp->t_disguise);
        }
      }
    }
}

//leave_room: Code for when we exit a room
void leave_room(Coord *cp)
{
  int y, x;
  struct Room *rp;
  byte floor;
  byte ch;

  rp = player.t_room;
  player.t_room = &passages[get_flags(cp->y, cp->x)&F_PNUM];
  floor = ((rp->r_flags&ISDARK) && !on(player, ISBLIND))?' ':FLOOR;
  if (rp->r_flags&ISMAZE) floor = PASSAGE;
  for (y = rp->r_pos.y+1; y<rp->r_max.y+rp->r_pos.y-1; y++) {
    for (x = rp->r_pos.x+1; x<rp->r_max.x+rp->r_pos.x-1; x++) {
      switch (ch = mvinch(y, x))
      {
      case ' ': case PASSAGE: case TRAP: case STAIRS:
        break;

      case FLOOR:
        if (floor==' ') addch(' ');
        break;

      default:
        //to check for monster, we have to strip out standout bit
        if (isupper(toascii(ch)))
          if (on(player, SEEMONST)) {
            standout(); 
            addch(ch); 
            standend(); 
            break;
          }
          else 
            monster_at(y, x)->t_oldch = '@';
          addch(floor);
      }
    }
  }
  door_open(rp);
}
