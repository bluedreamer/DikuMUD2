set(DMSERVER_SRC account.cpp act_change.cpp act_comm.cpp act_info.cpp act_movement.cpp
  act_obj1.cpp act_obj2.cpp act_obj3.cpp act_offensive.cpp act_other.cpp act_skills.cpp
  act_social.cpp act_wizard.cpp act_wstat.cpp affect.cpp apf_affect.cpp ban.cpp
  bank.cpp blkfile.cpp board.cpp comm.cpp common.cpp constants.cpp convert.cpp create.cpp db.cpp
  db_file.cpp db_utils.cpp demigod.cpp destruct.cpp dictionary.cpp dijkstra.cpp main.cpp
  eliza.cpp experience.cpp fight.cpp files.cpp guild.cpp handler.cpp help.cpp act_look.cpp
  hometown.cpp interpreter.cpp justice.cpp limits.cpp magic.cpp magic1.cpp magic2.cpp
  mobact.cpp modify.cpp money.cpp nanny.cpp nice.cpp pain.cpp pcsave.cpp
  postoffice.cpp protocol.cpp queue.cpp reception.cpp shop.cpp signals.cpp skills.cpp
  slime.cpp spec_assign.cpp spec_procs.cpp spell_parser.cpp str_parse.cpp string.cpp
  swap.cpp sysport.cpp system.cpp teach.cpp textutil.cpp tif_affect.cpp trie.cpp utility.cpp
  weather.cpp zon_basis.cpp zon_midgaard.cpp zon_wiz.cpp
  zone_boot.cpp zone_reset.cpp namelist.cpp structs.cpp dilfld.cpp extra.cpp
  dilinst.cpp dilexp.cpp dilrun.cpp dbfind.cpp competition.cpp unitfind.cpp combat.cpp
  select.cpp persist.cpp bytestring.cpp config.cpp connectionlog.cpp memory.cpp)


set(DSERVER_H account.h affect.h ban.h blkfile.h comm.h common.h db.h db_file.h
  dil.h files.h handler.h interpreter.h justice.h limits.h magic.h money.h
  movement.h postoffice.h skills.h spells.h structs.h sysport.h
  system.h textutil.h trie.h utils.h values.h)

add_compile_definitions(DMSERVER MEMORY_DEBUG MUD_DEBUG)
add_executable(dmserver ${DMSERVER_SRC} ${DMSERVER_H})
target_link_libraries(dmserver crypt)
