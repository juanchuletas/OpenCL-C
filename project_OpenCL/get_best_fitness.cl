__kernel void GetBestFitness(__global int *fitness,__global int *pop,__global int *best_global,__global int *data,__local int *fit_loc,__local int *target,int N_ind)
{
	 unsigned int work_item_indx = get_local_id(1)*get_local_size(0) + get_local_id(0);
	 unsigned int work_group_indx = get_group_id(1)*get_num_groups(0) + get_group_id(0);
	 unsigned int gi = (get_local_size(0)*get_local_size(1))* work_group_indx + work_item_indx;

	
	 if(gi < (unsigned int)N_ind)
	 {
           
		 fit_loc[work_item_indx] = fitness[gi];
                 __local int local_winner;
		 __local int best_loc_ind;

		 
                 local_winner = fitness[0];

                 barrier(CLK_LOCAL_MEM_FENCE);
		 if(work_item_indx==0)
		 {
			for(int i=0; i<16; i++)
			{
				if(fit_loc[i]<local_winner)
				{	
				       local_winner = fit_loc[i];
				       best_loc_ind = work_group_indx*16 + i;
				}
			}
			
		 }
                 barrier(CLK_GLOBAL_MEM_FENCE);
                 if(work_item_indx == 0)
                 {
			best_global[work_group_indx] = local_winner;
			target[work_group_indx] = best_loc_ind;
                 }
                 barrier(CLK_GLOBAL_MEM_FENCE);
		 int global_winner,best_glob_ind;
		 if(gi==0)
		 {
		 	global_winner = best_global[0];
			best_glob_ind = target[0];
		 	for(int i=0; i<16; i++)
			{
				if(best_global[i]<global_winner)
				{
					global_winner = best_global[i];
					best_glob_ind = target[i];
				}	
			}
			data[0] = global_winner;
			data[1] = best_glob_ind;
		 }


		 

	 }

}

