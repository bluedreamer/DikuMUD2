#include <cstdio>
#include <cstdlib>
#include <ctime>

/* Calculate how many ways you can roll <target> with            */
/*    <num_dice>d<dice_size>  dices                              */
/* <target> must be less than total possible                     */
auto dice_num(int num_dice, int dice_size, int target) -> int
{
   int i;
   int res;

   if(num_dice == 1)
   {
      return static_cast<int>(target <= dice_size);
   }

   for(res = 0, i = 1; (i <= dice_size) && ((target - i) >= 1); i++)
   {
      res += dice_num(num_dice - 1, dice_size, target - i);
   }

   return res;
}

auto number(int from, int to) -> int
{
   return ((rand() % (to - from + 1)) + from);
}

auto dice(int num, int size) -> int
{
   int sum;

   for(sum = 0; num > 0; num--)
   {
      sum += ((rand() % size) + 1);
   }

   return sum;
}

auto main(int argc, char *argv[]) -> int
{
   int num;
   int size;
   int i;
   int j;
   int accum;
   int rev_accum;
   int res;
   int potens;
   int old_res;

   if(argc != 3)
   {
      printf("Syntax:\n");
      printf("Dices <x> <y>\n");
      printf("Where x is number of dices, and y is the dice size.\n");
      exit(0);
   }

   srand((unsigned int)time(nullptr));
   num  = atoi(argv[1]);
   size = atoi(argv[2]);

   for(i = num - 1, potens = size; i > 0; i--)
   {
      potens *= size;
   }
   printf("Probability Chart for %dD%d\n\n", num, size);
   printf("Average : %6.1f\n", (float)(num + num * size) / 2.0);
   printf("Number of possible rolls : %d\n\n", potens);

   printf("          Probability        Accumulated    Reverse Accumulated\n");
   printf("Roll    Stroke  Percent    Stroke  Percent    Stroke  Percent\n");

   rev_accum = potens;
   old_res   = 0;
   for(res = accum = 0, j = num * size, i = num; i <= j; i++)
   {
      res = dice_num(num, size, i);
      accum += res;
      rev_accum -= old_res;

      printf(" %3d   %6d   %6.2f%%   %6d   %6.2f%%   %6d   %6.2f%%\n",
             i,
             res,
             (double)((100.0 * res) / potens),
             accum,
             (double)((100.0 * accum) / potens),
             rev_accum,
             (double)((100.0 * rev_accum) / potens));
      old_res = res;
   }

   return 0;
}
