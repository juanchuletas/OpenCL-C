__kernel void function_x2_y2(__global float *output,__global float *step,__global float *point,int Ny,int Nz,int Ntot) { 
 

    /*LOCAL VARIABLES TO THE WORK-ITEMS*/
    unsigned int indx_x,indx_y,indx_z;
    float x_point,y_point,z_point,target,r,r2;

   /* Access work-item/work-group information */
   unsigned int work_item_indx = get_local_id(1)*get_local_size(0) + get_local_id(0);
   unsigned int work_group_indx = get_group_id(1)*get_num_groups(0) + get_group_id(0);
   unsigned int global_indx  = (get_local_size(0)*get_local_size(1))* work_group_indx + work_item_indx;

   if(global_indx < (unsigned int)Ntot)
   {
	indx_x = (unsigned int)(global_indx/Nz)/Ny;
	indx_y = (unsigned int)(global_indx/Nz)%Ny;
        indx_z = (unsigned int)(global_indx%Nz);
	
	x_point = point[0] + indx_x*step[0];
	y_point = point[1] + indx_y*step[1];
	z_point = point[2] + indx_z*step[2];
        
        r2 = x_point*x_point + y_point*y_point + z_point*z_point;
        r = sqrt(r2);
  
        target = exp(-r);
        target = target*(x_point*x_point - y_point*y_point);
        output[global_indx] = target;
   }
   else
   {
	output[global_indx] = 0.0;
   }
   
   //output[global_indx] = point[0];

   barrier(CLK_LOCAL_MEM_FENCE);
}
