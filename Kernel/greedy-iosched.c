#include <linux/blkdev.h>
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>

sector_t curr_head = 0;

struct greedy_data {
    struct list_head uphill;
    struct list_head downhill;
};

static void greedy_merged_requests(struct request_queue *q, struct request *rq,
                 struct request *next)
{
    list_del_init(&next->queuelist);
}

static int greedy_dispatch(struct request_queue *q, int force)
{
    struct greedy_data *gd = q->elevator->elevator_data;
    struct request *rq_uphill;
    struct request *rq_downhill;
    struct request *rq;

    sector_t up_diff;
    sector_t down_diff;

    if ((list_empty(&gd->uphill)) && (list_empty(&gd->downhill))) {
        return 0;
    }

    if (list_empty(&gd->uphill)) {
        rq = list_entry(gd->downhill.next, struct request, queuelist);
    }

    else if (list_empty(&gd->downhill)) {
        rq = list_entry(gd->uphill.next, struct request, queuelist);
    }

    else {
        rq_uphill = list_entry(gd->uphill.next, struct request, queuelist);
        rq_downhill = list_entry(gd->downhill.next, struct request, queuelist);

        /*
        if (blk_rq_pos(rq_uphill) < curr_head)
            up_diff = curr_head - blk_rq_pos(rq_uphill);
        else
            up_diff = blk_rq_pos(rq_uphill) - curr_head;

        if (blk_rq_pos(rq_downhill) < curr_head)
            down_diff = curr_head - blk_rq_pos(rq_downhill);
        else
            down_diff = blk_rq_pos(rq_downhill) - curr_head;
        */

        up_diff = curr_head - blk_rq_pos(rq_uphill);
        down_diff = blk_rq_pos(rq_downhill) - curr_head;

        if (up_diff < down_diff){
            rq = rq_uphill;
        }
        else {
            rq = rq_downhill;
        }    
    }

    list_del_init(&rq->queuelist);
    elv_dispatch_add_tail(q, rq);
    curr_head = blk_rq_pos(rq) + blk_rq_sectors(rq);
    return 1;

    
}

static void greedy_add_request(struct request_queue *q, struct request *rq)
{
    struct greedy_data *gd = q->elevator->elevator_data;
    struct list_head *pos;
    struct request *ele;
    sector_t req_pos = blk_rq_pos(rq);
    sector_t ele_head;

    if(req_pos < curr_head){
        list_for_each(pos, &gd->downhill){
            ele = list_entry(pos, struct request, queuelist);
            ele_head = blk_rq_pos(ele);
            if(ele_head < req_pos){
                        list_add(&rq->queuelist, pos->prev);
                        return;
            }
        }
            list_add_tail(&rq->queuelist, &gd->downhill);
    }
    else{
        list_for_each(pos, &gd->uphill){
            ele = list_entry(pos, struct request, queuelist);
            ele_head = blk_rq_pos(ele);
            if(ele_head > req_pos){
                        list_add(&rq->queuelist, pos->prev);
                        return;
            }
        }
            list_add_tail(&rq->queuelist, &gd->uphill);
    }
}

static struct request *
greedy_former_request(struct request_queue *q, struct request *rq)
{
    struct greedy_data *gd = q->elevator->elevator_data;

    if(rq->queuelist.prev == &gd->uphill) {
        if(gd->downhill.next == &gd->downhill) {
            return NULL;
        }
        return list_entry(gd->downhill.next, struct request, queuelist);
    }
    return list_entry(rq->queuelist.prev, struct request, queuelist);
}

static struct request *
greedy_latter_request(struct request_queue *q, struct request *rq)
{
    struct greedy_data *gd = q->elevator->elevator_data;

    if(rq->queuelist.prev == &gd->downhill) {
        if(gd->uphill.next == &gd->uphill) {
            return NULL;
        }
        return list_entry(gd->uphill.next, struct request, queuelist);
    }
    return list_entry(rq->queuelist.next, struct request, queuelist);
}

static int greedy_init_queue(struct request_queue *q, struct elevator_type *e)
{
    struct greedy_data *gd;
    struct elevator_queue *eq;

    eq = elevator_alloc(q, e);
    if (!eq)
        return -ENOMEM;

    gd = kmalloc_node(sizeof(*gd), GFP_KERNEL, q->node);
    if (!gd) {
        kobject_put(&eq->kobj);
        return -ENOMEM;
    }
    eq->elevator_data = gd;

    INIT_LIST_HEAD(&gd->uphill);
    INIT_LIST_HEAD(&gd->downhill);

    spin_lock_irq(q->queue_lock);
    q->elevator = eq;
    spin_unlock_irq(q->queue_lock);
    return 0;
}

static void greedy_exit_queue(struct elevator_queue *e)
{
    struct greedy_data *gd = e->elevator_data;

    BUG_ON(!list_empty(&gd->uphill));
    BUG_ON(!list_empty(&gd->downhill));
    kfree(gd);
}

static struct elevator_type elevator_greedy = {
    .ops = {
        .elevator_merge_req_fn        = greedy_merged_requests,
        .elevator_dispatch_fn        = greedy_dispatch,
        .elevator_add_req_fn        = greedy_add_request,
        .elevator_former_req_fn        = greedy_former_request,
        .elevator_latter_req_fn        = greedy_latter_request,
        .elevator_init_fn        = greedy_init_queue,
        .elevator_exit_fn        = greedy_exit_queue,
    },
    .elevator_name = "greedy",
    .elevator_owner = THIS_MODULE,
};

static int __init greedy_init(void)
{
    return elv_register(&elevator_greedy);
}

static void __exit greedy_exit(void)
{
    elv_unregister(&elevator_greedy);
}

module_init(greedy_init);
module_exit(greedy_exit);


MODULE_AUTHOR("Clemson Tigers");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Greedy IO scheduler");
