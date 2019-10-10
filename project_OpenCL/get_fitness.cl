__kernel void GetFitness(__global int *fitness,__global int *popmat,__global int *admat,int N_reg,int N_ind)
{
	  /* Access work-item/work-group information */
   unsigned int work_item_indx = get_local_id(1)*get_local_size(0) + get_local_id(0);
   unsigned int work_group_indx = get_group_id(1)*get_num_groups(0) + get_group_id(0);
   unsigned int gi = (get_local_size(0)*get_local_size(1))* work_group_indx + work_item_indx;
   if(gi < (unsigned int)N_ind)
   {
	   int sum=0;
	   for(int k=0; k<N_reg; k++)
	   {
		   for(int j=0; j<N_reg; j++)
		   {
			   if(admat[k*N_reg + j]==1)
			   {
				   if(popmat[gi*N_reg + k]==popmat[gi*N_reg + j])
				   {
   					   barrier(CLK_LOCAL_MEM_FENCE);
					   sum = sum + 1;

				   }
			   }
		   }
	   }
	   fitness[gi] = sum;
   }

   barrier(CLK_LOCAL_MEM_FENCE);



}

