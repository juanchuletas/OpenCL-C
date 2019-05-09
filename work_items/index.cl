__kernel void indices(__global int *output) { 

   /* Access work-item/work-group information */
   int work_item_indx = get_local_id(1)*get_local_size(0) + get_local_id(0);
   int work_group_indx = get_group_id(1)*get_num_groups(0) + get_group_id(0);
   int global_indx  = (get_local_size(0)*get_local_size(1))* work_group_indx + work_item_indx;
   
   output[global_indx] = global_indx;
}
