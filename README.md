# Proj4-system-call-implementation
---
Q1- Make set_location_gps
---
Q2- Adding ext2- inode function for setter and getter operations
---
Add the following function pointers to the inode struct at include/linux/fs.h
int (*set_gps_location)(struct inode *);
int (*get_gps_location)(struct inode *, struct gps_location *);

Add the following on the ext2_inode struct at fs/ext2/ext2.h. This is the structure of an inode on the disk
  __le32 i_lat_integer;
  __le32 i_lat_fractional;
  __le32 i_lng_integer;
  __le32 i_lng_fractional;
  __le32 i_accuracy;
  
Q3- Make set_gps_location as default when ext2 is created and when file is modified.
---
Q4 -Make get_location_gps
---
