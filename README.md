# Proj4-system-call-implementation
---
Q1- Make set_location_gps
---
This is easiest part of all, just get the gps_location from the user to kernel, then check error and the conditions for the ranges of the gps location value. then memcpy the gps_location struct to a global struct of gps_location. The global struct of the gps_location is needed so that we can set the gps_location to the inode of ext2 file.

Q2- Adding ext2- inode function for setter and getter operations
---
Add the following function pointers to the inode struct at include/linux/fs.h.
```=clike
int (*set_gps_location)(struct inode *);
int (*get_gps_location)(struct inode *, struct gps_location *);
```
Add the following on the ext2_inode struct at fs/ext2/ext2.h. This is the struct of an inode on the disk
```=clike
  __le32 i_lat_integer;
  __le32 i_lat_fractional;
  __le32 i_lng_integer;
  __le32 i_lng_fractional;
  __le32 i_accuracy;
```
```=clike
  extern int ext2_set_gps_location(struct inode *);
  extern int ext2_get_gps_location(struct inode *, struct gps_location *);
```
I also defined a spinlock on the same file fs/ext2/ext2.h
```=clike
#include <linux/spinlock_types.h>
#include <linux/spinlock.h>
#include <linux/lockdep.h>
static DEFINE_SPINLOCK(lock);
```
On the fs/ext2/file.c .Similar to the proj3, register the function setter and getter ext2 inode operation.
```=clike
.set_gps_location = ext2_set_gps_location,
.get_gps_location = ext2_get_gps_location,
```
On the fs/ext2/inode.c , the struct inode *ext2_iget gets gps location information from the inode on the disk to the inode on memory.
  ```=clike
  ei->inode_gps.lat_integer = le32_to_cpu(raw_inode->i_lat_integer);
  ei->inode_gps.lat_fractional = le32_to_cpu(raw_inode->i_lat_fractional);
  ei->inode_gps.lng_integer = le32_to_cpu(raw_inode->i_lng_integer);
  ei->inode_gps.lng_fractional = le32_to_cpu(raw_inode->i_lng_fractional);
  ei->inode_gps.accuracy = le32_to_cu(raw_inode->i_accuracy);
   ```
The struct inode __ext2_write_inode writes the gps location information from the inode on the memory to the inode on the disk.
```=clike
 raw->inode->i_lat_integer = cpu_to_le32(ei->inode_gps.lat_integer);
 raw->inode->i_lat_fractional = cpu_to_le32(ei->inode_gps.lat_fractional);
 raw->inode->i_lng_integer = cpu_to_le32(ei->inode_gps.lng_integer);
 raw->inode->i_lng_fractional = cpu_to_le32(ei->inode_gps.lng_fractional);
 raw->inode->i_accuracy = cpu_to_le32(ei->inode_gps.accuracy);
 ```
also add the following implementation of ext2_set_gps_location(struct inode *inode) and ext2_get_gps_location(struct inode *inode, struct gps_location *gps)

ext2_set_gps_location set the gps location from the systemcall to the inode on the memory.
ext2_get_gps_location gets the gps location from the inode on the memory to the gps.

These are the following implementation.
```=clike
int ext2_set_gps_location(struct inode *inode)
{
   struct ext2_inode_info *ei = EXT2_I(inode);
   int flags;
   
   spin_lock_irqsave(&(lock), flags);
   ei->inode_gps = main_device_gps // from systemcall of set_gps_location.
   spin_unlock_irqrestore(&(lock), flags);
   return 0;
}
```
```=clike
int ext2_get_gps_location(struct inode *inode, struct gps_location *gps)
{
   struct ext2_inode_info *ei = EXT2_I(inode);
   int flags;
   if(gps == NULL)
    return 1;
   spin_lock_irqsave(&(lock), flags);
   *gps = ei->inode_gps;
   spin_unlock_irqsave(&(lock), flags);
   return 0;
}
```
Q3- Make set_gps_location as default when ext2 is created and when file is modified.
---
This part was tricky, though the implementation is simple, just have to make set_gps_location as default when ext2 is created and when file is modified.

For the creation of ext2 is located at fs/ext2/namei.c. At static int ext2_create function, add the following right before calling mark_inode_dirty(inode); 

```=clike
if(inode->i_op->set_gps_location)
  inode->i_op->set_gps_location(inode);
  ```
For the file modification, modification means writing something to the file informations, so the modification part is actually is located at fs/read_write.c. On the function ssize_t vps_write  before if(ret > 0), add the following.

```=clike
struct inode *inode;
inode = file->f_path.dentry->d_inode;
if(inode->i_op->set_gps_location)
  inode->i_op->set_gps_location;
  ```
  
Q4 -Make get_location_gps
---
This is also easy since we are given the users pathname and get gps_location from the ext2 to the users. Again we have to get users pathname to the kernel by copy_from_user function. By using the pathname in the kernel get the path struct by kern_path(pathname, LOOKUP_FOLLOW, &path). By using the path get the following inode entry. Then check the inode permission whether the file is readable or not.

Then after checking the permission of the inode check if GPS coordinates are embedded in the file by calling if(inode->i_op->get_gps_location). After the getting the gps_location struct from the get_gps_location function then copy that information to the user gps_location by copy_to_user function.
