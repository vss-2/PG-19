vec3 arestasWa[1000000];
int contadorDeWa = 0;

void pintarTriangulo(SDL_Renderer *renderer, const vec2 &v0, const vec2 &v1, const vec2 *v2)
{

}

bool edgefunction(Triangle triangle, vec3 &P)
{
	vec3 p0 = triangle.vertex[0].pos;
	vec3 p1 = triangle.vertex[1].pos;

	float Wa = p0.x() * p1.y() - p0.y() - p1.x();

	if(Wa >= 0)
	{
		// Se der bug, lembrar de ser conciso:
		// Usaremos os “mesmos” vetores/arestas que 
		// antes como input da função(ex: V0-V1 e V0-P), 
		// como está ilustrado na figura.
		arestasWa[contadorDeWa] = p0;
		contadorDeWa++; 
		arestasWa[contadorDeWa] = p1;
		contadorDeWa++;
	}

}
