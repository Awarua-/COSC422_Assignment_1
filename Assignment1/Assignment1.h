#pragma once
#define GRIDSIZE 128
#define HEIGHTSCALE 10
#define VERTEX_COMPONENTS 3
#define QUAD_COMPONENTS 4

float verts[GRIDSIZE * GRIDSIZE * VERTEX_COMPONENTS];
GLushort elems[GRIDSIZE * GRIDSIZE * QUAD_COMPONENTS];

inline void generateGrid()
{
		for (int j = 0; j < GRIDSIZE; j++)
		{	
			for (int i = 0; i < GRIDSIZE; i++)
			{
				int pos = j * GRIDSIZE + i; int index = pos * VERTEX_COMPONENTS;
				verts[index] = float(j);
				verts[index + 1] = 0.0f;
				verts[index + 2] = float(-i);

				if (j != (GRIDSIZE - 1) && i != (GRIDSIZE - 1))
				{
					int epos = pos * QUAD_COMPONENTS; int npos = pos + GRIDSIZE;
					elems[epos] = pos;
					elems[epos + 1] = npos;
					elems[epos + 2] = npos + 1;
					elems[epos + 3] = pos + 1;
				}
			}
		}
}