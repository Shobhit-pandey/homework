/*
 * elv_dispatch_sort is the function that actually adds the request to
 * the dispatch queue. It is exported...so it needs to be edited?
 */
static int sstf_dispatch(struct request_queue *q, int force)
{
	struct sstf_data *nd = q->elevator->elevator_data;
	struct request *closest_rq;
	int abs, closest = -1;

	
	if (!list_empty(&nd->queue)) {
		struct request *rq;
        	/* 
		 * Go through the request queue and find the smallest difference
		 * between the last sector in the dispatch queue and the first
		 * sector of each request 
		 */
		list_for_each_entry(rq, &nd->queue, queuelist) {
		
			/*
		 	* For each request, get the difference between the last
		 	* sector in the dispatch queue, and the first sector of the current request 
			*/
			abs = find_last_sector(rq);
			/*
			 * If the difference is smaller than the current smallest
			 * difference, update closest and closest_rq. If this is the first
			 * item in the list, update.
			 */
			if( closest > abs || closest == -1 ) {
				closest = abs;
				closest_rq = rq;
			}
		}
			 
		last_sector = blk_rq_pos(closest_rq) + 
				blk_rq_sectors(closest_rq);

		list_del_init(&closest_rq->queuelist);
		elv_dispatch_add_tail(q, closest_rq);
		
		printk("[SSTF] dsp %u %llu\n", 
			(current_rq->cmd_flags & REQ_WRITE),
			(unsigned long long)blk_rq_pos(closest_rq));
		return 1;
	}
	return 0;
}

