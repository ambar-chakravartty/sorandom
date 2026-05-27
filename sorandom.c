#include <linux/module.h>
#include <linux/fs.h>
#include <linux/sched.h>

#define DEVICE_NAME "sorandom"
#define RAND_MAX ((1U << 31) - 1)

static int seed_val = 0;
static int new_random_number(void) {
  return seed_val = (seed_val * 1103515245 + 12345) & RAND_MAX;;
}

static int sorandom_open(struct inode *inode, struct file *filp) {
  pr_info("sorandom: open for randomness\n");
  return 0;
}

static int sorandom_close(struct inode *inode, struct file *filp) {
  pr_info("sorandom: closing\n");
  return 0;
}

static ssize_t sorandom_read(struct file *filp, char __user *buf, size_t count, loff_t *pos) {
  if (count < sizeof(int)) {
    return -EFAULT;
  }

  if (*pos > 0) {
    return 0;
  }

  int randnum = new_random_number();
  if (copy_to_user(buf, &randnum, sizeof(int)) != 0) {
    return -EFAULT;
  }

  *pos += sizeof(int);
  return sizeof(int);
}

static unsigned int major;
static struct file_operations sorandom_ops = {
    .open = sorandom_open,
    .release = sorandom_close,
    .read = sorandom_read,
};

static int __init sorandom_init(void) {
  major = register_chrdev(0, DEVICE_NAME, &sorandom_ops);

  if (major < 0) {
    pr_alert("Failed to register device. Error: %d\n", major);
    return major;
  }

  pr_info("sorandom: Intialised (Major: %d) \n",major);
  return 0;
}

static void __exit sorandom_exit(void) {
  pr_info("sorandom: Exiting\n");
  unregister_chrdev(major,DEVICE_NAME);
}

module_init(sorandom_init);
module_exit(sorandom_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ambar Chakravartty");




