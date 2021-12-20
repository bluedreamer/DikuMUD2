#pragma once
class help_index_type
{
public:
   help_index_type();
   ~help_index_type();

   char *keyword;
   long  pos;
};

class help_file_type
{
public:
   help_file_type();
   ~help_file_type();

   void generate_help_idx(const char *filename);

   help_index_type *help_idx;       /* the help keyword list */
   int              elements, size; /* info about the list   */
   char            *filename;
};
