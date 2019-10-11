__kernel void GetBestFitness(__global int *fitness,__global int *pop,__global int *sol,__global int *res,__global int best,)
{
	 unsigned int work_item_indx = get_local_id(1)*get_local_size(0) + get_local_id(0);
	 unsigned int work_group_indx = get_group_id(1)*get_num_groups(0) + get_group_id(0);
	 unsigned int gi = (get_local_size(0)*get_local_size(1))* work_group_indx + work_item_indx;

	 if(gi<N_ind)
	 {
		 int local = fitness[0];
		 if(fitness[gi]<=local)
		 {
			 local = fitness[gi];
		 }
		 barrier(CLK_LOCAL_MEM_FENCE);
	         if(best<0)
		 {
			 best = local;
			 winner = gi;
		 }
		 else
		 {
			 if(local<best)
			 {
				 best = local;
				 winner = gi;
				 res[0] = best;
				 res[1] = winner;
			 }
		 }
		 barrier(CLK_LOCAL_MEM_FENCE);

	 }

}
