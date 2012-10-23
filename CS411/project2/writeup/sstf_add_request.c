/*
 * This will not need to be touched.
 * adding a request will just add it to the end of the list
 */
static void sstf_add_request(struct request_queue *q, struct request *rq)
{
	struct sstf_data *nd = q->elevator->elevator_data;

	list_add_tail(&rq->queuelist, &nd->queue);
	printk("[SSTF] add %u %llu\n",
		(rq->cmd_flags & REQ_WRITE),
		(unsigned long long)blk_rq_pos(rq));
}
