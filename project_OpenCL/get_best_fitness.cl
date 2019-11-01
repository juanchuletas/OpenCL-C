__kernel void GetBestFitness(__global int *fitness,__global int *pop,__global int *best_global,__global int *data,__local int *fit_loc,__local int *target,int N_ind)
{
	 unsigned int work_item_indx = get_local_id(1)*get_local_size(0) + get_local_id(0);
	 unsigned int work_group_indx = get_group_id(1)*get_num_groups(0) + get_group_id(0);
	 unsigned int gi = (get_local_size(0)*get_local_size(1))* work_group_indx + work_item_indx;

	
	 if(gi < (unsigned int)N_ind)
	 {
           
		 fit_loc[work_item_indx] = fitness[gi];
		 target[work_group_indx] = fit_loc[0];
                 __local int winner;
                 __local int shoot;
                 __local int thread;
                 winner = -1;
                 /*if(fit_loc[work_item_indx]<=target[work_group_indx])
                 {
                 	 //target[work_group_indx] = fit_loc[work_item_indx];
                         shoot  = fit_loc[work_item_indx];
                 }*/
                 barrier(CLK_LOCAL_MEM_FENCE);
                 if(winner<0)
                 {
                         winner = fit_loc[work_item_indx];
                         thread = work_item_indx;
                 }
                 else
                 {
                         if(fit_loc[work_item_indx]<winner)
                         {
                               winner = fit_loc[work_item_indx];
                               thread = work_item_indx;
                         }      
                 }
                 barrier(CLK_LOCAL_MEM_FENCE);
                 best_global[work_group_indx] = winner;
                 /*if(work_item_indx==0)
                 {
                     data[0] = winner;
                     data[1] = thread;
                 }*/
	 }

}

