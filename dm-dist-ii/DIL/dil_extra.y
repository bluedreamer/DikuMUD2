forlist  : block
         { $$ = $1; }
         | dilinst
         { $$ = $1; }
         | dilinst ',' forlist
         { $$.fst = $1.fst; $$.lst = $3.lst; }
         | block ',' forlist
         { $$.fst = $1.fst; $$.lst = $3.lst; }
         

         | DILSI_FOR '(' forlist ';'
             pushcnt ihold coreexp ahold ';'
             pushbrk defcnt ihold ahold forlist ihold ahold ')'
             dilcomposed ihold ahold
             defbrk popbrk popcnt
         {
            /*
             * allows both break and continue inside the composed
             * and induction statment list.
             * continue starts at the induction statement.
             * break starts with the instruction after loop.
             * The order of execution is reordered using goto.
             *
             * <forlist1>
             * <if> <exp> <break-adr>
             * <goto> <composed-adr>
             * <forlist2> <goto> <if-adr>
             * <composed> <goto> <forlist2-adr>
             */
				 
            wtmp = &tmpl.core[$6];
            bwrite_uint8_t(&wtmp,DILI_IF);
            wtmp = &tmpl.core[$8];
            bwrite_uint32_t(&wtmp,wcore-tmpl.core);
            wtmp = &tmpl.core[$12];
            bwrite_uint8_t(&wtmp,DILI_GOTO);
            bwrite_uint32_t(&wtmp,$18.fst);
            wtmp = &tmpl.core[$15];
            bwrite_uint8_t(&wtmp,DILI_GOTO);
            bwrite_uint32_t(&wtmp,$6);
            wtmp = &tmpl.core[$19];
            bwrite_uint8_t(&wtmp,DILI_GOTO);
            bwrite_uint32_t(&wtmp,$14.fst);				
         }
         | DILSI_FOE SYMBOL ihold ihold ihold ihold '(' coreexp ')'
             pushbrk pushcnt ihold ihold ihold ihold ahold
             dilcomposed
             defcnt ihold ihold ihold ihold ihold ihold ihold
             ihold ahold
             defbrk popbrk popcnt
         {
            /*
             * allows both break and continue inside the composed
             * statment. continue starts at the induction statement.
             * break starts with the instruction after loop.
             * The order of execution is ordered effectively.
             *
             * <ass> <var> <var#> <coreexp>
             * <if> <var> <var#> <break-adr>
             * <composed>
             * <ass> <var> <var#> <fld> <var> <var#> <.next>
             * <goto> <if-adr>
             */
            int i;

            if ((i = search_block($2, var_names, TRUE)) == -1)
               fatal("Unknown variable");
            else if (tmpl.vart[i]!=DILV_UP) 
               fatal("Foreach variable must be unitptr");
            else if ($8.typ != DILV_UP)
               fatal("Foreach expression must be unitptr");
            else {
               /* assign variable to expression initially */
               wtmp = &tmpl.core[$3];
               bwrite_uint8_t(&wtmp,DILI_ASS);
               bwrite_uint8_t(&wtmp,DILE_VAR);
               bwrite_uint16_t(&wtmp,i);
               wtmp = &tmpl.core[$12];
               bwrite_uint8_t(&wtmp,DILI_IF);
               bwrite_uint8_t(&wtmp,DILE_VAR);
               bwrite_uint16_t(&wtmp,i);
               bwrite_uint32_t(&wtmp,wcore-tmpl.core);
               wtmp = &tmpl.core[$19];
               bwrite_uint8_t(&wtmp,DILI_ASS);
               bwrite_uint8_t(&wtmp,DILE_VAR);
               bwrite_uint16_t(&wtmp,i);
               bwrite_uint8_t(&wtmp,DILE_FLD);
               bwrite_uint8_t(&wtmp,DILE_VAR);
               bwrite_uint16_t(&wtmp,i);
               bwrite_uint8_t(&wtmp,DILF_NXT);
               bwrite_uint8_t(&wtmp,DILI_GOTO);
               bwrite_uint32_t(&wtmp,$12);					
            }
         }
