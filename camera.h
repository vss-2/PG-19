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

const int WIDTH = 600;
const int HEIGHT = 400;

class camera
{
public:
    int imgWidth, imgHeight;
    int windowWidth, windowHeight;
    float fov, _far, _near;
    float bottom, left, top, right, aspectRatio;
    matrix44 camToWorld;
    matrix44 worldToCamera;

	vec3 _from, _at, _up;
    vec3 axisX, axisY, axisZ;

public:
    camera();
    camera(const vec3 &from, const vec3 &at, const vec3 &up,
           const float &f, const float &n, const float &a,
           const int &iwidth, const int &iheight): 
           fov(f), _near(n), _far(a), imgWidth(iwidth), imgHeight(iheight),
		   _from(from), _at(at), _up(up)
           {
                float radfov = fov*(3.141592f/180.f);
                top = std::tan(radfov/2.f);
                bottom = -top;
                aspectRatio = windowWidth/windowHeight;
                right = std::tan(radfov/2.f)*aspectRatio;
                left = -right;
                
                look_at(from, at, up);
           }

    void look_at(const vec3 &from, const vec3 &at, const vec3 &up)
    {
        axisZ = from - at;
        axisZ.make_unit_vector();
        axisY = up - (dot(up, axisZ)/ dot(axisZ, axisZ))*axisZ;
        axisY.make_unit_vector();
        axisX = cross(axisY, axisZ);
        axisX.make_unit_vector();
        
        camToWorld = matrix44(
            axisX.x(), axisX.y(), axisX.z(), 0.0,
            axisY.x(), axisY.y(), axisY.z(), 0.0,
            axisZ.x(), axisZ.y(), axisZ.z(), 0.0,
             from.x(),  from.y(),  from.z(), 1.0
        );
        worldToCamera = camToWorld.inverse();
    }

    bool compute_pixel_coordinates(const vec3 &pWorld, vec2 &praster) 
    { 
        vec3 algo, algo2;

        matrix44 multi = matrix44(
            2*_near/(right - left), 0.0      , 0.0      , 0.0,
            0.0      , 2*_near/(bottom - top), 0.0      , 0.0,
            -(right + left)/(right - left), -(bottom + top)/(bottom - top), (_far + _near)/(_far - _near), 1.0,
            0.0     ,     0.0, -(2*_near)/(_far - _near), 0.0
        );

        worldToCamera.mult_point_matrix(pWorld,algo);
        vec3 mProjecao = vec3(
            algo.x()*(_near/algo.z()),
            algo.y()*(_near/algo.z()),
            _near
        );

        multi.mult_point_matrix(mProjecao, algo2);
        
        praster = vec2((1+algo2.x())/2*imgWidth, (1-algo2.y())/2*imgHeight);

        if((bottom <= algo.y() <= top) && (left <= algo.x() <= right))
        {
            return true;
        } else {
            return false; 
        }

        // Retornar verdadeiro se o ponto pode ser visto
    }


    void render_scene( std::vector<Obj> objs, SDL_Renderer* renderer) {

        vec3 light(0.0f, 0.0f, -1.0f);
        light.make_unit_vector();

        for (auto obj : objs){
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

                if(v1 && v2)
                    SDL_RenderDrawLine(renderer, praster1.x(), praster1.y(), praster2.x(), praster2.y());
                if(v1 && v3)
                    SDL_RenderDrawLine(renderer, praster1.x(), praster1.y(), praster3.x(), praster3.y());
                if(v2 && v3)
                    SDL_RenderDrawLine(renderer, praster2.x(), praster2.y(), praster3.x(), praster3.y());
            }
        }
    }
};


#endif
