/************************************************************************/
/*									*/
/*	File:	pp5.c							*/
/*									*/
/*	Conditional processing.						*/
/*									*/
/*	Written by:							*/
/*			Gary Oliver					*/
/*			3420 NW Elmwood Dr.				*/
/*			PO Box 826					*/
/*			Corvallis, Oregon 97339				*/
/*			(503)758-5549					*/
/*	Maintained by:							*/
/*			Kirk Bailey					*/
/*			Logical Systems					*/
/*			P.O. Box 1702					*/
/*			Corvallis, OR 97339				*/
/*			(503)753-9051					*/
/*									*/
/*	This program is hereby placed in the public domain.  In		*/
/*	contrast to other claims of "public domain", this means no	*/
/*	copyright is claimed and you may do anything you like with PP,	*/
/*	including selling it!  As a gesture of courtesy, please retain	*/
/*	the authorship information in the source code and		*/
/*	documentation.							*/
/*									*/
/*	Functions contained within this module:				*/
/*									*/
/*		doelse		Process #else/#elif directive.		*/
/*		doendif		Process #endif directive.		*/
/*		doif		Process #if directive.			*/
/*		doifs		Do work for #ifdef/#ifndef stuff.	*/
/*									*/
/************************************************************************/

#include "pp.h"

/************************************************************************/
/*									*/
/*	doelse								*/
/*									*/
/*	Process #else/#elif statement.					*/
/*									*/
/************************************************************************/

void doelse(int elif, int abcd, const char *dcba)
{
#if PPDEBUG
   if(PPDEBUG)
      printf("doelse: %d\n", Iflevel);
#endif /* PPDEBUG */
   if(Iflevel != 0)
   {
      /* We are processing an if */
      if(Ifstack[Iflevel].i_else != 0)
      {
         non_fatal("\"#else\" already encountered", "");
      }
      else
      {
         if(Ifstate == IFTRUE)
         {
            Ifstate = Ifstack[Iflevel].i_state = IFNEVER;
         }
         else if(Ifstate == IFFALSE)
         {
            if(elif != 0)
            {
               /*
                *	Note that we first need to pretend that we are in a TRUE branch so
                *	that routines which "eval" causes to be called will correctly process
                *	the "elif" expression (in particular "_docall").
                */
               Ifstate = Ifstack[Iflevel].i_state = IFTRUE;
               Ifstate = Ifstack[Iflevel].i_state = eval() != 0 ? IFTRUE : IFFALSE;
            }
            else
            {
               Ifstate = Ifstack[Iflevel].i_state = IFTRUE;
            }
         }
         /* If have seen an #else */
         Ifstack[Iflevel].i_else = static_cast<char>(elif) == 0;
      }
   }
   else
   {
      if(elif != 0)
      {
         non_fatal(R"("#elif" outside of "#if")", "");
      }
      else
      {
         non_fatal(R"("#else" outside of "#if")", "");
      }
   }
}

/************************************************************************/
/*									*/
/*	doendif								*/
/*									*/
/*	Process #endif statement.					*/
/*									*/
/************************************************************************/

void doendif(int aaa, int bbb, const char *ccc)
{
#if PPDEBUG
   if(PPDEBUG)
      printf("doendif: %d\n", Iflevel);
#endif /* PPDEBUG */
   if(Iflevel != 0)
   {
      Ifstate = Ifstack[--Iflevel].i_state; /* Pop stack */
   }
   else
   {
      non_fatal(R"("#endif" outside of "#if")", "");
   }
}

/************************************************************************/
/*									*/
/*	doif								*/
/*									*/
/*	Process #if statement.						*/
/*									*/
/************************************************************************/

void doif(int aaa, int bbb, const char *ccc)
{
   if(Iflevel >= IFSTACKSIZE)
   {
      non_fatal("\"#if\" stack overflow", "");
   }
   else
   {
      if(Ifstate == IFTRUE)
      {
         Ifstate = Ifstack[++Iflevel].i_state = eval() != 0 ? IFTRUE : IFFALSE;
      }
      else
      {
         Ifstate = Ifstack[++Iflevel].i_state = IFNEVER; /* NO #else */
         scaneol();                                      /* Just absorb the rest */
      }
      Ifstack[Iflevel].i_else = FALSE; /* No #else seen */
   }
}

/************************************************************************/
/*									*/
/*	doifs								*/
/*									*/
/*	Process some kind of #ifxdef statement.				*/
/*									*/
/*	Lookup token and set Ifstack to inform output routine to	*/
/*	emit code or not.						*/
/*									*/
/************************************************************************/

void doifs(int t, int bbb, const char *ccc)
{
   int iftype;

   if(Ifstate == IFTRUE)
   {
      /* Get next non-space token */
      if(getnstoken(GT_STR) == LETTER)
      {
#if PPDEBUG
         if(PPDEBUG)
            printf("doifs: %d %s", t, Token);
#endif /* PPDEBUG */
         iftype = ((lookup(Token, nullptr) != nullptr ? TRUE : FALSE) ^ (t != 0 ? FALSE : TRUE)) != 0 ? IFTRUE : IFFALSE;
      }
      else
      {
         iftype = IFFALSE;
         illegal_symbol();
      }
   }
   else
   {
      iftype = IFNEVER; /* Inside false #if -- No #else */
   }

   if(Iflevel >= IFSTACKSIZE)
   {
      non_fatal("\"#if\" stack overflow", "");
   }
   else
   {
      Ifstate = Ifstack[++Iflevel].i_state = iftype;
      Ifstack[Iflevel].i_else              = FALSE;
   }

#if PPDEBUG
   if(PPDEBUG)
      printf("doifs: %d %d %d\n", t, iftype, Iflevel);
#endif /* PPDEBUG */
}
