#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/timekeeping.h>
#include <asm/uaccess.h>
#include <linux/wait.h>
#include <linux/blkdev.h>

SYSCALL_DEFINE0(init_blk_statistic) {
    init_blk_statistic();
    return (1);
}

SYSCALL_DEFINE1(report_statistic, long, addr) {
    int ret;
    ret = copy_to_user((struct blk_st_t*)addr, (struct blk_st_t*)report_statistic(), sizeof(struct blk_st_t));
    if(ret)
        printk(KERN_ALERT "copy_to_user failed %d\n", ret);
    return (1);
}
