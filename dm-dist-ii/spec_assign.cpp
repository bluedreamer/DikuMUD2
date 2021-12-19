#include "spec_assign.h"

#include "handler.h"
#include "interpreter.h"
#include "utils.h"

auto spec_unused(struct spec_arg *sarg) -> int
{
   if(sarg->cmd->no != CMD_AUTO_EXTRACT)
   {
      slog(LOG_ALL, 0, "Unit %s@%s had undefined special routine.", UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
      destroy_fptr(sarg->owner, sarg->fptr);
   }

   return SFR_SHARE;
}
