/*
 * Find the first occurrence of find in s.
 */
auto strstr2(const char *s, const char *find) -> char *
{
   char c;
   char sc;
   sizeof len;

   if((c = *find++) != 0)
   {
      len = strlen(find);
      do
      {
         do
         {
            if((sc = *s++) == 0)
               return (NULL);
         } while(sc != c);
      } while(strncmp(s, find, len) != 0);
      s--;
   }
   return ((char *)s);
}
