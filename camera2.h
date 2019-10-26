#ifndef CAMERAH
#define CAMERAH

#include "vec3.h"
#include "vec2.h"
#include "matrix44.h"
#include "object.h"
#include <math.h>

#ifdef _WIN32 || WIN32
	#include <SDL.h>
#elif defined(__unix__)
	#include <SDL2/SDL.h>
#endif

const int WIDTH = 800;
const int HEIGHT = 600;


vec2 arestasW[1000000];
int contadorDeW = 0;

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
		aspectRatio = (float)(windowWidth) / windowHeight;
		right = tan(radfov / 2.f) * aspectRatio;
		left = -right;

		look_at(from, at, up);
	}


	void look_at(const vec3& from, const vec3& at, const vec3& up)
	{
        _from = from;
        _at = at;
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

		vec3 v1 = pWorld - _from;

		vec3 v2 = _at - _from;

		v1.make_unit_vector();

		v2.make_unit_vector();

		float c = ((v1.x()*v2.x())+(v1.y()*v2.y())+(v1.z()*v2.z()))/(sqrt(pow(v1.x(),2)+pow(v1.y(),2)+pow(v1.z(),2)) * sqrt(pow(v2.x(),2)+pow(v2.y(),2)+pow(v2.z(),2)));



		if(c > 0)
		{
		return true;
		}
		return false;
	}
					
	void desenharLinha(SDL_Renderer *vemDoMain, vec2 &ponto1, vec2 &ponto2){
		vec2 diretor = ponto1 - ponto2;
		int fInt = (int) diretor.length();
		diretor.make_unit_vector();
		
		vec2 aux = ponto2;
		for(int iter = 0; iter <= fInt; iter++){
			SDL_RenderDrawPoint(vemDoMain, aux.x(), aux.y());
			aux += diretor;
		}
		
		
		
	}


	// bool edgefunction(int x, int y, const vec2 &v0, const vec2 &v1, const vec2 &v2)
	// {
	// 	float Wa = v0.x() * v1.y() - v0.y() - v1.x();
	// 	float Wb = v1.x() * v2.y() - v1.y() - v2.x();
	// 	float Wc = v2.x() * v0.y() - v2.y() - v0.x();

	// 	if(Wa >= 0 && Wb >= 0 && Wc >= 0)
	// 	{
	// 		// Se der bug, lembrar de ser conciso:
	// 		// Usaremos os “mesmos” vetores/arestas que 
	// 		// antes como input da função(ex: V0-V1 e V0-P), 
	// 		// como está ilustrado na figura.
	// 		// arestasW[contadorDeW] = v0;
	// 		// contadorDeW++; 
	// 		// arestasW[contadorDeW] = v1;
	// 		// contadorDeW++;
	// 		// arestasW[contadorDeW] = v2;
	// 		// contadorDeW++;
	// 		return true;
	// 	}
	// 	return false;
	// }

	vec3 PhongShading(vec3 normal, vec3 light_dir, float Ia, vec3 Ka, vec3 Kd, float Ks, int n, vec3 l, float r, vec3 normCam)
	{
		vec3 I = Ia * Ka + (Ia*(Kd * dot(n,l) + Ks * pow(dot(r,normCam), n)));
		return I;
	}

	bool edgeFunction(vec2 &P, const vec2 &v0, const vec2 &v1, const vec2 &v2){ 

		vec3 p1(v0.x(), v0.y(), 0);
		vec3 p2(v1.x(), v1.y(), 0);
		vec3 p3(v2.x(), v2.y(), 0);
		vec3 pNovo(P.x(), P.y(), 0);

		vec3 normTriag(0,0,-1);  

		// printf("X: %f Y: %f Z: %f\n", normTriag.x(), normTriag.y(), normTriag.z());
		
		vec3 cOut1 = -cross(pNovo - p1, p2 - p1);
		vec3 cOut2 = -cross(pNovo - p2, p3 - p2);
		vec3 cOut3 = -cross(pNovo - p3, p1 - p3);

		if((dot(cOut1, normTriag) > 0 && dot(cOut2, normTriag) > 0) && dot(cOut3, normTriag) > 0)
		{
			// printf("true\n");
			return true;
		}
		// printf("false\n");
		return false;
	}

	void pintarTriangulo(SDL_Renderer *renderer, const vec2 &v0, const vec2 &v1, const vec2 &v2, const Triangle &tr, const vec3 &light_dir)
	{
		float xMin = std::min(std::min(v0.x(), v2.x()), v1.x());
		float xMax = std::max(std::max(v0.x(), v2.x()), v1.x());
		float yMin = std::min(std::min(v0.y(), v2.y()), v1.y());
		float yMax = std::max(std::max(v0.y(), v2.y()), v1.y());

		// Edge Function real começa aqui:

		for(int x = xMin; x <= xMax; x++)
		{
			for (int y = yMin; y <= yMax; y++)
			{
				vec2 parametro (x,y);
				// printf("Ponto: %d %d\n", x, y);

				// Se estiver no triangulo, pinte-o
				if(edgeFunction(parametro, v0, v1, v2))
				{
					vec3 normTriag = (cross(v1-v0, v2-v0));  
					vec3 color = PhongShading(, light_dir, Ia, Ka, Kd, Ks, n, l, r, normCam);
					SDL_SetRenderDrawColor(renderer, color.r(), color.g(), color.b(), 255);
					SDL_RenderDrawPoint(renderer, x, y);
				}

				// Estamos passando o ponto e um triangulo
			}
		}
		// Edge Function acaba aqui.
	}

	int getOutcode(vec2 p, int xMin, int xMax, int yMin, int yMax){
		int inside = 0;
		int left   = 1;
		int right  = 2;
		int bottom = 4;
		int top    = 8;
		
		if(p.y() > yMax){
			inside |= top;
		}
		if(p.y() < yMin){
			inside |= bottom;
		}
		if(p.x() > xMax){
			inside |= right;
		}
		if(p.x() < xMin){
			inside |= left;
		}
		return inside;
	}

	bool ClipLine(vec2 &p0, vec2 &p1, int xMin, int xMax, int yMin, int yMax){
		int outcode0 = getOutcode(p0, xMin, xMax, yMin, yMax);
		int outcode1 = getOutcode(p1, xMin, xMax, yMin, yMax);
		
		bool accept = false;

		float slope = (p1.y() - p0.y())/(p1.x() - p0.x());
		float novoX, novoY = 0;


		while(true){
			if(outcode0 == 0 && outcode1 == 0){
				accept = true;
				break;
			} else if (outcode0 & outcode1){
				break;
			} else {
				int outcodeOutside = outcode1 != 0? outcode1 : outcode0;
				if (outcodeOutside & 8){
					novoX = p0.x() + (1.0/slope)*((float)yMax - p0.y());
					novoY = (float)yMax;
				} else if (outcodeOutside & 4){
					novoX = p0.x() + (1.0/slope)*((float)yMin - p0.y());
					novoY = (float)yMin;
				} else if (outcodeOutside & 2){
					novoX = (float)xMax;
					novoY = p0.y() + slope*((float)xMax - p0.x());
				} else if (outcodeOutside & 1){
					novoX = (float)xMin;
					novoY = p0.y() + slope*(xMin - p0.x());
				}
				if (outcodeOutside == outcode0){
					p0 = vec2(novoX, novoY);
					outcode0 = getOutcode(p0, xMin, xMax, yMin, yMax);
				} else {
					p1 = vec2(novoX, novoY);
					outcode1 = getOutcode(p1, xMin, xMax, yMin, yMax);
				}
			}
		}
		return accept;
	}


	void render_scene(std::vector<Obj> objs, SDL_Renderer* renderer) {

		vec3 light(0.0f, 0.0f, -1.0f);
		light.make_unit_vector();

		for (auto obj : objs) 
		{
			for (int i = 0; i < obj.mesh.tris.size(); i++)
			{
				vec2 praster1;
				vec2 praster2;
				vec2 praster3;
				vec3 col(255, 255, 255);
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

				bool v1, v2, v3;
				v1 = compute_pixel_coordinates(obj.mesh.tris[i].vertex[0].pos, praster1);
				v2 = compute_pixel_coordinates(obj.mesh.tris[i].vertex[1].pos, praster2);
				v3 = compute_pixel_coordinates(obj.mesh.tris[i].vertex[2].pos, praster3);				


				if (v1 && v2){
					vec2 aux_praster1 = praster1;
					vec2 aux_praster2 = praster2;

					if(ClipLine(aux_praster1, aux_praster2, 0, WIDTH, 0, HEIGHT)){
						desenharLinha(renderer, aux_praster1, aux_praster2);

					}
				}
				if (v1 && v3){
					vec2 aux_praster1 = praster1;
					vec2 aux_praster3 = praster3;

					if(ClipLine(aux_praster1, aux_praster3, 0, WIDTH, 0, HEIGHT)){
						desenharLinha(renderer, aux_praster1, aux_praster3);
					}
				}
				if (v2 && v3){
					vec2 aux_praster2 = praster2;
					vec2 aux_praster3 = praster3;

					if(ClipLine(aux_praster2, aux_praster3, 0, WIDTH, 0, HEIGHT)){
						desenharLinha(renderer, aux_praster2, aux_praster3);
					}
				}

				 vec3 vt1(praster1.x(), praster1.y(), 0);
				 vec3 vt2(praster2.x(), praster2.y(), 0);
				 vec3 vt3(praster3.x(), praster3.y(), 0);

				 Triangle Triag(Vertex vt1, Vertex vt2, Vertex vt3);

				// Triangle T( vec3(praster1.x(), praster1.y(), 0.0), vec3(praster2.x(), praster2.y(), 0), vec3(praster3.x(), praster3.y(), 0) );
		
				pintarTriangulo(renderer, praster1, praster2, praster3, Triag, light);
			}
		}
	}
};


#endif
