#ifndef __LINUX_GPS_H
#define __LINUX_GPS_H

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/elf.h>
#include <linux/smp.h>
#include <linux/ptrace.h>
#include <linux/user.h>
#include <linux/security.h>
#include <linux/init.h>
#include <linux/signal.h>
#include <linux/uaccess.h>
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#include <linux/regset.h>
#include <linux/audit.h>
#include <linux/tracehook.h>
#include <linux/unistd.h>

#include <linux/spinlock_types.h>
#include <linux/spinlock.h>
#include <linux/lockdep.h>
static DEFINE_SPINLOCK(lock);

struct gps_location {
	int lat_integer;
	int lat_fractional;
	int lng_integer;
	int lng_fractional;
	int accuracy;
};

asmlinkage int set_gps_location(struct gps_location __user *loc);

asmlinkage int get_gps_location(const char __user *pathname, struct gps_location __user *loc);
#endif
