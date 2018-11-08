// GPU specific kernel that utilizes opengl+opencl interoperability (memory sharing). 
__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

__kernel void GameOfLife(
	__read_only image2d_t input, 
	__write_only image2d_t output)
{
	const int2 pos = { get_global_id(0), get_global_id(1) };
	const float4 alive_colour = (float4)(1.0f, 0.0f, 0.0f, 1.0f);
	const float4 dead_colour = (float4)(0.0f, 0.0f, 0.0f, 1.0f);

	int neighbours_alive = 0;
	for (int x = -1; x <= 1; x++)
	{
		for (int y = -1; y <= 1; y++)
		{
			// don't count yourself 
			if (!(x == 0 && y == 0))
			{
				// dead == 0 , alive > 0 
				if (read_imagef(input, sampler, pos + (int2)(x, y)).x > 0)
				{
					neighbours_alive += 1;
				}
			}
		}
	}

	bool alive = read_imagef(input, sampler, pos).x > 0;

	if (!alive)
	{
		// rule 1
		if (neighbours_alive == 3)
		{
			write_imagef(output, pos, alive_colour);
		}
		else
		{
			write_imagef(output, pos, dead_colour);
		}
	}
	else
	{
		// rule 2 
		if (neighbours_alive == 2 || neighbours_alive == 3)
		{
			write_imagef(output, pos, alive_colour);
		}
		// rule 3
		else
		{
			write_imagef(output, pos, dead_colour);
		}
	}
}