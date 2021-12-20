#include "comm.h"
#include "common.h"
#include "db.h"
#include "graph.h"
#include "graph_vertice.h"
#include "handler.h"
#include "hob.h"
#include "interpreter.h"
#include "movement.h"
#include "structs.h"
#include "utility.h"
#include "utils.h"
#include "zone_info_type.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#define MAX_EXITS     20 /* Assume a maximum of 20 exits */
#define DIST_INFINITE 1000000

#define ROOM_ZONE_NO(r) (UNIT_FILE_INDEX(r)->zone->zone_no)

#define HOB_KEY(h) ((h)->dist)

#define P(x, h)  (((x)-1) / (h)->d)
#define CF(x, h) ((x) * (h)->d + 1)
#define CL(x, h) std::min((x) * (h)->d + (h)->d, (h)->no)

extern uint32_t memory_total_alloc;
extern int      memory_dijkstra_alloc;

class dir_array
{
public:
   graph_vertice *to_vertice;
   uint8_t        direction;
   int16_t        weight;
};

class izone_type
{
public:
   unit_data *room;
   uint8_t    dir;
};

izone_type **iz = nullptr; /* Global for ease of use :)
                                 I know it's nasty */

static dir_array d_array[MAX_EXITS];

static auto hob_create(graph *g) -> hob *
{
   hob *h;
   int  i;

   CREATE(h, hob, 1);

   h->no = g->no;
   h->d  = 4; /* Optimal is 2+m/n  (m=no edges, n = no vertices) */
   /* I estimate this is approx. 4 (5?) in a world    */
   CREATE(h->array, graph_vertice *, h->no);

   /* We know that the Hob consists of only equal weights */
   /* no need to shift around                             */

   for(i = 0; i < h->no; i++)
   {
      g->array[i].hob_pos = i;
      h->array[i]         = &g->array[i];
   }

   return h;
}

static void hob_shift_up(int x, hob *h)
{
   int            p;
   graph_vertice *tmp;

   p = P(x, h);

   while((x != 0) && (HOB_KEY(h->array[x]) < HOB_KEY(h->array[p])))
   {
      tmp = h->array[p];

      h->array[p] = h->array[x];
      h->array[x] = tmp;
      SWITCH(h->array[p]->hob_pos, h->array[x]->hob_pos);

      x = p;
      p = P(x, h);
   }
}

static auto hob_min_child(int x, hob *h) -> int
{
   int b;
   int minb;
   int b1;
   int b2;

   b1 = CF(x, h);
   b2 = CL(x, h);

   if(b1 > h->no)
   {
      return -1;
   }
   minb = b1;
   for(b = b1 + 1; b <= b2; b++)
   {
      if(HOB_KEY(h->array[b]) < HOB_KEY(h->array[minb]))
      {
         minb = b;
      }
   }
   return minb;
}

static void hob_shift_down(int x, hob *h)
{
   int            b;
   graph_vertice *tmp;

   b = hob_min_child(x, h);

   while((b != -1) && (HOB_KEY(h->array[b]) < HOB_KEY(h->array[x])))
   {
      tmp = h->array[b];

      h->array[b] = h->array[x];
      h->array[x] = tmp;
      SWITCH(h->array[b]->hob_pos, h->array[x]->hob_pos);

      x = b;
      b = hob_min_child(x, h);
   }
}

static void hob_remove(int x, hob *h)
{
   graph_vertice *s;
   graph_vertice *s1;

   s  = h->array[x];
   s1 = h->array[h->no - 1];

   h->no--;

   if(s != s1) /* Already one subtracted above */
   {
      h->array[x]          = s1;
      h->array[x]->hob_pos = x;

      if(HOB_KEY(s) >= HOB_KEY(s1))
      {
         hob_shift_up(x, h);
      }
      else
      {
         hob_shift_down(x, h);
      }
   }

   /* There is no need to make it physically less        */
   /* RECREATE(h->array, struct graph_vertice *, h->no); */
}

static auto hob_remove_min(hob *h) -> graph_vertice *
{
   graph_vertice *s;

   if(h->no == 0)
   {
      return nullptr;
   }
   s = h->array[0];

   hob_remove(0, h);

   return s;
}

static auto flag_weight(int flags) -> int
{
   int weight;

   if(flags == 0)
   {
      return 1;
   }

   weight = 0;

   if(IS_SET(flags, EX_CLOSED))
   {
      weight += 25;
   }

   if(IS_SET(flags, EX_LOCKED))
   {
      weight += 200;
   }

   if(IS_SET(flags, EX_OPEN_CLOSE))
   {
      weight += 10;
   }

   if(!IS_SET(flags, EX_OPEN_CLOSE) && IS_SET(flags, EX_CLOSED))
   {
      weight = 1000;
   }

   return weight;
}

void add_exit(graph *g, graph_vertice *v, unit_data *to, int *idx, int dir, int weight)
{
   int j;

   if(UNIT_FILE_INDEX(v->room)->zone == UNIT_FILE_INDEX(to)->zone)
   {
      d_array[*idx].weight = weight; /* Perhaps adjust for movement type */

      /* Calculate array entry for vertice of graph */
      j = UNIT_FILE_INDEX(to)->room_no;

      d_array[*idx].to_vertice = &g->array[j];
      d_array[*idx].direction  = dir;
      (*idx)++;
   }
   else /* different zones! */
   {
      /* Save this path in inter-zone matrix if not already there */
      if((iz[ROOM_ZONE_NO(v->room)][ROOM_ZONE_NO(to)].room == nullptr) && (ROOM_LANDSCAPE(v->room) != SECT_WATER_SAIL) &&
         (ROOM_LANDSCAPE(to) != SECT_WATER_SAIL))
      {
         iz[ROOM_ZONE_NO(v->room)][ROOM_ZONE_NO(to)].room = v->room;
         iz[ROOM_ZONE_NO(v->room)][ROOM_ZONE_NO(to)].dir  = dir;
      }
   }
   assert(*idx < MAX_EXITS);
}

static void outedges(graph *g, graph_vertice *v)
{
   unit_data *u;
   int        i;
   int        idx;
   int        weight;
   int        flags;

   idx = 0;

   for(i = 0; i < 6; i++)
   {
      if((ROOM_EXIT(v->room, i) != nullptr) && (ROOM_EXIT(v->room, i)->to_room != nullptr))
      /* If there is an edge between two rooms */
      {
         flags  = ROOM_EXIT(v->room, i)->exit_info;
         weight = flag_weight(flags);

         if(ROOM_LANDSCAPE(ROOM_EXIT(v->room, i)->to_room) == SECT_WATER_SAIL)
         {
            weight += 1000;
         }

         add_exit(g, v, ROOM_EXIT(v->room, i)->to_room, &idx, i, weight);
      }
   }

   /* Scan room for enter rooms */
   for(u = UNIT_CONTAINS(v->room); u != nullptr; u = u->next)
   {
      if(IS_ROOM(u) && IS_SET(UNIT_MANIPULATE(u), MANIPULATE_ENTER))
      {
         flags  = UNIT_OPEN_FLAGS(u);
         weight = 25 + flag_weight(flags);

         add_exit(g, v, u, &idx, DIR_ENTER, weight);
      }
   }

   /* Check room for an exit room */
   if((UNIT_IN(v->room) != nullptr) && IS_ROOM(UNIT_IN(v->room)))
   {
      if(IS_SET(UNIT_MANIPULATE(UNIT_IN(v->room)), MANIPULATE_ENTER))
      {
         flags  = UNIT_OPEN_FLAGS(v->room);
         weight = 25 + flag_weight(flags);

         add_exit(g, v, UNIT_IN(v->room), &idx, DIR_EXIT, weight);
      }
   }

   d_array[idx].to_vertice = nullptr;
}

void dijkstra(graph *g, graph_vertice *source)
{
   hob           *h;
   graph_vertice *v;
   graph_vertice *w;
   int            j;

   h = hob_create(g);

   source->dist      = 0;
   source->direction = DIR_HERE; /* We're at the goal */

   hob_shift_up(source->hob_pos, h);
   hob_remove_min(h); /* Remove S */

   v = source;

   do
   {
      outedges(g, v);
      for(j = 0; (w = d_array[j].to_vertice) != nullptr; j++)
      {
         if((v->dist + d_array[j].weight) < w->dist)
         {
            w->dist   = v->dist + d_array[j].weight;
            w->parent = v;
            if(v->direction <= 7)
            {
               w->direction = v->direction;
               /* I'm not sure this is true, but it will work */
            }
            else
            {
               w->direction = d_array[j].direction;
            }

            hob_shift_up(w->hob_pos, h);
         }
      }
      v = hob_remove_min(h);
   } while(v != nullptr);

   if(h->array != nullptr)
   {
      free(h->array);
   }
   free(h);
}

#define spi_idx(mtr, vidx, hidx) (mtr[vidx][(hidx) >> 1])

#define spi_val(mtr, vidx, hidx) ((hidx & 1) ? (spi_idx((mtr), (vidx), (hidx)) & 0x0F) : ((spi_idx((mtr), (vidx), (hidx)) & 0xF0) >> 4))

/* Given a zone, create the nesseceary graph structure, and  */
/* return a matrix of shortest path for the zone             */
auto create_graph(zone_type *zone) -> uint8_t **
{
   static graph     g;
   file_index_type *fi;
   int              i;
   int              j;
   int              hidx;
   int              vidx;
   uint8_t        **spi;

   g.no = zone->no_rooms;

   if(g.no == 0)
   {
      return nullptr;
   }

   CREATE(spi, uint8_t *, g.no);
   CREATE(g.array, graph_vertice, g.no);

   for(i = 0, fi = zone->fi; fi != nullptr; fi = fi->next)
   {
      if(fi->type == UNIT_ST_ROOM)
      {
         g.array[fi->room_no].room = fi->room_ptr;
         i++;
      }
   }

   for(j = 0; j < g.no; j++)
   {
      for(i = 0; i < g.no; i++)
      {
         g.array[i].parent    = nullptr;
         g.array[i].dist      = DIST_INFINITE;
         g.array[i].direction = DIR_IMPOSSIBLE;
         g.array[i].hob_pos   = 0;
      }

      dijkstra(&g, &g.array[j]);

      vidx = UNIT_FILE_INDEX(g.array[j].room)->room_no;

      CREATE(spi[vidx], uint8_t, (g.no / 2) + (g.no & 1));

      for(i = 0; i < g.no; i++)
      {
         hidx = UNIT_FILE_INDEX(g.array[i].room)->room_no;
         if((hidx & 1) != 0)
         {
            spi_idx(spi, vidx, hidx) |= g.array[i].direction;
         }
         else
         {
            spi_idx(spi, vidx, hidx) |= g.array[i].direction << 4;
         }
      }
   }
   free(g.array);

   return spi;
}

void stat_dijkstraa(unit_data *ch, zone_type *z)
{
   int        i;
   int        j;
   char       buf[MAX_STRING_LENGTH];
   zone_type *z2;
   char      *b;

   sprintf(buf, "%s borders the following zones (for auto-walk):\n\r\n\r", z->name);
   send_to_char(buf, ch);

   for(i = 0; i < zone_info.no_of_zones; i++)
   {
      if((i != z->zone_no) && (iz[z->zone_no][i].room != nullptr))
      {
         for(z2 = zone_info.zone_list, j = 0; j < zone_info.no_of_zones; j++, z2 = z2->next)
         {
            if(z2->zone_no == i)
            {
               break;
            }
         }

         b = buf;

         /* Not a primary link - indent! */
         if(iz[z->zone_no][i].dir == DIR_ENTER)
         {
            strcpy(b, "E ");
         }
         else if(iz[z->zone_no][i].dir == DIR_EXIT)
         {
            strcpy(b, "L ");
         }
         else if(iz[z->zone_no][i].dir == DIR_IMPOSSIBLE)
         {
            strcpy(b, "I ");
         }
         else if(iz[z->zone_no][i].dir == DIR_HERE)
         {
            strcpy(b, "H ");
         }
         else
         {
            assert(is_in(iz[z->zone_no][i].dir, 0, 5));

            if(z2 != UNIT_FILE_INDEX(ROOM_EXIT(iz[z->zone_no][i].room, iz[z->zone_no][i].dir)->to_room)->zone)
            {
               strcpy(b, "  ");
            }
            else
            {
               strcpy(b, "+ ");
            }
         }

         TAIL(b);

         if(is_in(iz[z->zone_no][i].dir, 0, 5) != 0)
         {
            sprintf(b,
                    "To %s via %s@%s to %s@%s\n\r",
                    z2->name,
                    UNIT_FI_NAME(iz[z->zone_no][i].room),
                    UNIT_FI_ZONENAME(iz[z->zone_no][i].room),
                    UNIT_FI_NAME(ROOM_EXIT(iz[z->zone_no][i].room, iz[z->zone_no][i].dir)->to_room),
                    UNIT_FI_ZONENAME(ROOM_EXIT(iz[z->zone_no][i].room, iz[z->zone_no][i].dir)->to_room));
         }
         else
         {
            sprintf(b,
                    "To %s via %s@%s (enter / leave / here) \n\r",
                    z2->name,
                    UNIT_FI_NAME(iz[z->zone_no][i].room),
                    UNIT_FI_ZONENAME(iz[z->zone_no][i].room));
         }
         send_to_char(buf, ch);
      }
   }
}

void create_dijkstra()
{
   zone_type *z;
   int        i;
   int        j;
   int        k;

#ifdef MEMORY_DEBUG
   memory_dijkstra_alloc = memory_total_alloc;
#endif
   CREATE(iz, izone_type *, zone_info.no_of_zones);
   zone_info.spmatrix = (void **)iz;

   /* Initialize inter-zone matrix */
   for(i = 0; i < zone_info.no_of_zones; i++)
   {
      CREATE(iz[i], izone_type, zone_info.no_of_zones);
      for(j = 0; j < zone_info.no_of_zones; j++)
      {
         iz[i][j].room = nullptr;
         iz[i][j].dir  = DIR_IMPOSSIBLE;
      }
   }

   /* Create shortest path matrix for each individual zone */
   for(z = zone_info.zone_list; z != nullptr; z = z->next)
   {
      slog(LOG_OFF, 0, "Creating shortest path for %s", z->name);
      z->spmatrix = create_graph(z);
   }

   /* Create inter-zone path info (not shortest) */
   /* Using pseudo-Warshall algorithm            */
   slog(LOG_OFF, 0, "Creating inter-zone path information.");

   for(k = 0; k < zone_info.no_of_zones; k++)
   {
      for(i = 0; i < zone_info.no_of_zones; i++)
      {
         for(j = 0; j < zone_info.no_of_zones; j++)
         {
            if((iz[i][j].room == nullptr) && (iz[i][k].room != nullptr) && (iz[k][j].room != nullptr))
            {
               iz[i][j] = iz[i][k];
            }
         }
      }
   }

#ifdef MEMORY_DEBUG
   memory_dijkstra_alloc = memory_total_alloc - memory_dijkstra_alloc;
#endif
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* Primitive move generator, returns direction */
auto move_to(const unit_data *from, const unit_data *to) -> int
{
   int i;
   int j;

   /* Assertion is that both from and to are rooms! */
   if(!IS_ROOM(from) || !IS_ROOM(to))
   {
      return DIR_IMPOSSIBLE;
   }

   if(UNIT_FILE_INDEX(from)->zone == (UNIT_FILE_INDEX(to)->zone))
   {
      if(UNIT_FILE_INDEX(to)->zone->spmatrix != nullptr)
      {
         i = UNIT_FILE_INDEX(from)->room_no;

         j = UNIT_FILE_INDEX(to)->room_no;

         return spi_val(UNIT_FILE_INDEX(to)->zone->spmatrix, i, j);
      }
      return DIR_IMPOSSIBLE;
   }
   /* Inter-zone path info needed */
   if(iz[UNIT_FILE_INDEX(from)->zone->zone_no][UNIT_FILE_INDEX(to)->zone->zone_no].room != nullptr)
   {
      i = move_to(from, iz[UNIT_FILE_INDEX(from)->zone->zone_no][UNIT_FILE_INDEX(to)->zone->zone_no].room);
      if(i == DIR_HERE)
      {
         return iz[UNIT_FILE_INDEX(from)->zone->zone_no][UNIT_FILE_INDEX(to)->zone->zone_no].dir;
      }
      return i;
   }
   return DIR_IMPOSSIBLE; /* Zone is unreachable */
}

/* This function is used with 'npc_move'  */
auto npc_stand(const unit_data *npc) -> int
{
   if((CHAR_POS(npc) < POSITION_SLEEPING) || (CHAR_POS(npc) == POSITION_FIGHTING))
   {
      return MOVE_BUSY; /* NPC is very busy */
   }
   if(CHAR_POS(npc) == POSITION_SLEEPING)
   {
      char mbuf[MAX_INPUT_LENGTH] = {0};
      do_wake((unit_data *)npc, mbuf, &cmd_auto_unknown);
      return MOVE_BUSY; /* Still busy, NPC is now sitting */
   }

   char mbuf[MAX_INPUT_LENGTH] = {0};
   do_stand((unit_data *)npc, mbuf, &cmd_auto_unknown);
   return MOVE_BUSY;
}

auto open_door(const unit_data *npc, int dir) -> int
{
   char buf[80];

   assert(IS_ROOM(UNIT_IN(npc)));
   assert(ROOM_EXIT(UNIT_IN(npc), dir));

   if(IS_SET(ROOM_EXIT(UNIT_IN(npc), dir)->exit_info, EX_OPEN_CLOSE) && IS_SET(ROOM_EXIT(UNIT_IN(npc), dir)->exit_info, EX_CLOSED))
   {
      sprintf(buf, "%s %s", dirs[dir], ROOM_EXIT(UNIT_IN(npc), dir)->open_name.Name());

      /* The door is closed and can be opened */
      if(IS_SET(ROOM_EXIT(UNIT_IN(npc), dir)->exit_info, EX_LOCKED))
      {
         do_unlock((unit_data *)npc, buf, &cmd_auto_unknown);
         if(IS_SET(ROOM_EXIT(UNIT_IN(npc), dir)->exit_info, EX_LOCKED))
         {
            return MOVE_FAILED;
         }
         return MOVE_BUSY;
      }

      do_open((unit_data *)npc, buf, &cmd_auto_unknown);
      if(IS_SET(ROOM_EXIT(UNIT_IN(npc), dir)->exit_info, EX_CLOSED))
      {
         return MOVE_FAILED;
      }
      return MOVE_BUSY;
   }

   return MOVE_FAILED;
}

auto enter_open(const unit_data *npc, const unit_data *enter) -> int
{
   char buf[80];

   assert(UNIT_IN(npc) == UNIT_IN(enter));

   if(IS_SET(UNIT_OPEN_FLAGS(enter), EX_OPEN_CLOSE) && IS_SET(UNIT_OPEN_FLAGS(enter), EX_CLOSED))
   {
      sprintf(buf, "%s", UNIT_NAME((unit_data *)enter));

      /* The door is closed and can be opened */
      if(IS_SET(UNIT_OPEN_FLAGS(enter), EX_LOCKED))
      {
         do_unlock((unit_data *)npc, buf, &cmd_auto_unknown);
         if(IS_SET(UNIT_OPEN_FLAGS(enter), EX_LOCKED))
         {
            return MOVE_FAILED;
         }
         return MOVE_BUSY;
      }

      do_open((unit_data *)npc, buf, &cmd_auto_unknown);
      if(IS_SET(UNIT_OPEN_FLAGS(enter), EX_CLOSED))
      {
         return MOVE_FAILED;
      }
      return MOVE_BUSY;
   }

   return MOVE_FAILED;
}

auto exit_open(const unit_data *npc) -> int
{
   char       buf[80];
   unit_data *enter;

   enter = UNIT_IN(npc);

   if(IS_SET(UNIT_OPEN_FLAGS(enter), EX_OPEN_CLOSE) && IS_SET(UNIT_OPEN_FLAGS(enter), EX_CLOSED) &&
      IS_SET(UNIT_OPEN_FLAGS(enter), EX_INSIDE_OPEN))
   {
      sprintf(buf, "%s", UNIT_NAME(enter));

      /* The door is closed and can be opened */
      if(IS_SET(UNIT_OPEN_FLAGS(enter), EX_LOCKED))
      {
         do_unlock((unit_data *)npc, buf, &cmd_auto_unknown);
         if(IS_SET(UNIT_OPEN_FLAGS(enter), EX_LOCKED))
         {
            return MOVE_FAILED;
         }
         return MOVE_BUSY;
      }

      do_open((unit_data *)npc, buf, &cmd_auto_unknown);
      if(IS_SET(UNIT_OPEN_FLAGS(enter), EX_CLOSED))
      {
         return MOVE_FAILED;
      }
      return MOVE_BUSY;
   }

   return MOVE_FAILED;
}

auto npc_move(const unit_data *npc, const unit_data *to) -> int
{
   int        i;
   int        dir;
   unit_data *in;
   unit_data *u;

   /*    Returns: 1=succes, 0=fail, -1=dead.    */

   if(!IS_ROOM(to))
   {
      return MOVE_FAILED; /* How can we move to anything but rooms? */
   }

   if(UNIT_FILE_INDEX(to)->zone->spmatrix == nullptr)
   {
      return MOVE_FAILED;
   }

   if(CHAR_POS(npc) < POSITION_STANDING)
   {
      return npc_stand(npc);
   }

   if(!IS_ROOM(in = UNIT_IN(npc)))
   {
      char mbuf[MAX_INPUT_LENGTH] = {0};
      do_exit((unit_data *)npc, mbuf, &cmd_auto_unknown);
      if(in == UNIT_IN(npc))
      { /* NPC couldn't leave */
         return exit_open(npc);
      }

      return MOVE_CLOSER; /* We approached a room */
   }

   dir = move_to(in, to);

   if(dir <= DOWN)
   {
      assert(ROOM_EXIT(UNIT_IN(npc), dir));

      if(IS_SET(ROOM_EXIT(UNIT_IN(npc), dir)->exit_info, EX_CLOSED))
      {
         return open_door(npc, dir);
      }

      i = do_advanced_move((unit_data *)npc, dir, static_cast<int>(TRUE));
      if(i == -1)
      {
         return MOVE_DEAD; /* NPC died */
      }
      if(i == 1)
         return MOVE_CLOSER; /* The NPC was moved closer */

      /* Something (not closed) prevented the NPC from moving     */
      return MOVE_FAILED;
   }
   if(dir == DIR_ENTER)
   {
      for(u = UNIT_CONTAINS(UNIT_IN(npc)); u != nullptr; u = u->next)
      {
         if(IS_ROOM(u) && IS_SET(UNIT_MANIPULATE(u), MANIPULATE_ENTER))
         {
            if(move_to((unit_data *)u, to) != DIR_EXIT)
            {
               if(IS_SET(UNIT_OPEN_FLAGS(u), EX_CLOSED))
               {
                  return enter_open(npc, u);
               }
               do_enter((unit_data *)npc, (char *)UNIT_NAME(u), &cmd_auto_unknown);
               if(in == UNIT_IN(npc))
                  return MOVE_FAILED;
               else
                  return MOVE_CLOSER;
            }
         }
      }
      return MOVE_FAILED;
   }
   else if(dir == DIR_EXIT)
   {
      u = UNIT_IN(UNIT_IN(npc));
      if((u != nullptr) && IS_ROOM(u) && IS_SET(UNIT_MANIPULATE(u), MANIPULATE_ENTER))
      {
         if(IS_SET(UNIT_OPEN_FLAGS(u), EX_CLOSED))
         {
            return exit_open(npc);
         }
         char mbuf[MAX_INPUT_LENGTH] = {0};
         do_exit((unit_data *)npc, mbuf, &cmd_auto_unknown);
         if(in == UNIT_IN(npc))
            return MOVE_FAILED;
         else
            return MOVE_CLOSER;
      }
      return MOVE_FAILED;
   }
   else if(dir == DIR_IMPOSSIBLE)
   {
      return MOVE_FAILED;
   }
   else if(dir == DIR_HERE)
   {
      return MOVE_GOAL;
   }

   slog(LOG_ALL, 0, "Error: In very high IQ monster move!");
   return MOVE_FAILED;
}
