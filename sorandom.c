#include <linux/module.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/crypto.h>
#include <crypto/rng.h>

#define DEVICE_NAME "sorandom"
#define RAND_MAX ((1U << 31) - 1)
#define CIPHER_ALGO "drbg_nopr_hmac_sha512"

static struct crypto_rng *rng = NULL;
 
static int new_random_number(int* res) {
  int ret;
  ret = crypto_rng_get_bytes(rng,(u8*)res,sizeof(int));
  return ret;
}

static int sorandom_open(struct inode *inode, struct file *filp) {
  u8 seed[32];
  int ret;
  
  rng = crypto_alloc_rng(CIPHER_ALGO, 0, 0);
  if (IS_ERR(rng)) {
    pr_alert("sorandom: Failed to allocated cipher handle\n");
    return PTR_ERR(rng);
  }

  get_random_bytes(seed, sizeof(seed));
  ret = crypto_rng_reset(rng,seed,sizeof(seed));
  if (ret) {
    pr_info("sorandom: Failed to seed RNF: %d\n", ret);
    return ret;
  }
  //pr_info("sorandom: open for randomness\n");
  return 0;
}

static int sorandom_close(struct inode *inode, struct file *filp) {
  crypto_free_rng(rng);
  //pr_info("sorandom: closing\n");
  return 0;
}

static ssize_t sorandom_read(struct file *filp, char __user *buf, size_t count, loff_t *pos) {
  if (count < sizeof(int)) {
    return -EFAULT;
  }

  if (*pos > 0) {
    return 0;
  }

  int randnum = 0;
  if (new_random_number(&randnum) != 0) {
    pr_info("sorandom: Error generating new random number\n");
    return -EIO;
  }
  
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




