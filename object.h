#include <vector>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstring>
#include "vec3.h"
#include "vec2.h"
#include "matrix44.h"
#include "lodepng.h"

#define min_x 0
#define max_x 1
#define min_y 2
#define max_y 3
#define min_z 4
#define max_z 5

#define M_PI 3.141592653589793

struct Vertex{
	vec3 pos;
	vec2 tex;
	vec3 norm;
};

struct Triangle {

	Vertex vertex[3];

	Triangle( const std::vector<vec3> v , const std::vector<vec2> t , const std::vector<vec3> n)
	{
		vertex[0].pos = v[0];
		vertex[1].pos = v[1];
		vertex[2].pos = v[2];

		vertex[0].tex = t[0];
		vertex[1].tex = t[1];
		vertex[2].tex = t[2];

		vertex[0].norm = n[0];
		vertex[1].norm = n[1];
		vertex[2].norm = n[2];

	}

	~Triangle(){}
};

class Mesh 
{
public:
	std::vector<Triangle> tris;

	Mesh() {}
	~Mesh() {}

	bool load_mesh_from_file(const char* path) 
	{
		tris.clear();
		std::vector< unsigned int > vertexIndices;
		std::vector< unsigned int > textureIndices;
		std::vector< unsigned int > normalIndices;
		
		std::vector< vec3 > temp_vertices;
		std::vector< vec2 > temp_textures;
		std::vector< vec3 > temp_normals;

		std::ifstream f(path);
		if (!f.is_open())
		{
			std::cout << "File cannot be oppened or does not exist\n";
			return false;
		}

		std::cout << "file was  oppened!\n";

		
		while (!f.eof())
		{
			char line[256];
			f.getline(line, 256);

			std::stringstream s;
			s << line;

			char junk;

			if ( line[0] == 'v')
			{
				if (line[1] == 'n') 
				{
					vec3 vertex;
					s >> junk >> junk >> vertex[0] >> vertex[1] >> vertex[2];
					temp_normals.push_back(vertex);
				}
				else if (line[1] == 't') 
				{
					vec2 vertex;
					s >> junk >> junk >> vertex[0] >> vertex[1];
					// printf("vt %d %f %f\n", vertex.x(), vertex.y());
					temp_textures.push_back(vertex);
					
				}
				else {
					vec3 vertex;
					s >> junk >> vertex[0] >> vertex[1] >> vertex[2];
					temp_vertices.push_back(vertex);
				}
			}

			else if ( line[0] == 'f')
			{
				
				std::string vertex1, vertex2, vertex3;
    			unsigned int vertexIndex[3], textureIndex[3], normalIndex[3];
				
   				s >> junk >> vertex1 >> vertex2 >> vertex3;
	/* ---------------------------------------------------------------- */
	/* ---------------------------------------------------------------- */
				int fstslash = vertex1.find("/");
				std::string fst = vertex1.substr(0, fstslash);
				vertexIndex[0] = atoi( fst.c_str() );
				vertex1 = vertex1.substr(fstslash + 1, vertex1.size()-1);
				
				fstslash = vertex2.find("/");
				fst = vertex2.substr(0, fstslash);
				vertexIndex[1] = atoi(fst.c_str());
				vertex2 = vertex2.substr(fstslash + 1, vertex2.size()-1);
				
				fstslash = vertex3.find("/");
				fst = vertex3.substr(0, fstslash);
				vertexIndex[2] = atoi(fst.c_str());
				vertex3 = vertex3.substr(fstslash + 1, vertex3.size()-1);
				//std::cout << "a!\n";
	/* ---------------------------------------------------------------- */
	/* ---------------------------------------------------------------- */
				fstslash = vertex1.find("/");
				fst = vertex1.substr(0, fstslash);
				textureIndex[0] = atoi( fst.c_str() );
				vertex1 = vertex1.substr(fstslash + 1, vertex1.size()-1);
				
				fstslash = vertex2.find("/");
				fst = vertex2.substr(0, fstslash);
				textureIndex[1] = atoi(fst.c_str());
				vertex2 = vertex2.substr(fstslash + 1, vertex2.size()-1);
				
				fstslash = vertex3.find("/");
				fst = vertex3.substr(0, fstslash);
				textureIndex[2] = atoi(fst.c_str());
				vertex3 = vertex3.substr(fstslash + 1, vertex3.size()-1);
	/* ---------------------------------------------------------------- */
	/* ---------------------------------------------------------------- */
				normalIndex[0] = atoi(vertex1.c_str());
				normalIndex[1] = atoi(vertex2.c_str());
				normalIndex[2] = atoi(vertex3.c_str());
	/* ---------------------------------------------------------------- */
	/* ---------------------------------------------------------------- */
 				  vertexIndices.push_back(vertexIndex[0]);
     			  vertexIndices.push_back(vertexIndex[1]);
     			  vertexIndices.push_back(vertexIndex[2]);
//std::cout << "h!\n";
    			textureIndices.push_back(textureIndex[0]);
    			textureIndices.push_back(textureIndex[1]);
    			textureIndices.push_back(textureIndex[2]);
//std::cout << "i!\n";
    			  normalIndices.push_back(normalIndex[0]);
    			  normalIndices.push_back(normalIndex[1]);
    			  normalIndices.push_back(normalIndex[2]);
			}
		}

		for (int i = 0; i < vertexIndices.size(); i+=3)
		{
			//printf("%d\n", i);

		//	std::cout << "aa!\n";
			unsigned int v1 = vertexIndices[i];
			unsigned int v2 = vertexIndices[i+1];
			unsigned int v3 = vertexIndices[i+2];
			
			unsigned int t1 = textureIndices[i];
			unsigned int t2 = textureIndices[i+1];
			unsigned int t3 = textureIndices[i+2];
			
			unsigned int n1 = normalIndices[i];
			unsigned int n2 = normalIndices[i+1];
			unsigned int n3 = normalIndices[i+2];
			
			std::vector<vec3> vertices;
			std::vector<vec2> textures;
			std::vector<vec3> normals;
			
			vertices.push_back(temp_vertices[v1 - 1]);
			//printf("v %d %f %f %f\n", v1, temp_vertices[v1 - 1].x(), temp_vertices[v1 - 1].y(), temp_vertices[v1 - 1].z());

			vertices.push_back(temp_vertices[v2 - 1]);
			//printf("v %d %f %f %f\n", v2, temp_vertices[v2 - 1].x(), temp_vertices[v2 - 1].y(), temp_vertices[v2 - 1].z());

			vertices.push_back(temp_vertices[v3 - 1]);
			//printf("v %d %f %f %f\n", v3, temp_vertices[v3 - 1].x(), temp_vertices[v3 - 1].y(), temp_vertices[v3 - 1].z());
			


			textures.push_back(temp_textures[t1 - 1]);
			//printf("vt %d %f %f\n", t1, temp_textures[t1 - 1].x(), temp_textures[t1 - 1].y());

			textures.push_back(temp_textures[t2 - 1]);
			//printf("vt %d %f %f\n", t2, temp_textures[t2 - 1].x(), temp_textures[t2 - 1].y());

			textures.push_back(temp_textures[t3 - 1]);
			//printf("vt %d %f %f\n", t3, temp_textures[t3 - 1].x(), temp_textures[t3 - 1].y());
			


			  normals.push_back(temp_normals[n1 - 1]);
			//printf("vn %d %f %f %f\n", n1, temp_normals[n1 - 1].x(), temp_normals[n1 - 1].y(), temp_normals[n1 - 1].z());
			  
			  normals.push_back(temp_normals[n2 - 1]);
			//printf("vn %d %f %f %f\n", n2, temp_normals[n2 - 1].x(), temp_normals[n2 - 1].y(), temp_normals[n2 - 1].z());
			  
			normals.push_back(temp_normals[n3 - 1]);
			//printf("vn %d %f %f %f\n", n3, temp_normals[n3 - 1].x(), temp_normals[n3 - 1].y(), temp_normals[n3 - 1].z()); 
			  
			  //for (int i = 0; i < temp_textures.size(); i++)
			 // {
			//	    printf("vt %d %f %f\n", i, temp_textures[i].x(), temp_textures[i].y());
			 // }
			  
			
			Triangle t(vertices,textures, normals);
			tris.push_back(t);
		}

		// std::cout << "vertSize = " << vertexIndices.size() << "\n";
		return true;
	}
};

class Obj 
{
public:
	Mesh mesh;
	std::vector<vec3> texture_buffer;
	int texture_width, texture_height;

	void decodeOneStep(const char* filename)
	{
		std::vector<unsigned char> png;
		std::vector<unsigned char> image;
		unsigned width, height;
		lodepng::State state;

		unsigned error = lodepng::load_file(png, filename);
		if(!error) error = lodepng::decode(image, width, height, state, png);

		if(error) std::cout << "decode error " << error << ": " << lodepng_error_text(error) << std::endl;

		texture_buffer.reserve((int)width*(int)height);
		
		for(int i = 0; i < image.size(); i+= 4){
			texture_buffer.push_back( vec3( float(image[i]), float(image[i+1]), float(image[i+2]) ) );
		}

		texture_width  = (int) width;
		texture_height = (int) height;

	}

	Obj(){}
	Obj( const char* file_path ){
		mesh.load_mesh_from_file(file_path); 
	}
	
	Obj( const char* file_path, const char* texture_path ){
		mesh.load_mesh_from_file(file_path); 
		decodeOneStep(texture_path);
	}

	~Obj(){}
};