/*
 * Copyright (C) 2008-2013 Helmut Grohne <helmut@subdivi.de> - All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License v.2 or v.3.
 */

#ifndef PLUGINS_H
#define PLUGINS_H

int cpu(int argc, char **argv);
int df(int argc, char **argv);
int entropy(int argc, char **argv);
int external_(int argc, char **argv);
int forks(int argc, char **argv);
int fw_packets(int argc, char **argv);
int if_(int argc, char **argv);
int if_err_(int argc, char **argv);
int interrupts(int argc, char **argv);
int iostat(int argc, char **argv);
int load(int argc, char **argv);
int memory(int argc, char **argv);
int open_files(int argc, char **argv);
int open_inodes(int argc, char **argv);
int processes(int argc, char **argv);
int swap(int argc, char **argv);
int threads(int argc, char **argv);
int uptime(int argc, char **argv);

#endif
