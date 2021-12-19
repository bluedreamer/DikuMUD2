main()
{
   static char array[10000];
   int         i, t;
   for(i = 2; i < 10000; i++)
      if(!array[i])
      {
         printf("%d\n", i);
         for(t = i * 2; t < 10000; t += i)
            array[t] = 1;
      }
}
