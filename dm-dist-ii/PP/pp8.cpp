/************************************************************************/
/*									*/
/*	File:	pp8.c							*/
/*									*/
/*	Expression evaluator.						*/
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
/*		eval		Top level eval function.		*/
/*		evaltern	Evaluate ?: operator.			*/
/*		evallor		Evaluate || operator.			*/
/*		evalland	Evaluate && operator			*/
/*		evalbor		Evaluate | operator.			*/
/*		evalbxor	Evaluate ^ operator.			*/
/*		evalband	Evaluate & operator.			*/
/*		evaleq		Evaluate == and != operators.		*/
/*		evalrel		Evaluate < <= > and >= operators.	*/
/*		evalsh		Evaluate << and >> operators.		*/
/*		evalsum		Evaluate + and - operators.		*/
/*		evalmdr		Evaluate * / and % operators.		*/
/*		evalfuns	Evaluate functions ("defined", etc.)	*/
/*		evalucom	Evaluate unary ~ operator.		*/
/*		evalunot	Evaluate unary ! operator.		*/
/*		evalumin	Evaluate unary - operator.		*/
/*		evalval		Evaluate a value.			*/
/*		hexbin		Convert hex digit to binary.		*/
/*		ishex		Returns TRUE if char is hex digit.	*/
/*		isoct		Returns TRUE if char is octal digit.	*/
/*		item		Fetch next token.			*/
/*		look		Look ahead for matching string.		*/
/*		match		Read tokens while match and return str.	*/
/*		readexpline	Read a line and macro expand for expr.	*/
/*		test		Test (read and absorb) matching tokens.	*/
/*									*/
/************************************************************************/

#include "pp.h"

/************************************************************************/
/*									*/
/*	eval								*/
/*									*/
/*	Recursive descent expression parser.  Returns the value		*/
/*	of the expression.  Errors detected during parsing cause	*/
/*	calls to perr().  Tokens are fetched using getnstoken().	*/
/*									*/
/************************************************************************/

auto eval() -> EVALINT
{
   char    buf[TOKENSIZE];
   EVALINT val;

   pbcstr(readexpline(buf, TOKENSIZE)); /* Macro expand line */

   for(;;)
   {
      val = evaltern(); /* Top level ternary form */

      if(Token[0] != ',')
      {
         if(Token[0] != '\n')
         {
            non_fatal("Expression: Invalid operator: ", Token);
         }
         else
         {
            pushback('\n'); /* Put eol back out */
         }
         break;
      }
   }
   return (val);
}

/************************************************************************/
/*									*/
/*	evaltern							*/
/*									*/
/*	?: operator							*/
/*									*/
/************************************************************************/

auto evaltern() -> EVALINT
{
   EVALINT val;
   EVALINT val_f;
   EVALINT val_t;

   val = evallor(); /* Look for expression */
   if(test("?") == 0)
   { /* ? part of ?: */
      return (val);
   }

   val_t = eval(); /* Get true-side expression */
   if(test(":") == 0)
   {
      non_fatal("Expression: ':' expected", "");
      return ((EVALINT)0);
   }
   val_f = eval(); /* Get false-side expression */

   return (val != 0 ? val_t : val_f); /* Use it or loose it */
}

/************************************************************************/
/*									*/
/*	|| operator							*/
/*									*/
/************************************************************************/

auto evallor() -> EVALINT
{
   EVALINT val;

   for(val = evalland(); test("||") != 0;)
   {
      if(evalland() != 0)
      {
         val = TRUE;
      }
   }
   return (val);
}

/************************************************************************/
/*									*/
/*	&& operator							*/
/*									*/
/************************************************************************/

auto evalland() -> EVALINT
{
   EVALINT val;

   for(val = evalbor(); test("&&") != 0;)
   {
      /*lint -e503 lint doesn't like boolean arguments to relationals */
      if(static_cast<int>(static_cast<int>(evalbor()) == 0) != 0)
      {
         /*lint +e503 lint doesn't like boolean arguments to relationals */
         val = FALSE;
      }
   }
   return (val);
}

/************************************************************************/
/*									*/
/*	| operator							*/
/*									*/
/************************************************************************/

auto evalbor() -> EVALINT
{
   EVALINT val;

   for(val = evalbxor(); (look("||") == 0) && (test("|") != 0);)
   {
      val = val | evalbxor();
   }

   return (val);
}

/************************************************************************/
/*									*/
/*	^ operator							*/
/*									*/
/************************************************************************/

auto evalbxor() -> EVALINT
{
   EVALINT val;

   for(val = evalband(); test("^") != 0;)
   {
      val = val ^ evalband();
   }

   return (val);
}

/************************************************************************/
/*									*/
/*	& operator							*/
/*									*/
/************************************************************************/

auto evalband() -> EVALINT
{
   EVALINT val;

   for(val = evaleq(); (look("&&") == 0) && (test("&") != 0);)
   {
      val = val & evaleq();
   }

   return (val);
}

/************************************************************************/
/*									*/
/*	== and != operators						*/
/*									*/
/************************************************************************/

auto evaleq() -> EVALINT
{
   EVALINT val;

   for(val = evalrel();;)
   {
      if(test("==") != 0)
      {
         val = static_cast<long>(val == evalrel());
      }
      else if(test("!=") != 0)
      {
         val = static_cast<long>(val != evalrel());
      }
      else
      {
         break;
      }
   }
   return (val);
}

/************************************************************************/
/*									*/
/*	< > <= and >= operators						*/
/*									*/
/************************************************************************/

auto evalrel() -> EVALINT
{
   EVALINT val;

   for(val = evalsh();;)
   {
      if(test("<=") != 0)
      {
         val = static_cast<long>(val <= evalsh());
      }
      else if(test(">=") != 0)
      {
         val = static_cast<long>(val >= evalsh());
      }
      else if(test("<") != 0)
      {
         val = static_cast<long>(val < evalsh());
      }
      else if(test(">") != 0)
      {
         val = static_cast<long>(val > evalsh());
      }
      else
      {
         break;
      }
   }
   return (val);
}

/************************************************************************/
/*									*/
/*	<< and >> operators						*/
/*									*/
/************************************************************************/

auto evalsh() -> EVALINT
{
   EVALINT val;

   for(val = evalsum();;)
   {
      if(test("<<") != 0)
      {
         val = val << evalsum();
      }
      else if(test(">>") != 0)
      {
         val = val >> evalsum();
      }
      else
      {
         break;
      }
   }
   return (val);
}

/************************************************************************/
/*									*/
/*	+ and - operators						*/
/*									*/
/************************************************************************/

auto evalsum() -> EVALINT
{
   EVALINT val;

   for(val = evalmdr();;)
   {
      if(test("+") != 0)
      {
         val = val + evalmdr();
      }
      else if(test("-") != 0)
      {
         val = val - evalmdr();
      }
      else
      {
         break;
      }
   }
   return (val);
}

/************************************************************************/
/*									*/
/*	* / and % operators						*/
/*									*/
/************************************************************************/

auto evalmdr() -> EVALINT
{
   EVALINT temp;
   EVALINT val;

   for(val = evalfuns();;)
   {
      if(test("*") != 0)
      {
         val = val * evalfuns();
      }
      else if(test("/") != 0)
      {
         temp = evalfuns();
         if(temp == 0)
         {
            non_fatal("Expression: Division by zero", "");
            val = 0;
         }
         else
         {
            val = val / temp;
         }
      }
      else if(test("%") != 0)
      {
         temp = evalfuns();
         if(temp == 0)
         {
            non_fatal("Expression: Division by zero", "");
            val = 0;
         }
         else
         {
            val = val % temp;
         }
      }
      else
      {
         break;
      }
   }
   return (val);
}

/************************************************************************/
/*									*/
/*	evalfuns							*/
/*									*/
/*	This level evaluates the pseudo-functions.			*/
/*									*/
/*	sizeof()	This is not allowed by the ANSI Draft C		*/
/*			Standard.  It is here to catch its use		*/
/*			in old programs where it was allowed.		*/
/*									*/
/*	defined		This is a pseudo-function that tests for	*/
/*			the defined state of the next symbol token.	*/
/*									*/
/*	_isstring	Predicate returns 1 if argument is a string	*/
/*			constant.					*/
/*									*/
/*	_strsize	Returns the amount of storage a string constant	*/
/*			will use.					*/
/*									*/
/************************************************************************/

auto evalfuns() -> EVALINT
{
   int   count;
   int   pflag;
   int   rv;
   char *str;
   int   t;

   if(test("sizeof") != 0)
   {
      non_fatal("Expression: sizeof() not allowed", "");

      while(((t = getnstoken(GT_STR)) != ')') && (t != '\n') && (t != EOF))
      {
         ;
      }
      return ((EVALINT)0);
   }
   if(test("defined") != 0)
   {
      if((pflag = static_cast<int>(getnstoken(GT_STR) == '(')) == 0)
      {                /* Latch '(' */
         pbstr(Token); /* Put it back if not */
      }

      if((t = getnstoken(GT_STR)) == LETTER) /* Get an Id */
      {
         rv = (int)(lookup(Token, nullptr) != nullptr);

         if((pflag != 0) && (getnstoken(GT_STR) != ')'))
         {
            non_fatal("Expression: Missing ')'", "");
            pbstr(Token);
         }

         return ((EVALINT)rv);
      }
      if(t == '\n')
         pushback('\n');
      else if(t == EOF)
         end_of_file();

      non_fatal("Expression: Not an identifier: ", Token);
      return ((EVALINT)FALSE);
   }
   if(test("_isstring"))
   {
      rv = FALSE;
      if(!test("("))
         non_fatal("Expression: Missing '('", "");
      else
      {
         if(item(getnstoken, GT_STR) && (Token[0] == '"'))
            rv = TRUE;
         t     = Token[0];
         count = 0; /* Paren nesting level */
         while((!((t == ')') && (count == 0))) && (t != '\n') && (t != EOF))
         {
            if(t == '(')
               count++;
            else if(t == ')')
               count--;
            t = getnstoken(GT_STR);
         }
         if(t != ')')
         {
            non_fatal("Expression: Missing ')'", "");
            if(t == EOF)
               end_of_file();
            pbstr(Token);
         }
      }
      return ((EVALINT)rv);
   }
   else if(test("_strsize"))
   {
      rv = 1;
      if(!test("("))
         non_fatal("Expression: Missing '('", "");
      else
      {
         if(item(getnstoken, GT_STR) && (Token[0] != '"'))
         {
            non_fatal("_strsize: Missing string", "");
            t     = Token[0];
            count = 0; /* Paren nesting level */
            while((!((t == ')') && (count == 0))) && (t != '\n') && (t != EOF))
            {
               if(t == '(')
                  count++;
               else if(t == ')')
                  count--;
               t = getnstoken(GT_STR);
            }
            if(t != ')')
            {
               non_fatal("Expression: Missing ')'", "");
               if(t == EOF)
                  end_of_file();
               pbstr(Token);
            }
         }
         else
         {
            for(;;)
            {
               str = &Token[1];
               for(; (*str != '\0') && (*str != '\"'); rv++)
               {
                  /************************************************************************/
                  if(*str == '\\')
                  {
                     if(*++str == 'x')
                     { /* Hex seq. >= 2 hex digits */
                        count = 0;
                        while(isxdigit(*++str) && (++count < 3))
                           ;
                     }
                     else
                     {
                        if(isdigit(*str))
                        { /* Octal seq. >=3 octal dig */
                           count = 0;
                           while(isdigit(*++str) && (++count < 3))
                              ;
                        }
                        else
                           str++;
                     }
                  }
                  else
                     str++;
                  /************************************************************************/
               }
               if((t = getnstoken(GT_STR)) == ')')
                  break; /* Done */
               if(t == '"')
                  /* Concatenated string */
                  continue;
               non_fatal("_strsize: Not a string", "");
               count = 0; /* Paren nesting lvl */
               while(((t != ')') || (count != 0)) && (t != '\n') && (t != EOF))
               {
                  if(t == '(')
                     count++;
                  else if(t == ')')
                     count--;
                  t = getnstoken(GT_STR);
               }
               if(t != ')')
               {
                  non_fatal("Expression: Missing ')'", "");
                  if(t == '\n')
                     pushback('\n');
                  else if(t == EOF)
                     end_of_file();
               }
               break;
            }
         }
      }
      return ((EVALINT)rv);
   }
   else
      return (evalucom());
}

/************************************************************************/
/*									*/
/*	unary ~ operator						*/
/*									*/
/************************************************************************/

auto evalucom() -> EVALINT
{
   if(test("~") != 0)
   {
      /*lint -e502 lint doesn't like complementing signed integral types */
      return (~evalfuns());
      /*lint +e502 lint doesn't like complementing signed integral types */
   }
   return (evalunot());
}

/************************************************************************/
/*									*/
/*	unary ! operator						*/
/*									*/
/************************************************************************/

auto evalunot() -> EVALINT
{
   if(test("!") != 0)
   {
      return static_cast<long>((EVALINT)evalfuns() == 0);
   }
   return (evalumin());
}

/************************************************************************/
/*									*/
/*	unary + and - operators						*/
/*									*/
/************************************************************************/

auto evalumin() -> EVALINT
{
   if(test("+") != 0)
   {
      return ((EVALINT)evalfuns());
   }
   if(test("-") != 0)
   {
      return ((EVALINT)-evalfuns());
   }
   return (evalval());
}

/************************************************************************/
/*									*/
/*	Must be a value (number or 'x') or a '(' for subexpression.	*/
/*									*/
/************************************************************************/

auto evalval() -> EVALINT
{
   int     c;
   char   *p;
   EVALINT val;

   if(test("(") != 0)
   {
      val = evaltern(); /* Get deeper expression */
      if(test(")") == 0)
      {
         non_fatal("Expression: Mismatched \"()\"", "");
      }
   }
   else if(test("'") != 0) /* Quoted character constant ? */
   {
      val = getchn(); /* Get next character char */
      if(val == '\\') /* Special test for escaped chars */
      {
         switch((char)(val = getchn()))
         {
            case 'a':
               val = '\a';
               break; /* Alert char	*/
            case 'b':
               val = '\b';
               break; /* Backspace	*/
            case 'f':
               val = '\f';
               break; /* Form feed	*/
            case 'n':
               val = '\n';
               break; /* Newline	*/
            case 'r':
               val = '\r';
               break; /* Return	*/
            case 't':
               val = '\t';
               break; /* Horiz. tab	*/
            case 'v':
               val = '\v';
               break; /* Vert. tab	*/
            case '\\':
               val = '\\';
               break;
            case '\'':
               val = '\'';
               break;
            case '\"':
               val = '\"';
               break;
            case '?':
               val = '?';
               break;

            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
               /*
                *	Lead in to octal numeric field: get octal number.
                */
               for(val -= '0'; (c = getchn()) != '\'';)
               {
                  if(c == '\n')
                  {
                     non_fatal("Expression: EOL in '' constant", "");
                     break;
                  }
                  if(c == EOF)
                  {
                     end_of_file();
                  }
                  else if(isoct(c) != 0)
                  {
                     val = val * 8 + c - '0'; /* Add in */
                  }
                  else
                  {
                     non_fatal("Expression: Illegal octal digit", "");
                     break;
                  }
               }
               pushback(c);
               break;

            case 'x':
               /*
                *	Beginning of hex numeric field, get number.
                */
               for(val = 0; (c = getchn()) != '\'';)
               {
                  if(c == '\n')
                  {
                     non_fatal("Expression: EOL in '' constant", "");
                     break;
                  }
                  if(c == EOF)
                  {
                     end_of_file();
                  }
                  else if(ishex(c) != 0)
                  {
                     val = val * 16 + hexbin(c);
                  }
                  else
                  {
                     non_fatal("Expression: Illegal hex digit", "");
                     break;
                  }
               }
               pushback(c);
               break;
            default:
               non_fatal("Expression: Illegal character escape", "");
               break;
         }
      }
      if(test("'") == 0)
      {
         non_fatal("Expression: Mismatched apostrophes", "");
      }
   }
   else
   {
      /* Pack a number */
      if(item(getnstoken, GT_STR) != 0)
      {
         if(isdigit(Token[0]) != 0)
         {
            val = 0;
            p   = Token;
            if(*p == '0')
            {
               /* Octal or Hex number */
               if(*++p == 'x' || *p == 'X')
               {
                  /* Hex number */
                  for(++p; ishex(*p) != 0; ++p)
                  {
                     val = val * 16 + hexbin(*p);
                  }
               }
               else
               {
                  /* Octal number */
                  for(; isoct(*p) != 0; ++p)
                  {
                     val = val * 8 + *p - '0';
                  }
               }
            }
            else
            {
               /* Decimal number */
               for(; isdigit(*p) != 0; ++p)
               {
                  val = val * 10 + *p - '0';
               }
            }
            if(*p != '\0')
            {
               non_fatal("Expression: Bad operand: ", Token);
            }
         }
         else
         {
            /* #if of undef'd id OK */
            if(!istype(Token[0] & 0xFF, C_L))
            {
               non_fatal("Expression: Expected operand: ", Token);
            }
            return ((EVALINT)0);
         }
      }
      else
      {
         non_fatal("Expression: Expected operand: ", Token);
         return ((EVALINT)0);
      }
   }
   return (val);
}

/************************************************************************/
/*									*/
/*	hexbin								*/
/*									*/
/*	Convert hexadecimal digit to binary.				*/
/*									*/
/************************************************************************/

auto hexbin(char ch) -> EVALINT
{
   if(isdigit(ch) != 0)
   {
      return ((EVALINT)(ch - '0'));
   }
   return ((EVALINT)((isupper(ch) != 0 ? tolower(ch) : ch) - 'a' + 10));
}

/************************************************************************/
/*									*/
/*	ishex								*/
/*									*/
/*	Returns TRUE if character is a hexadecimal digit.		*/
/*									*/
/************************************************************************/

auto ishex(char ch) -> int
{
   return static_cast<int>((isdigit(ch) != 0) || ((ch >= 'a') && (ch <= 'f')) || ((ch >= 'A') && (ch <= 'F')));
}

/************************************************************************/
/*									*/
/*	isoct								*/
/*									*/
/*	Returns TRUE if characters is an octal digit.			*/
/*									*/
/************************************************************************/

auto isoct(char ch) -> int
{
   return static_cast<int>((ch >= '0') && (ch <= '7'));
}

/************************************************************************/
/*									*/
/*	item								*/
/*									*/
/*	Skip to first non-blank token and fetch the token into the	*/
/*	token buffer <Token>.  Return TRUE if an item was fetched or	*/
/*	FALSE if at eol.						*/
/*									*/
/************************************************************************/

auto item(int(fun)(int), int f) -> int
{
   int t;

   while((t = (*fun)(f)) != '\n')
   {
      if(t == EOF)
      {
         end_of_file(); /* Abort on end of file */
      }
      return (TRUE);
   }
   pushback('\n'); /* Unfetch the newline */

   return (FALSE);
}

/************************************************************************/
/*									*/
/*	look								*/
/*									*/
/*	Look ahead in for the specified token.	Returns TRUE if		*/
/*	token is found, else FALSE.  Always leaves tokens unfetched.	*/
/*									*/
/************************************************************************/

auto look(const char *str) -> int
{
   int  rc;
   char tokenbuf[TOKENSIZE];

   rc = match(tokenbuf, str);
   pbstr(tokenbuf); /* Unfetch the item */
   return (rc);
}

/************************************************************************/
/*									*/
/*	match								*/
/*									*/
/*	Skip leading blanks and fetch tokens into passed <tbuf> until	*/
/*	a complete match with <str> is found or a failure to match	*/
/*	occurs.  If a complete match occurs, return TRUE else return	*/
/*	FALSE.  On return, <tbuf> has the successfully matched part.	*/
/*									*/
/************************************************************************/

auto match(char *tbuf, const char *str) -> int
{
   int (*fun)(int);
   const char *s;
   char       *t;

   fun = getnstoken; /* Initially skip spaces */
   t   = tbuf;       /* Token packing buffer */
   s   = str;        /* String to match */

   while((*s != '\0') && (item(fun, 0) != 0))
   {
      if(strncmp(Token, s, strlen(Token)) == EQUAL)
      {
         /* Build token */
         t = addstr(t, &tbuf[TOKENSIZE], "Expression: Token too long", Token);

         s += strlen(Token); /* Move past matched part */
         if(*s == '\0')
         {
            break; /* Exit if end of string */
         }
      }
      else
      {
         pbstr(Token); /* Unfetch unmatching string */
         break;
      }
      fun = gettoken; /* Don't skip spaces anymore */
   }

   *t = '\0'; /* End of token here */

   return static_cast<int>(*s == '\0'); /* Return TRUE if end of string	*/
}

/************************************************************************/
/*									*/
/*	readexpline							*/
/*									*/
/*	Read and edit a line into buffer with macro expansion.  Like	*/
/*	"readline" except knows about "defined" pseudo-function.	*/
/*									*/
/************************************************************************/

auto readexpline(char *buf, int bufsize) -> char *
{
   static char    rbo[] = "Read buffer overflow";

   char          *bufp;
   int            is_func;
   struct symtab *sy;
   int            t;

   is_func = FALSE;

   for(bufp = buf; (t = gettoken(GT_STR)) != '\n';)
   {
      if(t == EOF)
      {
         end_of_file();
      }
      if(t == LETTER)
      {
         if((is_func == 0) && ((sy = lookup(Token, nullptr)) != nullptr) && (sy->disable != TRUE))
         {
            bufp = docall(sy, bufp, &buf[bufsize - 1]);
         }
         else
         {
            bufp = addstr(bufp, &buf[bufsize - 1], rbo, Token);
            if(is_func != 0)
            {
               is_func = FALSE;
            }
            else if(strcmp(Token, "defined") == EQUAL)
            {
               is_func = TRUE;
            }
         }
      }
      else
      {
         bufp = addstr(bufp, &buf[bufsize - 1], rbo, Token);
      }
   }

   pushback('\n');
   *bufp = '\0';

   for(bufp = buf; istype(*bufp & 0xFF, C_W); ++bufp)
   {
      ; /* Skip leading blanks */
   }

   return (bufp);
}

/************************************************************************/
/*									*/
/*	test								*/
/*									*/
/*	Skip to next non-blank token and test for match	against		*/
/*	specified token.  If match, return TRUE and advance past	*/
/*	the token.  Otherwise return FALSE, leaving pointer pointing	*/
/*	to first non-blank character in token.				*/
/*									*/
/************************************************************************/

auto test(const char *str) -> int
{
   char tokenbuf[TOKENSIZE];

   if(match(tokenbuf, str) != 0)
   {
      return (TRUE);
   }
   pbstr(tokenbuf); /* Lets look at it again later */
   return (FALSE);
}
