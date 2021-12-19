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
        select.cpp persist.cpp bytestring.cpp config.cpp connectionlog.cpp memory.cpp account.h comm.h dbfind.h extra.h interpreter.h namelist.h spells.h ttydef.h
        act.h common.h db.h fight.h justice.h postoffice.h str_parse.h unitfind.h
        affect.h competition.h dijkstra.h files.h limits.h protocol.h structs.h unixshit.h
        ban.h composed.h dilexp.h guilddef.h magic.h queue.h sysport.h utility.h
        base.h config.h dil.h guild.h main.h select.h system.h utils.h
        blkfile.h connectionlog.h dilinst.h handler.h modify.h skills.h textutil.h values.h
        bytestring.h constants.h dilrun.h hashstring.h money.h spec_assign.h translate.h weather.h
        combat.h db_file.h essential.h help.new.h movement.h spelldef.h trie.h wmacros.h act_movement.h zon_basis.h nice.h signals.h reception.h destruct.h pcsave.h nanny.h mobact.h unit_data.cpp unit_data.h file_index_type.cpp file_index_type.h bin_search_type.cpp bin_search_type.h zone_reset_cmd.cpp zone_reset_cmd.h zone_type.cpp zone_type.h time_info_data.cpp time_info_data.h snoop_data.cpp snoop_data.h descriptor_data.cpp descriptor_data.h room_direction_data.cpp room_direction_data.h room_data.cpp room_data.h obj_data.cpp obj_data.h pc_time_data.cpp pc_time_data.h pc_account_data.cpp pc_account_data.h pc_data.cpp pc_data.h npc_data.cpp npc_data.h char_point_data.cpp char_point_data.h char_follow_type.cpp char_follow_type.h char_data.cpp char_data.h unit_affected_type.cpp unit_affected_type.h unit_fptr.cpp unit_fptr.h cHook.cpp cHook.h cMultiHook.cpp cMultiHook.h)

add_compile_definitions(DMSERVER MEMORY_DEBUG MUD_DEBUG)
add_executable(dmserver ${DMSERVER_SRC})
target_link_libraries(dmserver crypt)
