#ifndef FILESYSTEM_H
#define FILESYSTEM_H

void fs_info();
void partition_list();
void partition_select(int id);

extern char selected_partition[64];
void slack_scan();
void stealth_hide(char *secret, char *target);
void stealth_extract(char *target);
void slack_carve();
void slack_scan();
#endif
