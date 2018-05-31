#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/gps.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/mman.h>
#include <uapi/asm-generic/errno-base.h>
#include <linux/fs.h>
#include <linux/namei.h>
#include <linux/string.h>
#include <linux/path.h>

#define ten_6	(0.00001)

struct gps_location main_device_gps;

asmlinkage int set_gps_location(struct gps_location __user *loc)
{
	struct gps_location temploc;
	
	if(copy_from_user(&temploc, loc, sizeof(struct gps_location)) < 0)
		return -EFAULT;
	
	double latitude = temploc->lat_integer + loc->lat_fractional * ten_6;
	double longtitude = temploc->lng_integer + loc->lng_fractional * ten_6;

	//check nonzero and below 999999 for lat_fractional and lng_fractional
	if(temploc.lat_fractional < 0 || temploc.lat_fractional > 999999 || temploc.lng_fractional < 0 || temploc.lng_fractional > 999999)
		return -EFAULT;

	//check latitude range
        if(temploc.lat_integer < -90 || temploc.lat_integer > 90)
                return -EFAULT;
        //check longitudinal range
        if(temploc.lng_integer < -180 || temploc.lng_integer > 180)
                return -EFAULT;

	//check accuracy range
	if(temploc.accuracy < 0)
		return -EFAULT;
	int flags;
	
	spin_lock_irqsave(&(lock), flags);	
	memcpy(&main_device_gps, &temploc, sizeof(struct gps_location))
	spin_unlock_irqrestore(&(lock), flags);
	return 0;
}

asmlinkage int get_gps_location(const char __user *pathname, struct gps_location __user *loc)
{
	struct gps_location temploc;
	// get file location information
	struct path path;

	if(loc == NULL)
		return -EFAULT;

	// check if file is readable by current user
	struct inode *inode;
	struct path path;
	int len;

	// not sure the error checking over here
	if((len = strnlen_user(pathname)) == 0)
		return -EINVAL;

	char pathname_from_user = (char *) kmalloc(sizeof(char) * len, GFP_ATOMIC);
	
	if(pathname_from_user == NULL)
		return -ENOMEM;
	//copy from user	
	if(copy_from_user(pathname_from_user, pathname, len) < 0)
	{
		kfree(name);
		return -EFAULT;
	}
	
	if(kern_path(pathname, LOOKUP_FOLLOW, &path))
	{
		kfree(name);
		return -EINVAL;
	}

	kfree(name);
	// By using the path get the inode entry
	inode = path.dentry->d_inode;
	printk("DEBUG: Path name: %s, inode: %lu\n", pathname, inode->i_ino);
	
	// Check inode permission whether the file is readable or not
	if(inode_permission(inode, MAY_READ))
		return -EACCES;
	// if no GPS coordinates are embedded in the file return error.
	if(inode->i_op->get_gps_location)
		inode->i_op->get_gps_location(inode, &temploc);
	else
		return -ENODEV;
	
	//copy to user
	if(copy_to_user(loc, &temploc, sizeof(struct gps_location) < 0)
		return -EFAULT;

	return 0;
}

// we need function to check whether it two files are the same location for file access

bool check_accuracy_of_location(struct gps_location *loc1, struct gps_location *loc2)
{
  //TODO
}
