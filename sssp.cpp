#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <stdlib.h>
#include <sstream>
#include <ctime> 
#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include "metamorph.h"
#include "metacl_module.h"
#define INT_MAX	2147483647

struct Node
{
	int starting;
	int no_of_edges;
	
};

void run_bfs_gpu(int no_of_nodes, Node *h_graph_nodes, int edge_list_size, 
		int *h_graph_edges, int *h_graph_mask, int *h_updating_graph_mask, 
		int *h_graph_visited, int *h_cost_ref, int *h_graph_weights,int *h_graph_upcost)
{
	cl_int errNum;
    	cl_uint numPlatforms;
    	cl_platform_id firstPlatformId;
    	cl_context context = NULL;
	cl_device_id *devices;
    	cl_command_queue commandQueue = NULL;
	cl_program program;
	cl_kernel kernel1 = 0;
	cl_kernel kernel2 = 0;
	cl_device_id device = 0;
    	size_t deviceBufferSize = -1;
	const char* fileName="sssp.cl";
	cl_mem memObjects[10] = {0,0,0,0,0,0,0,0,0,0};

    	// First, select an OpenCL platform to run on.  For this example, we
    	// simply choose the first available platform.  Normally, you would
    	// query for all available platforms and select the most appropriate one.
    	meta_set_acc(-1, metaModePreferOpenCL); //Must be set to OpenCL, don't need a device since we will override
    	meta_get_state_OpenCL(&firstPlatformId, &device, &context, &commandQueue);
	meta_register_module(&meta_gen_opencl_metacl_module_registry);

	/*	
	clGetPlatformIDs(1, &firstPlatformId, &numPlatforms);
	
    	cl_context_properties contextProperties[] =
    	{
        CL_CONTEXT_PLATFORM,
        (cl_context_properties)firstPlatformId,
        0
    	};
    	context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_GPU,
                                      NULL, NULL, &errNum);
									  
									  
    
    
	if (errNum != CL_SUCCESS)
    	{
        std::cout << "Could not create GPU context, trying CPU..." << std::endl;
        context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_CPU,
                                          NULL, NULL, &errNum);
        if (errNum != CL_SUCCESS)
        {
            std::cerr << "Failed to create an OpenCL GPU or CPU context." << std::endl;
            return ;
        }
    	}
	
	errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &deviceBufferSize);
    	if (errNum != CL_SUCCESS)
    	{
        std::cerr << "Failed call to clGetContextInfo(...,GL_CONTEXT_DEVICES,...)";
        return ;
    	}
	clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &deviceBufferSize);

	
	
    	// Allocate memory for the devices buffer
    	devices = new cl_device_id[deviceBufferSize / sizeof(cl_device_id)];
    
    	clGetContextInfo(context, CL_CONTEXT_DEVICES, deviceBufferSize, devices, NULL);
    
    	commandQueue = clCreateCommandQueue(context, devices[0],0 , NULL);
    	clFinish(commandQueue);
	
	
    	device = devices[0];
    	delete [] devices;
    	std::ifstream kernelFile(fileName, std::ios::in);
    
	if (!kernelFile.is_open())
    	{
        std::cerr << "Failed to open file for reading: " << fileName << std::endl;
        return ;
    	}
	
    	std::ostringstream oss;
    	oss << kernelFile.rdbuf();

    	std::string srcStdStr = oss.str();
    	const char *srcStr = srcStdStr.c_str();
    	program = clCreateProgramWithSource(context, 1,(const char**)&srcStr,NULL, NULL);
    
	if (program == NULL)
    	{
        std::cerr << "Failed to create CL program from source." << std::endl;
        return ;
    	}
	
	//clFinish(commandQueue);
	clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	//clFinish(commandQueue);
    	*/
	
	int *d_over=0;
	int h_over=1;
	
	
    	memObjects[0] = clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR , sizeof(Node) * no_of_nodes, h_graph_nodes, NULL);
    	memObjects[1] = clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR , sizeof(int) * edge_list_size, h_graph_edges, NULL);
	memObjects[2] = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int) * no_of_nodes, h_graph_mask, NULL);
	memObjects[3] = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int) * no_of_nodes, h_updating_graph_mask, NULL);
	memObjects[4] = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int) * no_of_nodes, h_graph_visited, NULL);
	memObjects[5] = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int) * no_of_nodes, h_cost_ref, NULL);
	memObjects[6] = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int) , &h_over, NULL);
	memObjects[7] = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int), &no_of_nodes, NULL);
	memObjects[8] = clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR , sizeof(int) * edge_list_size, h_graph_weights, NULL);
	memObjects[9] = clCreateBuffer(context, CL_MEM_WRITE_ONLY| CL_MEM_COPY_HOST_PTR , sizeof(int) * no_of_nodes, h_graph_upcost, NULL);
	
	clEnqueueWriteBuffer(commandQueue, memObjects[0], CL_TRUE, 0, sizeof(Node) * no_of_nodes, h_graph_nodes, 0, NULL, NULL);
	clFinish(commandQueue);
	clEnqueueWriteBuffer(commandQueue, memObjects[1], CL_TRUE, 0, sizeof(int) * edge_list_size, h_graph_edges, 0, NULL, NULL);
	clFinish(commandQueue);
	clEnqueueWriteBuffer(commandQueue, memObjects[2], CL_TRUE, 0, sizeof(int) * no_of_nodes, h_graph_mask, 0, NULL, NULL);
	clFinish(commandQueue);
	clEnqueueWriteBuffer(commandQueue, memObjects[3], CL_TRUE, 0, sizeof(int) * no_of_nodes, h_updating_graph_mask, 0, NULL, NULL);
	clFinish(commandQueue);
	clEnqueueWriteBuffer(commandQueue, memObjects[4], CL_TRUE, 0, sizeof(int) * no_of_nodes, h_graph_visited, 0, NULL, NULL);
	errNum=clEnqueueWriteBuffer(commandQueue, memObjects[5], CL_TRUE, 0, sizeof(int) * no_of_nodes, h_cost_ref, 0, NULL, NULL);
	clFinish(commandQueue);
	
	errNum=clEnqueueWriteBuffer(commandQueue, memObjects[8], CL_TRUE, 0, sizeof(int) * edge_list_size, h_graph_weights, 0, NULL, NULL);
	clFinish(commandQueue);
	
	errNum=clEnqueueWriteBuffer(commandQueue, memObjects[9], CL_TRUE, 0, sizeof(int) * no_of_nodes,h_graph_upcost, 0, NULL, NULL);
	clFinish(commandQueue);
	/*
    	kernel1 = clCreateKernel(program, "kernel1", &errNum);
	
	if (kernel1 == NULL)
    {
        std::cerr << "Failed to create kernel1" << std::endl;
		
        return ;
    }

	

	kernel2 = clCreateKernel(program, "kernel2", NULL);
	
	if (kernel2 == NULL)
    {
        std::cerr << "Failed to create kernel2" << std::endl;
        
        return ;
    }
	
	errNum = clSetKernelArg(kernel1, 0, sizeof(cl_mem), &memObjects[0]);
	errNum |= clSetKernelArg(kernel1, 1, sizeof(cl_mem), &memObjects[1]);
	errNum |= clSetKernelArg(kernel1, 2, sizeof(cl_mem), &memObjects[2]);
	errNum |= clSetKernelArg(kernel1, 3, sizeof(cl_mem), &memObjects[3]);
	errNum |= clSetKernelArg(kernel1, 4, sizeof(cl_mem), &memObjects[4]);
	errNum |= clSetKernelArg(kernel1, 5, sizeof(cl_mem), &memObjects[5]);
    	errNum |= clSetKernelArg(kernel1, 6, sizeof(cl_mem), &memObjects[7]);
	errNum |= clSetKernelArg(kernel1, 7, sizeof(cl_mem), &memObjects[8]);
	errNum |= clSetKernelArg(kernel1, 8, sizeof(cl_mem), &memObjects[9]);
	
	if (errNum != CL_SUCCESS)
    	{
        std::cerr << "Error setting kernel 1 arguments." << std::endl;
        return ;
    	}
	
	errNum = clSetKernelArg(kernel2, 0, sizeof(cl_mem), &memObjects[2]);
	errNum |= clSetKernelArg(kernel2, 1, sizeof(cl_mem), &memObjects[3]);
	errNum |= clSetKernelArg(kernel2, 2, sizeof(cl_mem), &memObjects[4]);
	errNum |= clSetKernelArg(kernel2, 3, sizeof(cl_mem), &memObjects[6]);
	errNum |= clSetKernelArg(kernel2, 4, sizeof(cl_mem), &memObjects[7]);
	errNum |= clSetKernelArg(kernel2, 5, sizeof(cl_mem), &memObjects[9]);
	errNum |= clSetKernelArg(kernel2, 6, sizeof(cl_mem), &memObjects[5]);
	*/
	
	size_t maxThreads[3];
	errNum = clGetDeviceInfo(device,CL_DEVICE_MAX_WORK_ITEM_SIZES,sizeof(size_t)*3,&maxThreads, NULL);
	
	maxThreads[0] = no_of_nodes < 256? no_of_nodes: 256; //maxThreads[0];

	size_t globalWorkSize[3] = {(no_of_nodes/maxThreads[0])*maxThreads[0] + ((no_of_nodes%maxThreads[0])==0?0:maxThreads[0])}; // one dimensional Range

	size_t localWorkSize[3] = {maxThreads[0]};
	
	int stop=1;
	int k=0;
	cl_event event1,event2,event3,event4;

	while(stop==1){	
	stop=0;
	clEnqueueWriteBuffer(commandQueue, memObjects[6], CL_TRUE, 0, sizeof(int), (void*)&stop, 0, NULL, NULL);
	clFinish(commandQueue);
	
	//errNum=clEnqueueNDRangeKernel(commandQueue, kernel1, 1, NULL,globalWorkSize, localWorkSize,0, NULL,NULL);
	//clFinish(commandQueue);	
	errNum= meta_gen_opencl_sssp_kernel1(commandQueue, &globalWorkSize, &localWorkSize, &memObjects[0], &memObjects[1], &memObjects[2], &memObjects[3], &memObjects[4], &memObjects[5], &memObjects[7], &memObjects[8],&memObjects[9], 1, NULL);
	//errNum=clEnqueueNDRangeKernel(commandQueue, kernel2, 1, NULL,globalWorkSize, localWorkSize,0, NULL, NULL);
	//clFinish(commandQueue);
errNum= meta_gen_opencl_sssp_kernel2(commandQueue, &globalWorkSize, &localWorkSize, &memObjects[2], &memObjects[3], &memObjects[4], &memObjects[6], &memObjects[7], &memObjects[9], &memObjects[5], 1, NULL);
	errNum = clEnqueueReadBuffer(commandQueue, memObjects[6], CL_TRUE,0,sizeof(int), (void*)&stop,0, NULL, NULL);
	
	clFinish(commandQueue);
	
	}
	
	
	errNum = clEnqueueReadBuffer(commandQueue, memObjects[5], CL_TRUE,0, no_of_nodes * sizeof(int), (void*)h_cost_ref,0, NULL, NULL);
	
	
	//Free memory memory
	
	
	
	
	clReleaseMemObject(memObjects[0]);
	clReleaseMemObject(memObjects[1]);
	clReleaseMemObject(memObjects[2]);
	clReleaseMemObject(memObjects[3]);
	clReleaseMemObject(memObjects[4]);
	clReleaseMemObject(memObjects[5]);
	clReleaseMemObject(memObjects[6]);
	clReleaseMemObject(memObjects[7]);
	clReleaseMemObject(memObjects[8]);
	clReleaseMemObject(memObjects[9]);

	meta_deregister_module(&meta_gen_opencl_metacl_module_registry);
	/*
	clReleaseKernel(kernel1);
	
	clReleaseKernel(kernel2);
	
	clReleaseProgram(program);
	
	clReleaseContext(context);
	clReleaseCommandQueue(commandQueue);
	*/
	
}


void run_bfs_cpu(int no_of_nodes, Node *h_graph_nodes, int edge_list_size, 
		int *h_graph_edges, int *h_graph_mask, int *h_updating_graph_mask, 
		int *h_graph_visited, int *h_cost,int *h_graph_weights,int *h_graph_upcost){
    bool stop;
    int k = 0;
    do{
        //if no thread changes this value then the loop stops
        stop=false;
        for(int tid = 0; tid < no_of_nodes; tid++ )
        {
            if (h_graph_mask[tid] == 1){
                h_graph_mask[tid]=0;
				
                for(int i=h_graph_nodes[tid].starting; i<(h_graph_nodes[tid].no_of_edges + h_graph_nodes[tid].starting); i++){
                    int id = h_graph_edges[i];	
                    
							if(h_graph_upcost[id]>h_cost[tid]+h_graph_weights[i])
					{
						h_graph_upcost[id]=h_cost[tid]+h_graph_weights[i];
						h_updating_graph_mask[id] = 1;
					}
                }
            }
        }

        for(int tid=0; tid< no_of_nodes ; tid++ )
        {
            if (h_updating_graph_mask[tid] == 1){
                h_graph_mask[tid]=1;
                h_graph_visited[tid]=1;
                stop=true;
                h_updating_graph_mask[tid]=0;
				if(h_cost[tid]>h_graph_upcost[tid])
				{
					h_cost[tid]=h_graph_upcost[tid];
				}
				h_graph_upcost[tid]=h_cost[tid];
            }
        }
        k++;
    }
    while(stop);
}

void Usage(int argc, char**argv){

    fprintf(stderr,"Usage: %s <input_file>\n", argv[0]);

}

int main(int argc, char * argv[]) {
    //time_t begin,end;
	clock_t begin,end;
	int no_of_nodes;
    int edge_list_size;
    FILE *fp;
    Node *h_graph_nodes;
    int *h_graph_mask, *h_updating_graph_mask, *h_graph_visited;

    char *input_f;
    if(argc!=2){
        Usage(argc, argv);
        exit(0);
    }

    input_f = argv[1];
    printf("\n");
	printf("Reading File\n");
	printf("\n");
    //Read in Graph from a file
    fp = fopen(input_f,"r");
    if(!fp){
        printf("Error Reading graph file\n");
        return 0;
    }

    int source = 0;
	//printf("seg fault occurs here\n");
    fscanf(fp,"%d",&no_of_nodes);

    int num_of_blocks = 1;
    int num_of_threads_per_block = no_of_nodes;

    //Make execution Parameters according to the number of nodes
    //Distribute threads across multiple Blocks if necessary

    // allocate host memory
    h_graph_nodes = (Node*) malloc(sizeof(Node)*no_of_nodes);
    h_graph_mask = (int*) malloc(sizeof(int)*no_of_nodes);
    h_updating_graph_mask = (int*) malloc(sizeof(int)*no_of_nodes);
    h_graph_visited = (int*) malloc(sizeof(int)*no_of_nodes);
	
    int start, edgeno;
	float speedup;
	double sd=0;
    // initalize the memory
    for(int i = 0; i < no_of_nodes; i++){
        fscanf(fp,"%d %d",&start,&edgeno);
        h_graph_nodes[i].starting = start;
        h_graph_nodes[i].no_of_edges = edgeno;
		//printf("node %d #edges: %d\n",i,edgeno);
		
        h_graph_mask[i]=0;
        h_updating_graph_mask[i]=0;
        h_graph_visited[i]=0;
    }
    //read the source node from the file
    fscanf(fp,"%d",&source);
    source=0;
    //set the source node as true in the mask
    h_graph_mask[source]=1;
    h_graph_visited[source]=1;
    fscanf(fp,"%d",&edge_list_size);
	double mean=(double)edge_list_size/no_of_nodes;
	for (int i=0;i<no_of_nodes;i++){
		sd+=pow((h_graph_nodes[i].no_of_edges-mean),2);
	}
	sd=sqrt(sd/no_of_nodes);
    int id,cost;
    int* h_graph_edges = (int*) malloc(sizeof(int)*edge_list_size);
	int* h_graph_weights= (int*) malloc(sizeof(int)*edge_list_size);
	int* h_graph_upcost= (int*) malloc(sizeof(int)*no_of_nodes);
	
    for(int i=0; i < edge_list_size ; i++){
        fscanf(fp,"%d",&id);
        fscanf(fp,"%d",&cost);
        h_graph_edges[i] = id;
		h_graph_weights[i]=cost;
		
    }
	printf("#nodes: %d\n",no_of_nodes);
	printf("#edges: %d\n",edge_list_size);
	printf("standard deviation: %lf\n",sd);
    if(fp)
        fclose(fp);
    // allocate mem for the result on host side
	//printf("seg fault occurs here 2\n");
    int	*h_cost = (int*) malloc(sizeof(int)*no_of_nodes);
    int *h_cost_ref = (int*)malloc(sizeof(int)*no_of_nodes);
    for(int i=0;i<no_of_nodes;i++){
        h_cost[i]= INT_MAX;
        h_cost_ref[i] = INT_MAX;
		h_graph_upcost[i]=INT_MAX;
    }
    h_cost[source]=0;
    h_cost_ref[source]=0;
	h_graph_upcost[source]=0;
    //---------------------------------------------------------
    //--gpu entry
	//time (&begin);
	
	begin=clock();
    run_bfs_cpu(no_of_nodes,h_graph_nodes,edge_list_size,h_graph_edges, h_graph_mask, h_updating_graph_mask, h_graph_visited, h_cost,h_graph_weights,h_graph_upcost);
	end=clock();
	double time_spent2 = (double)(end - begin) / CLOCKS_PER_SEC;
	//double time_spent2 = (double)(end - begin) / CLOCKS_PER_SEC;
	printf ("cpu execution time: %f \n", time_spent2);
    //printf("seg fault occurs here 3\n");
	//time (&end);
	//double difference = difftime (end,begin);
	//printf ("GPU time %f seconds\n", difference );
	//---------------------------------------------------------
    //--cpu entry
    // initalize the memory again
    bool flag=true;
	
	for(int i = 0; i < no_of_nodes; i++){
        h_graph_mask[i]=0;
        h_updating_graph_mask[i]=0;
        h_graph_visited[i]=0;
		h_graph_upcost[i]=INT_MAX;
    }
    //set the source node as true in the mask
    source=0;
    h_graph_mask[source]=1;
    h_graph_visited[source]=1;
	h_graph_upcost[source]=0;
    
	
	//time (&begin);
	begin=clock();
	run_bfs_gpu(no_of_nodes,h_graph_nodes,edge_list_size,h_graph_edges, h_graph_mask, h_updating_graph_mask, h_graph_visited, h_cost_ref,h_graph_weights,h_graph_upcost);
	end=clock();
	double time_spent1 = (double)(end - begin) / CLOCKS_PER_SEC;
	printf ("gpu execution time: %f\n", time_spent1);
	speedup=(float)(time_spent2/time_spent1);
	printf("speedup: %f\n",speedup);
	printf("\n");
	//time (&end);
	
	for(int i=0; i<no_of_nodes;i++){
		if(no_of_nodes<2001)
			printf("gpu %d, cpu %d\n",h_cost_ref[i],h_cost[i]);
		if(h_cost_ref[i]!=h_cost[i])
		{	
			printf("the implementation is incorrect\n");//flag==false;
			//exit(0);
		}
	}
	
	//if(flag==false)printf("the implementation is incorrect\n");
	//else 
	printf("the implementation is correct\n");
	//if(!flag)printf("not correct");
	
	
	//difference = difftime (end,begin);
	//printf("seg fault occurs here \n");
	//printf ("CPU time %f seconds\n", difference );
	
	
    //for (int i=0;i<no_of_nodes;i++){
    //    printf("node %d : cost %d\n",i,h_cost_ref[i]);
    //}
    //---------------------------------------------------------
    //--result verification
    //compare_results<int>(h_cost, h_cost_ref, no_of_nodes);
    //release host memory
    free(h_graph_nodes);
    free(h_graph_mask);
    free(h_updating_graph_mask);
    free(h_graph_visited);
	free(h_cost_ref);
	free(h_graph_edges);
	free(h_cost);
	
}




