/*
 * slob_alloc: entry point into the slob allocator.
 */
static void *slob_alloc(size_t size, gfp_t gfp, int align, int node)
{
	struct slob_page *sp = NULL;
	struct slob_page *sp_t;
	struct list_head *slob_list;
	slob_t *b = NULL;
	unsigned long flags;

	if (size < SLOB_BREAK1)
		slob_list = &free_slob_small;
	else if (size < SLOB_BREAK2)
		slob_list = &free_slob_medium;
	else
		slob_list = &free_slob_large;

	spin_lock_irqsave(&slob_lock, flags);
	/* Iterate through each partially free page, try to find room */
	list_for_each_entry(sp_t, slob_list, list) {
#ifdef CONFIG_NUMA
		/*
		 * If there's a node specification, search for a partial
		 * page with a matching node id in the freelist.
		 */
		if (node != -1 && page_to_nid(&sp_t->page) != node)
			continue;
#endif
		/* Enough room on this page? */
		if (sp_t->units < SLOB_UNITS(size))
			/* Not enough room */
			continue;

		if (sp == NULL)
			sp = sp_t;

		if (sp_t->units < sp->units)
			/* Get the smallest slob_page that
 			 * is large enough for our needs */
			sp = sp_t;
	}

	/* Attempt to alloc */
	if(sp != NULL) {
		b = slob_page_alloc(sp, size, align);
	}
	spin_unlock_irqrestore(&slob_lock, flags);

	/* Not enough space: must allocate a new page */
	if (!b) {
		b = slob_new_pages(gfp & ~__GFP_ZERO, 0, node);
		if (!b)
			return NULL;
		sp = slob_page(b);
		set_slob_page(sp);
		
		/* We allocatted a new page, increment the count */
		slob_page_count++;


		spin_lock_irqsave(&slob_lock, flags);
		sp->units = SLOB_UNITS(PAGE_SIZE);
		sp->free = b;
		INIT_LIST_HEAD(&sp->list);
		set_slob(b, SLOB_UNITS(PAGE_SIZE), b + SLOB_UNITS(PAGE_SIZE));
		set_slob_page_free(sp, slob_list);
		b = slob_page_alloc(sp, size, align);
		BUG_ON(!b);
		spin_unlock_irqrestore(&slob_lock, flags);
	}
	if (unlikely((gfp & __GFP_ZERO) && b))
		memset(b, 0, size);
	return b;
}

asmlinkage long sys_slob_claimed(void)
{
	long slob_total = SLOB_UNITS(PAGE_SIZE) * slob_page_count;
	return slob_total;
}

/*
 * Return a count of all free units across used pages */
asmlinkage long sys_slob_free(void)
{
	long free_units = 0;
	struct slob_page *sp;
	struct list_head *slob_list;

	/* Pages with small blocks (page units < 256) */
	slob_list = &free_slob_small;
	list_for_each_entry(sp, slob_list, list) {
		free_units += sp->units;
	}

	/* Pages with medium blocks (page units < 1024) */
	slob_list = &free_slob_medium;
	list_for_each_entry(sp, slob_list, list) {
		free_units += sp->units;
	}

	/* Pages with large blocks (1024 < page units < PAGE_SIZE) */
	slob_list = &free_slob_large;
	list_for_each_entry(sp, slob_list, list) {
		free_units += sp->units;
	}
	return free_units;
}
