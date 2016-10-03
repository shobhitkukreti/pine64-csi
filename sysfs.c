/*
 *SYSFS Entry for CSI Driver 
 */

#include<linux/fs.h>
#include<linux/kobject.h>
#include<linux/slab.h>
#include "csi.h"

extern struct csi_cci_platform_device pine64_drv;

struct csi_attr {
	struct attribute attr;
};

static struct csi_attr csi_attr_struct = {
	.attr.name="csi_cci",
	.attr.mode=0644,

};

static struct attribute *attr_csi[] = {
	&csi_attr_struct.attr,
	NULL
};



static ssize_t csi_show (struct kobject *kobj, struct attribute *attr, char *buf)
{
//	struct csi_attr *a = container_of(attr, struct csi_attr, attr);
	return scnprintf(buf, PAGE_SIZE, "CSI-READ\n");

}

static ssize_t csi_store(struct kobject *kobj, struct attribute *attr, const char *buf, size_t len)
{
	uint32_t addr, val;
	//struct csi_attr *a = container_of(attr, struct csi_attr, attr);
	sscanf(buf, "%x %x", addr, val);

	return len;
}


static struct sysfs_ops csi_ops = {
	.show = csi_show,
	.store = csi_store,
};


static struct kobj_type csi_direc = {
	.sysfs_ops = NULL,
	.default_attrs = NULL,
};

static struct kobj_type csi_file = {
	.sysfs_ops = &csi_ops,
	.default_attrs = attr_csi,
};

struct kobject *dkobj, *fkobj;

int sysfs_init( void ) 
{
	dkobj = kzalloc (sizeof(*dkobj), GFP_KERNEL);
	if(dkobj) {
		kobject_init(dkobj, &csi_direc);
		if(kobject_add(dkobj, NULL, "%s", "csi")) {
			printk(KERN_ERR "SYSFS CSI Direc Failed\n");
			kobject_put(dkobj);
			kfree(dkobj);
			dkobj = NULL;
			return -1;
		}
	}

	fkobj = kzalloc(sizeof(*fkobj), GFP_KERNEL);
	if(fkobj) {
		kobject_init(fkobj, &csi_file);
		if(kobject_add(fkobj, dkobj, "%s", "csi_driver")) {
			printk("SYSFS CSI File Failed\n");
			kobject_put(fkobj);
			kfree(fkobj);
			fkobj = NULL;
			kobject_put(dkobj);
			kfree(dkobj);
			dkobj = NULL;
			return -1;	
		}

	}


	return 0;
}


void sysfs_cleanup(void)
{
	if(fkobj) {
		kobject_put(fkobj);
		kfree(fkobj);
	}

	if(dkobj) {
		kobject_put(dkobj);
		kfree(dkobj);
	}	

}
