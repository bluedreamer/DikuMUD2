#include "essential.h"
#include "textutil.h"

#include <cassert>
#include <cctype>
#include <cstdlib>
#include <cstring>

auto parse_match(char *pData, const char *pMatch) -> char *
{
   char *pTmp1;
   char *pTmp2;

   pTmp1 = str_cstr(pData, pMatch);
   if(pTmp1 == nullptr)
   {
      return nullptr;
   }

   pTmp2 = skip_blanks(pTmp1 + strlen(pMatch));
   if(*pTmp2 != '=')
   {
      return nullptr;
   }

   pTmp2 = skip_blanks(pTmp2 + 1);

   if(*pTmp2 != 0)
   {
      return pTmp2;
   }
   return nullptr;
}

auto parse_name(char **pData) -> char *
{
   char  Buf[MAX_STRING_LENGTH];
   char *pTmp1;
   char *pTmp2;

   Buf[0] = 0;

   pTmp1 = skip_blanks(*pData);
   if(*pTmp1 != '~')
   {
      return nullptr;
   }

   pTmp1++;

   for(pTmp2 = pTmp1; *pTmp2 != 0; pTmp2++)
   {
      if(*pTmp2 == '~')
      {
         break;
      }
   }

   if(*pTmp2 != '~')
   {
      return nullptr;
   }

   memcpy(Buf, pTmp1, pTmp2 - pTmp1);
   Buf[pTmp2 - pTmp1] = 0;

   *pData = pTmp1 + (1 + pTmp2 - pTmp1); /* +1 to skip the last ~ */

   return str_dup(Buf);
}

auto parse_num(char **pData, int *pNum) -> int
{
   char *pTmp1;

   *pNum = 0;

   pTmp1 = skip_blanks(*pData);
   if((isdigit(*pTmp1) == 0) && *pTmp1 != '+' && *pTmp1 != '-')
   {
      return static_cast<int>(FALSE);
   }

   *pNum = atoi(pTmp1);
   if(*pTmp1 == '+' || *pTmp1 == '-')
   {
      pTmp1++;
   }

   while(isdigit(*pTmp1) != 0)
   {
      pTmp1++;
   }

   if((isspace(*pTmp1) == 0) && *pTmp1 != 0)
   {
      return static_cast<int>(FALSE);
   }

   *pData = pTmp1;

   return static_cast<int>(TRUE);
}

auto parse_numlist(char **pData, int *int_count) -> int *
{
   int *nums  = nullptr;
   int  count = 0;
   int  i;

   while(parse_num(pData, &i) != 0)
   {
      count++;
      if(count == 1)
      {
         CREATE(nums, int, count);
      }
      else
      {
         RECREATE(nums, int, count);
      }

      nums[count - 1] = i;
      while(((isspace(**pData) != 0) || (ispunct(**pData) != 0)))
      {
         (*pData)++;
      }
   }

   *int_count = count;
   return nums;
}

auto parse_namelist(char **pData) -> char **
{
   char  *pTmp1;
   char  *pTmp2;
   char **pNamelist;

   pTmp1 = *pData;

   pNamelist = create_namelist();

   while((pTmp2 = parse_name(&pTmp1)) != nullptr)
   {
      pNamelist = add_name(pTmp2, pNamelist);
      while(*pTmp1 != '~' && ((isspace(*pTmp1) != 0) || (ispunct(*pTmp1) != 0)))
      {
         pTmp1++;
      }
      free(pTmp2);
   }

   if(pNamelist[0] == nullptr)
   {
      free_namelist(pNamelist);
      return nullptr;
   }

   *pData = pTmp1;

   return pNamelist;
}

auto parse_match_num(char **pData, const char *pMatch, int *pResult) -> int
{
   char *pTmp;

   pTmp = parse_match(*pData, pMatch);
   if(pTmp == nullptr)
   {
      return static_cast<int>(FALSE);
   }

   if((parse_num(&pTmp, pResult)) == 0)
   {
      return static_cast<int>(FALSE);
   }

   *pData = pTmp;

   return static_cast<int>(TRUE);
}

auto parse_match_numlist(char **pData, const char *pMatch, int *count) -> int *
{
   char *pTmp;
   int  *i;

   *count = 0;
   pTmp   = parse_match(*pData, pMatch);
   if(pTmp == nullptr)
   {
      return nullptr;
   }

   i = parse_numlist(&pTmp, count);

   *pData = pTmp;

   return i;
}

auto parse_match_name(char **pData, const char *pMatch) -> char *
{
   char *pName;
   char *pTmp;

   pTmp = parse_match(*pData, pMatch);
   if(pTmp == nullptr)
   {
      return nullptr;
   }

   pName = parse_name(&pTmp);

   *pData = pTmp;

   return pName;
}

auto parse_match_namelist(char **pData, const char *pMatch) -> char **
{
   char **pNamelist;
   char  *pTmp;

   pTmp = parse_match(*pData, pMatch);
   if(pTmp == nullptr)
   {
      return nullptr;
   }

   pNamelist = parse_namelist(&pTmp);

   *pData = pTmp;

   return pNamelist;
}
