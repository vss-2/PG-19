#ifndef CAMERAH
#define CAMERAH

#include "vec3.h"
#include "vec2.h"
#include "matrix44.h"
#include "object.h"

#ifdef _WIN32 || WIN32
	#include <SDL.h>
#elif defined(__unix__)
	#include <SDL2/SDL.h>
#endif

const int WIDTH = 800;
const int HEIGHT = 600;

class camera
{
public:
	int imgWidth, imgHeight;
	float fov, _near, _far;
	float bottom, left, top, right, aspectRatio;
	matrix44 camToWorld;
	matrix44 worldToCamera;

	vec3 _from, _at, _up;
	vec3 axisX, axisY, axisZ;

public:
	camera();
	camera(const vec3& from, const vec3& at, const vec3& up,
		const float& f, const float& n,
		const int& windowWidth, const int& windowHeight, const float& far) :
		fov(f), _near(n), imgWidth(windowWidth), imgHeight(windowHeight),
		_from(from), _at(at), _up(up), _far(far)
	{
		float radfov = fov * (3.141592f / 180.f);
		top = tan(radfov / 2.f);
		bottom = -top;
		aspectRatio = windowWidth / windowHeight;
		right = tan(radfov / 2.f) * aspectRatio;
		left = -right;

		look_at(from, at, up);
	}


	void look_at(const vec3& from, const vec3& at, const vec3& up)
	{
		axisZ = from - at;
		axisZ.make_unit_vector();
		axisY = up - (dot(up, axisZ) / dot(axisZ, axisZ)) * axisZ;
		axisY.make_unit_vector();
		axisX = cross(axisY, axisZ);
		axisX.make_unit_vector();

		camToWorld = matrix44(
			axisX.x(), axisX.y(), axisX.z(), 0.0,
			axisY.x(), axisY.y(), axisY.z(), 0.0,
			axisZ.x(), axisZ.y(), axisZ.z(), 0.0,
			from.x(), from.y(), from.z(), 1.0
		);
		worldToCamera = camToWorld.inverse();
	}

	bool compute_pixel_coordinates(const vec3& pWorld, vec2& praster)
	{
		vec3 algo, algo2;

		matrix44 multi = matrix44(
			2 * _near / (right - left), 0.0, 0.0, 0.0,
			0.0, 2 * _near / (bottom - top), 0.0, 0.0,
			-(right + left) / (right - left), -(bottom + top) / (bottom - top), (_far + _near) / (_far - _near), 1.0,
			0.0, 0.0, -(2 * _near) / (_far - _near), 0.0
		);

		worldToCamera.mult_point_matrix(pWorld, algo);

		vec3 mProjecao = vec3(
			algo.x() * (_near / algo.z()),
			algo.y() * (_near / algo.z()),
			_near
		);


		multi.mult_point_matrix(mProjecao, algo2);

		praster = vec2((1 + algo2.x()) / 2 * imgWidth, (1 - algo2.y()) / 2 * imgHeight);

		if ((bottom <= algo.y() && algo.y() <= top) && (left <= algo.x() && algo.x() <= right)) 
		{
			return true;
		}
		else 
		{
			return false;
			//calcula a cos(de algo), se der positivo, da false
		}
	}

	void desenharLinha(SDL_Renderer *renderer, vec2 &p0, vec2 &p1)
	{
		vec2 start = p1;
        vec2 diretor = p0-p1;
		int fInt = (int) diretor.length();
		diretor.make_unit_vector();

		for(int i = 0; i <= fInt; i++)
		{
			SDL_RenderDrawPoint(renderer, (int) start.x(), (int) start.y());
			start += diretor;
		}
	}

	int getOutcode(vec2 p, int xMin, int xMax, int yMin, int yMax){
		int bits   = 0;
		int inside = 0;
		int left   = 1;
		int right  = 2;
		int bottom = 4;
		int top    = 8;
		
		if(p.y() > yMax)
		{
			bits = top    | inside;
		}

		if(p.y() < yMin)
		{
			bits = bottom | inside;
		}

		if(p.x() > xMax)
		{
			bits = right  | inside;
		}

		if(p.x() < xMin)
		{
			bits = left   | inside;
		}

		if(p.y() > yMax && p.x() > xMax)
		{	//Top-Right
			bits = top    | right;
		}

		if(p.y() < yMin && p.x() < xMin)
		{	//Bottom-Left
			bits = bottom | left;
		}

		if(p.x() > xMax && p.y() < yMin)
		{	//Bottom-Right
			bits = right  | right;
		}

		if(p.x() < xMin && p.y() > yMax)
		{	//Top-Left
			bits = left   | top;
		}

		// if((xMin < p.x() < xMax) and (yMin < p.y() < yMax))
		// {	// Inside
		// 	bits = inside;
		// }

		if(bits != 0)
			printf("O bits retornou: %i\n", bits);

		return bits;
	}

	bool ClipLine(vec2 &bkprasterA, vec2 &bkprasterB, int xMin, int xMax, int yMin, int yMax)
	{
		int outcode0 = getOutcode(bkprasterA, xMin, xMax, yMin, yMax);
		int outcode1 = getOutcode(bkprasterB, xMin, xMax, yMin, yMax);
		
		float slope, novoX, novoY = 0;

		bool accept = false;

		while(true){
			if(outcode0 == 0 & outcode1 == 0)
			{
				accept = true;
				break;
			} 
			else if (outcode0 & outcode1)
			{
				break;
			} else {
				// Pelo menos um ponto está fora da janela

				int outcodeOutside = outcode1 != 0? outcode1 : outcode0;
				// printf("O outcodeOutside é: %i\n", outcodeOutside);

				// Calcula x e y para interseção com top, bottom, right e left
				if (outcodeOutside & 8)
				{
                    slope = (bkprasterB.y() - bkprasterA.y())/(bkprasterB.x() - bkprasterA.x());
					novoX = bkprasterA.x() + (1.0f/slope)*(yMax - bkprasterA.y());
					novoY = yMax;
					printf("Entrei no TOP\n");
				} else if (outcodeOutside & 4)
				{
                    slope = (bkprasterB.y() - bkprasterA.y())/(bkprasterB.x() - bkprasterA.x());
					novoX = bkprasterA.x() + (1.0f/slope)*(yMin - bkprasterA.y());
					novoY = yMin;
					printf("Entrei no BOT\n");
				} else if (outcodeOutside & 2)
				{
                    slope = (bkprasterB.y() - bkprasterA.y())/(bkprasterB.x() - bkprasterA.x());
					novoY = bkprasterA.y() + slope*(xMax - bkprasterA.x());
					novoX = xMax; 
					printf("Entrei no RGT\n");
				} else if (outcodeOutside & 1)
				{
                    slope = (bkprasterB.y() - bkprasterA.y())/(bkprasterB.x() - bkprasterA.x());
					novoY = bkprasterA.y() + slope*(xMin - bkprasterA.x());
					novoX = xMin;
					printf("Entrei no LFT\n");
				}
                    
				//printf("Slope: %f, novoX: %f, novoY: %f", slope, novoX, novoY);
				printf("OutcodeOutside:%i\nOutcode0:%i\nOutcode1:%i\n", outcodeOutside, outcode0, outcode1);
				if (outcodeOutside == outcode0)
				{
					printf("Outcode0 é igual\n");
                    bkprasterA[0] = novoX;
                    bkprasterA[1] = novoY;
					outcode0 = getOutcode(bkprasterA, xMin, xMax, yMin, yMax);
				} else {
					printf("Outcode 0 é diferente\n");
                    bkprasterB[0] = novoX;
                    bkprasterB[1] = novoY;
                    //printf("Slope: %f, novoX: %f, novoY: %f",slope ,novoX, novoY);
					outcode1 = getOutcode(bkprasterB, xMin, xMax, yMin, yMax);
					printf("Atualizei outcode1: %i\n", outcode1);
				}
			}
		}
		return accept;
	}

	void render_scene(std::vector<Obj> objs, SDL_Renderer* renderer){
		int PosX = 0;
		int PosY = 0;
		int PosZ = 4;
		vec3 light(0.0f, 0.0f, -1.0f);
		light.make_unit_vector();
		int aa = 0;

		for (auto obj : objs) 
		{
			for (int i = 0; i < obj.mesh.tris.size(); i++)
			{
				vec2 praster1, praster2, praster3;
				vec2 bkprasterA, bkprasterB;

				vec3 col(255, 255, 255);
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

				bool v1, v2, v3;
				v1 = compute_pixel_coordinates(obj.mesh.tris[i].vertex[0].pos, praster1);
				v2 = compute_pixel_coordinates(obj.mesh.tris[i].vertex[1].pos, praster2);
				v3 = compute_pixel_coordinates(obj.mesh.tris[i].vertex[2].pos, praster3);

				if (v1 && v2)
                {
					bkprasterA = praster1;
					bkprasterB = praster2;
                    if(ClipLine(bkprasterA, bkprasterB, 0, WIDTH, 0, HEIGHT))
                    {
                    	desenharLinha(renderer, bkprasterA, bkprasterB);
                    }
                }

				if (v1 && v3)
                {
					bkprasterA = praster1;
					bkprasterB = praster3;
                    if(ClipLine(bkprasterA, bkprasterB, 0, WIDTH, 0, HEIGHT))
                    {
                    	desenharLinha(renderer, bkprasterA, bkprasterB);
                    }
                }

				if (v2 && v3)
                {
					bkprasterA = praster2;
					bkprasterB = praster3;
                    if(ClipLine(bkprasterA, bkprasterB, 0, WIDTH, 0, HEIGHT))
                    {
                    	desenharLinha(renderer, bkprasterA, bkprasterB);
                    }                    
                }
			}
		}
	}
};


#endif
