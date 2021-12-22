#pragma once

char                      *in_string(std::shared_ptr<unit_data> ch, std::shared_ptr<unit_data> u);
extern int                 lose_exp(std::shared_ptr<unit_data>);
extern void                tax_player(std::shared_ptr<unit_data> ch);
int                        is_slimed(std::shared_ptr<file_index_type> sp);
int                        lose_exp(std::shared_ptr<unit_data> ch);
int                        patch(char *ref, ubit32 reflen, char *dif, int diflen, char *res, int reslen, ubit32 crc);
int                        pay_point_charlie(std::shared_ptr<unit_data> ch, std::shared_ptr<unit_data> to);
int                        read_player_id(void);
std::shared_ptr<unit_data> make_corpse(std::shared_ptr<unit_data> ch);
struct time_info_data      age(std::shared_ptr<unit_data> ch);
struct time_info_data      real_time_passed(time_t t2, time_t t1);
ubit8                      player_has_mail(std::shared_ptr<unit_data> ch);
void                       die_follower(std::shared_ptr<unit_data> ch);
void                       do_return(std::shared_ptr<unit_data> ch, char *arg, struct command_info *cmd);
void                       gain_exp(std::shared_ptr<unit_data> ch, int gain);
void                       nanny_close(struct descriptor_data *d, char *arg);
void                       nanny_menu(struct descriptor_data *d, char *arg);
void                       register_destruct(int i, void *ptr);
void                       stat_bank(const std::shared_ptr<unit_data> ch, std::shared_ptr<unit_data> u);
void                       stop_all_special(std::shared_ptr<unit_data> u);
void                       switchbody(std::shared_ptr<unit_data> ch, std::shared_ptr<unit_data> victim);
void                       unlink_affect(std::shared_ptr<unit_data> u, struct unit_affected_type *af);
void                       unsnoop(std::shared_ptr<unit_data> ch, int mode);
void                       unswitchbody(std::shared_ptr<unit_data> npc);
