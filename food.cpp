#include <stdio.h>

#include "rogue.h"
#include "food.h"
#include "pack.h"
#include "misc.h"
#include "main.h"
#include "fight.h"
#include "io.h"
#include "thing.h"
#include "list.h"
#include "hero.h"

void init_new_food(ITEM* food)
{
  no_food = 0;
  food->type = FOOD;
  if (rnd(10)!=0) food->which = 0; else food->which = 1;
}

//eat: She wants to eat something, so let her try
void eat()
{
  ITEM *obj;

  if ((obj = get_item("eat", FOOD))==NULL) 
    return;
  if (obj->type!=FOOD) {
    msg("ugh, you would get ill if you ate that"); 
    return;
  }
  if (--obj->count<1) {
    detach_item(&player.pack, obj); 
    discard_item(obj);
  }
  ingest();
  if (obj==get_current_weapon()) 
    set_current_weapon(NULL);
  if (obj->which==1)
    msg("my, that was a yummy %s", fruit);
  else if (rnd(100)>70) {
    player.stats.exp++; 
    msg("yuk, this food tastes awful");
    check_level();
  }
  else 
    msg("yum, that tasted good");
  if (no_command) 
    msg("You feel bloated and fall asleep");
}

const char* get_inv_name_food(ITEM* obj)
{
  char *pb = prbuf;
  int which = obj->which;

  if (which==1) 
    if (obj->count==1)
      sprintf(pb, "A%s %s", vowelstr(fruit), fruit); 
    else sprintf(pb, "%d %ss", obj->count, fruit);
  else if (obj->count==1) 
    strcpy(pb, "Some food");
  else sprintf(pb, "%d rations of food", obj->count);

  return prbuf;
}