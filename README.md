# Proj4-system-call-implementation
---
Q1- Make set_location_gps
---
Q2- Adding ext2- inode function for setter and getter operations
---
Add the following function pointers to the inode struct at include/linux/fs.h
int (*set_gps_location)(struct inode *);
int (*get_gps_location)(struct inode *, struct gps_location *);

Add the following on the ext2_inode struct at fs/ext2/ext2.h. This is the struct of an inode on the disk
  __le32 i_lat_integer;
  __le32 i_lat_fractional;
  __le32 i_lng_integer;
  __le32 i_lng_fractional;
  __le32 i_accuracy;
  
On the fs/ext2/file.c .Similar to the proj3, register the function setter and getter ext2 inode operation.

.set_gps_location = ext2_set_gps_location,
.get_gps_location = ext2_get_gps_location,

On the fs/ext2/inode.c , the struct inode *ext2_iget gets gps location information from the inode on the disk to the inode on memory.
  ei->inode_gps.lat_integer = le32_to_cpu(raw_inode->i_lat_integer);
  ei->inode_gps.lat_fractional = le32_to_cpu(raw_inode->i_lat_fractional);
  ei->inode_gps.lng_integer = le32_to_cpu(raw_inode->i_lng_integer);
  ei->inode_gps.lng_fractional = le32_to_cpu(raw_inode->i_lng_fractional);
  ei->inode_gps.accuracy = le32_to_cu(raw_inode->i_accuracy);
   
The struct inode __ext2_write_inode writes the gps location information from the inode on the memory to the inode on the disk.
 raw->inode->i_lat_integer = cpu_to_le32(ei->inode_gps.lat_integer);
 raw->inode->i_lat_fractional = cpu_to_le32(ei->inode_gps.lat_fractional);
 raw->inode->i_lng_integer = cpu_to_le32(ei->inode_gps.lng_integer);
 raw->inode->i_lng_fractional = cpu_to_le32(ei->inode_gps.lng_fractional);
 raw->inode->i_accuracy = cpu_to_le32(ei->inode_gps.accuracy);
 
also add the following implementation of ext2_set_gps_location(struct inode *inode) and ext2_get_gps_location(struct inode *inode, struct gps_location *gps)

ext2_set_gps_location set the gps location from the systemcall to the inode on the memory.
ext2_get_gps_location gets the gps location from the inode on the memory to the gps.

These are the following implementation.

int ext2_set_gps_location(struct inode *inode)
{
   struct ext2_inode_info *ei = EXT2_I(inode);
   int flags;
   
   spin_lock_irqsave(&(lock), flags);
   ei->inode_gps = main_device_gps // from systemcall of set_gps_location.
   spin_unlock_irqrestore(&(lock), flags);
   return 0;
}

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
Q3- Make set_gps_location as default when ext2 is created and when file is modified.
---
Q4 -Make get_location_gps
---
