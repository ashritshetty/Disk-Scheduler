#!/bin/sh
set -e

echo "Before compile, make sure that you have change the syscall_64.tbl"
echo "-------------"
echo "331 common  init_blk_statistic  sys_init_blk_statistic"
echo "332 common  report_statistic    sys_report_statistic"
echo "-------------"
echo "Input y to continue"

read ok
if [ $ok != "y" ]
then
exit 1
fi

cp -f scsi/blk.h block/
cp -f scsi/blkdev.h include/linux/
cp -f scsi/elevator.c block/
cp -f scsi/blk-core.c block/

make bzImage
make modules
make modules_install
make install
