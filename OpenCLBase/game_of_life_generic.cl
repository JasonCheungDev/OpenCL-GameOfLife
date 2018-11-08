
__kernel void GameOfLife(
	__global uchar4* input, 
	__global uchar4* output, 
	int height, 
	int width, 
	int alive)
{
	// NVIDIA Specific: output[id][0] = ... // ERROR: subscript indexing is NOT allowed.
	// Workaround: Use an union to format the data 
	union
	{
		uchar arr[4];
		uchar4 vec;
	} element;

    // get our screen array co-ords
	const int2 pos = { get_global_id(0), get_global_id(1) };

    // convert to get our position in the data
    size_t id = (pos.y * get_global_size(0) + pos.x);
	
	int neighbours_alive = 0;
	for (int x = -1; x <= 1; x++)
	{
		for (int y = -1; y <= 1; y++)
		{
			// don't count yourself 
			if (!( x == 0 && y == 0))
			{
                // check that our x position is not out of bounds
                if (!(pos.x + x < 0 || pos.x + x >= width || pos.y + y < 0 || pos.y + y >= height)) 
				{
					// check neigbour cell
					size_t neighbourId = (pos.y + y) * get_global_size(0) + (pos.x + x);
					element.vec = input[neighbourId];
					if (element.arr[0] > 0)
					{
						neighbours_alive++;
					}
                }
			}
		}
	}

	uchar4 alive_color = (uchar4)(alive, alive, alive, alive);
	element.vec = input[id];
	bool is_alive = element.arr[0] > 0;
	if (!is_alive)
	{
		// rule 1
		if (neighbours_alive == 3)
		{
			output[id] = alive_color;
		}
		else
		{
			output[id] = 0;
		}
	}
	else
	{
		// rule 2 
		if (neighbours_alive == 2 || neighbours_alive == 3)
		{
			output[id] = alive_color;
		}
		// rule 3
		else
		{
			output[id] = 0;
		}
	}

}