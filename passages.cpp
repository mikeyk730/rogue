//Draw the connecting passages
//passages.c  1.4 (A.I. Design)       12/14/84

#include <stdlib.h>

#include "rogue.h"
#include "maze.h"
#include "main.h"
#include "curses.h"
#include "misc.h"
#include "passages.h"
#include "io.h"
#include "level.h"

extern struct Room rooms[];

static int pnum;
static byte newpnum;

//conn: Draw a corridor from a room in a certain direction.
void conn(int r1, int r2)
{
  struct Room *room_from, *room_to;
  int rmt, rm;
  int distance, turn_spot, turn_distance;
  int direc;
  Coord del, curr, turn_delta, spos, epos;

  if (r1<r2)
  {
    rm = r1;
    if (r1+1==r2) direc = 'r'; else direc = 'd';
  }
  else
  {
    rm = r2;
    if (r2+1==r1) direc = 'r'; else direc = 'd';
  }
  room_from = &rooms[rm];
  //Set up the movement variables, in two cases: first drawing one down.
  if (direc=='d')
  {
    rmt = rm+3; //room # of dest
    room_to = &rooms[rmt]; //room pointer of dest
    del.x = 0; //direction of move
    del.y = 1;
    //If we are drawing from/to regular or maze rooms, we have to pick the spot we draw from/to
    if ((room_from->flags&ISGONE)==0 || (room_from->flags&ISMAZE))
    {
      spos.y = room_from->pos.y+room_from->size.y-1;
      do {spos.x = room_from->pos.x+rnd(room_from->size.x-2)+1;} while (get_tile(spos.y,spos.x)==' ');
    }
    else {spos.x = room_from->pos.x; spos.y = room_from->pos.y;}
    epos.y = room_to->pos.y;
    if ((room_to->flags&ISGONE)==0 || (room_to->flags&ISMAZE))
    {
      do {epos.x = room_to->pos.x+rnd(room_to->size.x-2)+1;} while (get_tile(epos.y,epos.x)==' ');
    }
    else epos.x = room_to->pos.x;
    distance = abs(spos.y-epos.y)-1; //distance to move
    turn_delta.y = 0; //direction to turn
    turn_delta.x = (spos.x<epos.x?1:-1);
    turn_distance = abs(spos.x-epos.x); //how far to turn
  }
  else if (direc=='r')
  {
    //setup for moving right
    rmt = rm+1;
    room_to = &rooms[rmt];
    del.x = 1;
    del.y = 0;
    if ((room_from->flags&ISGONE)==0 || (room_from->flags&ISMAZE))
    {
      spos.x = room_from->pos.x+room_from->size.x-1;
      do {spos.y = room_from->pos.y+rnd(room_from->size.y-2)+1;} while (get_tile(spos.y,spos.x)==' ');
    }
    else {spos.x = room_from->pos.x; spos.y = room_from->pos.y;}
    epos.x = room_to->pos.x;
    if ((room_to->flags&ISGONE)==0 || (room_to->flags&ISMAZE))
    {
      do {epos.y = room_to->pos.y+rnd(room_to->size.y-2)+1;} while (get_tile(epos.y,epos.x)==' ');
    }
    else epos.y = room_to->pos.y;
    distance = abs(spos.x-epos.x)-1;
    turn_delta.y = (spos.y<epos.y?1:-1);
    turn_delta.x = 0;
    turn_distance = abs(spos.y-epos.y);
  }
  else debug("error in connection tables");

  turn_spot = rnd(distance-1)+1;
  //Draw in the doors on either side of the passage or just put #'s if the rooms are gone.
  if (!(room_from->flags&ISGONE)) door(room_from, &spos);
  else psplat(spos.y, spos.x);
  if (!(room_to->flags&ISGONE)) door(room_to, &epos);
  else psplat(epos.y, epos.x);
  //Get ready to move...
  curr.x = spos.x;
  curr.y = spos.y;
  while (distance)
  {
    //Move to new position
    curr.x += del.x;
    curr.y += del.y;
    //Check if we are at the turn place, if so do the turn
    if (distance==turn_spot)
    {
      while (turn_distance--) {psplat(curr.y, curr.x); curr.x += turn_delta.x; curr.y += turn_delta.y;}
    }
    //Continue digging along
    psplat(curr.y, curr.x);
    distance--;
  }
  curr.x += del.x;
  curr.y += del.y;
  if (!equal(curr, epos)) {epos.x -= del.x; epos.y -= del.y; psplat(epos.y, epos.x);}
}

//do_passages: Draw all the passages on a level.
void do_passages()
{
  int i, j;
  int roomcount;
  static struct rdes
  {
    char conn[MAXROOMS]; //possible to connect to room i?
    char isconn[MAXROOMS]; //connection been made to room i?
    char ingraph; //this room in graph already?
  } rdes[MAXROOMS] =
  {
    { {0, 1, 0, 1, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0}, 0},
    { {1, 0, 1, 0, 1, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0}, 0},
    { {0, 1, 0, 0, 0, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0}, 0},
    { {1, 0, 0, 0, 1, 0, 1, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0}, 0},
    { {0, 1, 0, 1, 0, 1, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0}, 0},
    { {0, 0, 1, 0, 1, 0, 0, 0, 1}, {0, 0, 0, 0, 0, 0, 0, 0, 0}, 0},
    { {0, 0, 0, 1, 0, 0, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0}, 0},
    { {0, 0, 0, 0, 1, 0, 1, 0, 1}, {0, 0, 0, 0, 0, 0, 0, 0, 0}, 0},
    { {0, 0, 0, 0, 0, 1, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0}, 0}
  };
  struct rdes *r1, *r2;

  //reinitialize room graph description
  for (r1 = rdes; r1<&rdes[MAXROOMS]; r1++)
  {
    for (j = 0; j<MAXROOMS; j++) r1->isconn[j] = FALSE;
    r1->ingraph = FALSE;
  }
  //starting with one room, connect it to a random adjacent room and then pick a new room to start with.
  roomcount = 1;
  r1 = &rdes[rnd(MAXROOMS)];
  r1->ingraph = TRUE;
  do
  {
    //find a room to connect with
    j = 0;
    for (i = 0; i<MAXROOMS; i++) if (r1->conn[i] && !rdes[i].ingraph && rnd(++j)==0) r2 = &rdes[i];
    //if no adjacent rooms are outside the graph, pick a new room to look from
    if (j==0)
    {
      do r1 = &rdes[rnd(MAXROOMS)]; while (!r1->ingraph);
    }
    //otherwise, connect new room to the graph, and draw a tunnel to it
    else
    {
      r2->ingraph = TRUE;
      i = r1-rdes;
      j = r2-rdes;
      conn(i, j);
      r1->isconn[j] = TRUE;
      r2->isconn[i] = TRUE;
      roomcount++;
    }
  } while (roomcount<MAXROOMS);
  //attempt to add passages to the graph a random number of times so that there isn't always just one unique passage through it.
  for (roomcount = rnd(5); roomcount>0; roomcount--)
  {
    r1 = &rdes[rnd(MAXROOMS)]; //a random room to look from
    //find an adjacent room not already connected
    j = 0;
    for (i = 0; i<MAXROOMS; i++) if (r1->conn[i] && !r1->isconn[i] && rnd(++j)==0) r2 = &rdes[i];
    //if there is one, connect it and look for the next added passage
    if (j!=0)
    {
      i = r1-rdes;
      j = r2-rdes;
      conn(i, j);
      r1->isconn[j] = TRUE;
      r2->isconn[i] = TRUE;
    }
  }
  passnum();
}

//door: Add a door or possibly a secret door.  Also enters the door in the exits array of the room.
void door(struct Room *rm, Coord *cp)
{
  int xit;

  if (rnd(10)+1<get_level() && rnd(5)==0)
  {
    set_tile(cp->y, cp->x, (cp->y==rm->pos.y || cp->y==rm->pos.y+rm->size.y-1)?HWALL:VWALL);
    unset_flag(cp->y, cp->x, F_REAL);
  }
  else set_tile(cp->y, cp->x, DOOR);
  xit = rm->num_exits++;
  rm->exits[xit].y = cp->y;
  rm->exits[xit].x = cp->x;
}

//add_pass: Add the passages to the current window (wizard command)
void add_pass()
{
  int y, x, ch;

  for (y = 1; y<maxrow; y++) for (x = 0; x<COLS; x++) if ((ch = get_tile(y, x))==DOOR || ch==PASSAGE) mvaddch(y, x, ch);
}

//passnum: Assign a number to each passageway
void passnum()
{
  struct Room *room;
  int i;

  pnum = 0;
  newpnum = FALSE;
  for (room = passages; room<&passages[MAXPASS]; room++) room->num_exits = 0;
  for (room = rooms; room<&rooms[MAXROOMS]; room++) for (i = 0; i<room->num_exits; i++)
  {
    newpnum++;
    numpass(room->exits[i].y, room->exits[i].x);
  }
}

//numpass: Number a passageway square and its brethren
void numpass(int y, int x)
{
  byte fp;
  struct Room *room;
  byte ch;

  if (offmap(y, x)) return;
  fp = get_flags(y, x);
  if (fp&F_PNUM) return;
  if (newpnum) {pnum++; newpnum = FALSE;}
  //check to see if it is a door or secret door, i.e., a new exit, or a numberable type of place
  if ((ch = get_tile(y, x))==DOOR || (!(fp&F_REAL) && ch!=FLOOR))
  {
    room = &passages[pnum];
    room->exits[room->num_exits].y = y;
    room->exits[room->num_exits++].x = x;
  }
  else if (!(fp&F_PASS)) return;
  set_flag(y, x, pnum);
  //recurse on the surrounding places
  numpass(y+1, x);
  numpass(y-1, x);
  numpass(y, x+1);
  numpass(y, x-1);
}

void psplat(int y, int x)
{
  set_tile(y, x, PASSAGE);
  set_flag(y, x, F_PASS);
}