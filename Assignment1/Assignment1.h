#pragma once
#define GRIDSIZE 128
#define VERTEX_COMPONENTS 3
#define QUAD_COMPONENTS 4

float verts[GRIDSIZE * GRIDSIZE * VERTEX_COMPONENTS];
GLushort elems[GRIDSIZE * GRIDSIZE * QUAD_COMPONENTS];

inline void generateGrid()
{
		for (auto j = 0; j < GRIDSIZE; j++)
		{	
			for (auto i = 0; i < GRIDSIZE; i++)
			{
				auto pos = j * GRIDSIZE + i, index = pos * VERTEX_COMPONENTS;
				verts[index] = j;
				verts[index + 1] = 0.0f;
				verts[index + 2] = -i;

				if (j != (GRIDSIZE - 1) && i != (GRIDSIZE - 1))
				{
					auto epos = pos * QUAD_COMPONENTS, npos = pos + GRIDSIZE;
					elems[epos] = pos;
					elems[epos + 1] = npos;
					elems[epos + 2] = npos + 1;
					elems[epos + 3] = pos + 1;
				}
			}
		}
}