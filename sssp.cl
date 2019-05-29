typedef struct
{
	int starting;
	int no_of_edges;
}Node;

__kernel void kernel1(__global const Node* h_graph_nodes,
		__global int* h_graph_edges,
		__global int* h_graph_mask,
		__global int* h_updating_graph_mask,
		__global int* h_graph_visited,
		__global int* h_cost_ref,
		__global int* no_of_nodes,
		__global int* h_graph_weights,
		__global int* h_graph_upcost) 
{
	int tid = get_global_id(0);
	if(tid<*no_of_nodes && h_graph_mask[tid] != 0)
	{
		h_graph_mask[tid] = 0;
		int max = (h_graph_nodes[tid].no_of_edges + h_graph_nodes[tid].starting);
		for(int i = h_graph_nodes[tid].starting; i < max; i++)
		{
			int id = h_graph_edges[i];
			if(h_graph_upcost[id]>h_cost_ref[tid]+h_graph_weights[i])
			{
				atom_min(&h_graph_upcost[id],h_cost_ref[tid]+h_graph_weights[i]);
				h_updating_graph_mask[id] = 1;
			}
		}
	}
}

__kernel void kernel2(__global int* h_graph_mask,
		__global int* h_updating_graph_mask,
		__global int* h_graph_visited,
		__global int* h_over,
		__global int* no_of_nodes,
		__global int* h_graph_upcost,
		__global int* h_cost_ref)
{
	int tid = get_global_id(0);
	
	
	if(tid < *no_of_nodes && h_updating_graph_mask[tid] == 1)
	{
		h_graph_mask[tid] = 1;
		h_graph_visited[tid] = 1;
		*h_over = 1;
		h_updating_graph_mask[tid] = 0;
		if(h_cost_ref[tid]>h_graph_upcost[tid])
		{
			h_cost_ref[tid]=h_graph_upcost[tid];
		}
		h_graph_upcost[tid]=h_cost_ref[tid];
	}
	//
}
