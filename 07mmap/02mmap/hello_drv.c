/*************************************************************************
 > File Name: hello.drv.c
 > Author: Winter
 > Created Time: Sun 07 Jul 2024 12:35:19 AM EDT
 ************************************************************************/

#include <linux/module.h>

#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/kmod.h>
#include <linux/gfp.h>
#include <asm/pgtable.h>
#include <linux/mm.h>
#include <linux/slab.h>





// 1确定主设备号，也可以让内核分配
static int major = 0;				// 让内核分配
static char* kernel_buf;			// 保存应用程序的数据，kmalloc分配的数据
static struct class *hello_class;
static int bufSize = 1024 * 8;

#define MIN(a, b) (a < b ? a : b)

// 3 实现对应的 drv_open/drv_read/drv_write 等函数，填入 file_operations 结构体
static ssize_t hello_drv_read (struct file *file, char __user *buf, size_t size, loff_t *offset)
{
	int err;
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	// 将kernel_buf区的数据拷贝到用户区数据buf中，即从内核kernel_buf中读数据
	err = copy_to_user(buf, kernel_buf, MIN(bufSize, size));
	return MIN(bufSize, size);
}

// mmap函数
int hello_drv_mmap (struct file* file, struct vm_area_struct* vma)
{
	// 1获得物理地址-虚拟地址转物理地址
	unsigned long phy = virt_to_phys(kernel_buf);

	// 2设置属性cache/buff
	// 不使用cache，使用buffer
	vma->vm_page_prot = pgprot_writecombine(vma->vm_page_prot);

	// 3映射mmap
	if (remap_pfn_range(vma, vma->vm_start, phy >> PAGE_SHIFT, 
		vma->vm_end - vma->vm_start, vma->vm_page_prot))
	{
		printk("mmap remap_pfn_range failed\n");
		return -ENOBUFS;
	}
	return 0;
}


static ssize_t hello_drv_write (struct file *file, const char __user *buf, size_t size, loff_t *offset)
{
	int err;
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	// 把用户区的数据buf拷贝到内核区kernel_buf，即向写到内核kernel_buf中写数据
	err = copy_from_user(kernel_buf, buf, MIN(bufSize, size));
	return MIN(1024, size);
}

static int hello_drv_open (struct inode *node, struct file *file)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	return 0;
}

static int hello_drv_close (struct inode *node, struct file *file)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	return 0;
}


// 2定义自己的 file_operations 结构体
static struct file_operations hello_drv = {
	.owner = THIS_MODULE,
	.open = hello_drv_open,
	.read = hello_drv_read,
	.write = hello_drv_write,
	.mmap = hello_drv_mmap,
	.release = hello_drv_close,
};


// 4把 file_operations 结构体告诉内核： register_chrdev
// 5谁来注册驱动程序啊？得有一个入口函数：安装驱动程序时，就会去调用这个入口函数
static int __init hello_init(void)
{
	int err;
	// 分配内存
	kernel_buf = kmalloc(bufSize, GFP_KERNEL);
	
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	// 注册hello_drv，返回主设备号
	major = register_chrdev(0, "hello", &hello_drv);  /* /dev/hello */
	// 创建class
	hello_class = class_create(THIS_MODULE, "hello_class");
	err = PTR_ERR(hello_class);
	if (IS_ERR(hello_class)) {
		printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
		unregister_chrdev(major, "hello");
		return -1;
	}
	// 创建device
	device_create(hello_class, NULL, MKDEV(major, 0), NULL, "hello"); /* /dev/hello */
	
	return 0;
}



// 6有入口函数就应该有出口函数：卸载驱动程序时，出口函数调用unregister_chrdev
static void __exit hello_exit(void)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	device_destroy(hello_class, MKDEV(major, 0));
	class_destroy(hello_class);
	// 卸载
	unregister_chrdev(major, "hello");
	// 释放内存
	kfree(kernel_buf);
}


// 7其他完善：提供设备信息，自动创建设备节点： class_create,device_create
module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");

