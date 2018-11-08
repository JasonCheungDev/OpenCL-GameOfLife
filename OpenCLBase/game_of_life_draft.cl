// Not used kernel. Here for reference.

//input array, //output array
__kernel void GameOfLife(__global uchar4* input, __global uchar4* output, int height, int width, int alive)
{
    //get our screen array co-ords
	const int2 pos = { get_global_id(0), get_global_id(1) };

    //convert to get our position in the data
    size_t id = (pos.y  * get_global_size(0) + pos.x);

	int neighbours_alive = 0;
	for (int x = -1; x <= 1; x++)
	{
		for (int y = -1; y <= 1; y++)
		{
			 //don't count yourself 
			if (!( x == 0 && y == 0))
			{
                //check that our x position is not out of bounds
                if (!((pos.x + x) < 0 || (pos.x + x) >= width)) {
                    //check that our y position is not out of bounds.
                    if (!((pos.y + y) < 0 || (pos.y + y) >= height)) {
                        //check neigbour cell
                        size_t tempId = ((pos.y + y)  * get_global_size(0) + (pos.x + x));
                        if (input[tempId][1] > 0 || input[tempId][0] > 0) {
                            neighbours_alive++;
                        }
                    }
                }
			}
		}
	}

    output[id][0] = 0;
    //determine if we are alive
	//bool isAlive = (input[id][1] > 0);
 //   output[id] = input[id];
 //   
 //   if (input[id][1] > 0) {
 //       output[id][1] = 0;
 //   }
 //   else {
 //       output[id][1] = 255;
 //   }
    
    //output[id][1] = 255;
    //if alive
	if (input[id][1] > 0 || input[id][0] > 0)
	{
		
        // rule 2 
        if (neighbours_alive == 2 || neighbours_alive == 3)
        {
            output[id][1] = 255;
        }

        // rule 3
		else
		{
            output[id][1] = 0;
		}
        
	}
	else
	{
        // rule 1
        if (neighbours_alive == 3)
        {
            output[id][1] = 255;
        }
		
		else
		{
            output[id][1] = 0;
		}
	}
}