__kernel void new_kernel(__global float *output,__global float *step,__global float *point,int z_pts,int y_pts) 
{
	float x,y,z;
	int indx_x,indx_y,indx_z;

	indx_x = get_global_id(0);
	indx_y = get_global_id(1)/z_pts;
	indx_z = get_global_id(1)%z_pts;

	w_index = get_global_id(0)*y_pts*z_pts + get_global_id(1);

    

   	if(indx_x < x_pts && indx_y < y_pts && indx_z < z_pts)
   	{
		x = point[0] + (float)indx_x*step[0];
		y = point[1] + (float)indx_y*step[1];
		z = point[2] + (float)indx_z*step[2];

		output[w_index]  = 1.0;
	
   	}
   	else
   	{
		output[w_indx] = 0.0;
	}
   
   //output[global_indx] = point[0];

  	 barrier(CLK_LOCAL_MEM_FENCE);
}
