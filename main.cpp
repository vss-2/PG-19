#include <string>
#include <math.h>
#include "camera.h" 

#include "ImGUI/imgui_sdl.h"
#include "ImGUI/imgui.h"



int main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {

		SDL_Window* window = SDL_CreateWindow("Projeto PG - Grupo 3", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
		SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

		if (window && renderer) {
			int PosX = 0;
			int PosY = 0;
			int PosZ = 4;
			SDL_bool done = SDL_FALSE;
			SDL_SetRelativeMouseMode(SDL_FALSE);

			std::vector<Obj> objects;
			objects.push_back(Obj("./monkey_smooth.obj"));

			ImGui::CreateContext();
			ImGuiSDL::Initialize(renderer, WIDTH, HEIGHT);

			camera cam(vec3(PosX, PosY, PosZ), vec3(0, 0, -1), vec3(0, 1, 0), 90.0f, 1.f, WIDTH, HEIGHT, 1000);

			float my_color[4];
			bool my_tool_active;

			while (!done) {
				SDL_Event event;
				ImGuiIO& io = ImGui::GetIO();
				std::clock_t then = std::clock();

				int mouseX, mouseY;
				const int buttons = SDL_GetMouseState(&mouseX, &mouseY);

				io.DeltaTime = 1.0f / 60.0f;
				io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
				io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
				io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);

				ImGui::NewFrame();

				// Create a window called "My First Tool", with a menu bar.
				ImGui::Begin("My First Tool", &my_tool_active, ImGuiWindowFlags_MenuBar);
				if (ImGui::BeginMenuBar())
				{
					if (ImGui::BeginMenu("File"))
					{
						if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
						if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
						if (ImGui::MenuItem("Close", "Ctrl+W")) { my_tool_active = false; }
						ImGui::EndMenu();
					}
					ImGui::EndMenuBar();
				}

				// Edit a color (stored as ~4 floats)
				ImGui::ColorEdit4("Color", my_color);
				// Plot some values
				const float my_values[] = { 0.2f, 0.1f, 1.0f, 0.5f, 0.9f, 2.2f };
				ImGui::PlotLines("Frame Times", my_values, IM_ARRAYSIZE(my_values));

				// Display contents in a scrolling region
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "Pojeto di PeGue");
				ImGui::BeginChild("Scrolling");
				ImGui::Text("Vo mata o Krushtu\n");
				ImGui::EndChild();
				ImGui::End();

				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
				SDL_RenderClear(renderer); // clear previous frame generated image

				cam.render_scene(objects, renderer); // feed triangle data onto renderer

				ImGui::Render();
				ImGuiSDL::Render(ImGui::GetDrawData());
				SDL_RenderPresent(renderer); // present the generated triangle data onto screen

				
				while (SDL_PollEvent(&event)) {

					if (event.type == SDL_KEYDOWN) {
						if (event.key.keysym.sym == SDLK_a) {
							PosX++;
							printf("Just pressed keyboard key A!\n");
							cam.look_at(vec3(PosX, PosY, PosZ), vec3(0, 0, -1), vec3(0, 1, 0));
						}
					}
					if (event.type == SDL_KEYDOWN) {
						if (event.key.keysym.sym == SDLK_w) {
							PosZ--;
							printf("Just pressed keyboard key W!\n");
							cam.look_at(vec3(PosX, PosY, PosZ), vec3(0, 0, -1), vec3(0, 1, 0));
						}
					}
					if (event.type == SDL_KEYDOWN) {
						if (event.key.keysym.sym == SDLK_s) {
							PosZ++;
							printf("Just pressed keyboard key S!\n");
							cam.look_at(vec3(PosX, PosY, PosZ), vec3(0, 0, -1), vec3(0, 1, 0));
						}
					}
					if (event.type == SDL_KEYDOWN) {
						if (event.key.keysym.sym == SDLK_d) {
							PosX--;
							printf("Just pressed keyboard key D!\n");
							cam.look_at(vec3(PosX, PosY, PosZ), vec3(0, 0, -1), vec3(0, 1, 0));
						}
					}


					// mouse pressed event
					if (event.type == SDL_MOUSEBUTTONDOWN)
					{
						//If the left mouse button was pressed
						if (event.button.button == SDL_BUTTON_RIGHT)
						{
						}
						if (event.button.button == SDL_BUTTON_LEFT)
						{
							//Relative mouse mode moves mouse to center of window
							//every frame. Useful if you want to use mouse and not worry
							//about it going off screen
							int mousePosX, mousePosY;
							SDL_GetMouseState(&mousePosX, &mousePosY);
							float Px = mousePosX/1.0;
							float Py = mousePosY/1.0;
							//SDL_SetRelativeMouseMode(SDL_TRUE);
							float Px = -(2 * (Px/imgWidth) - 1) * cam.right/cam._near;
							float Py = (1 - 2*(Py/imgHeight)) * cam.top/cam._near;
							vec3 RayDirection = vec3(Px, Py, -1);
							
							vec3 RDWorld;

							cam.worldToCamera.mult_point_matrix(RayDirection, RDWorld);
							
							RDWorld.make_unit_vector();

							for (auto object : objs) 
							{ // Se não rodar com object, trocar por obj
								for (int i = 0; i < object.mesh.tris.size(); i++)
								{
									vec3 p1 = obj.mesh.tris[i].vertex[0].pos;
									vec3 p2 = obj.mesh.tris[i].vertex[1].pos;
									vec3 p3 = obj.mesh.tris[i].vertex[2].pos;

									vec3 normTriag = (cross(p2-p1, p3-p1));

									// Tomar CUIDADO com a ordem:
									// a arbitrária é A 1,2 - B 1,3 - C 2,3
									// Fazer p2-p1, p3-p1

									float D = dot(normTriag, p1);
									// D é o D do plano

									// É o t do slide, que é a distância até o ponto:
									float t = -(dot(normTriag, cam._from) - D)/dot(normTriag, RDWorld);

									if(dot(normTriag, RDWorld) >= 0.00001 && t >= 0)
									{
										vec3 pInter;
										pInter[0] = cam._from[0] + (t*RDWorld[0]);
										pInter[1] = cam._from[1] + (t*RDWorld[1]);
										pInter[2] = cam._from[2] + (t*RDWorld[2]);	
									}

									// Ver se os produtos vetorias com cross

								}
							}
						}
					}
					if (event.type == SDL_MOUSEBUTTONUP)
					{
						//If the left mouse button was released
						if (event.button.button == SDL_BUTTON_LEFT)
						{
							SDL_SetRelativeMouseMode(SDL_FALSE);
						}
					}

					// mouse move event
					if (event.type == SDL_MOUSEMOTION)
					{
						//get relative mouse motion, i.e mouse delta x,y from last frame
						float x = event.motion.xrel;
						float y = event.motion.yrel;

					}

					if (event.type == SDL_QUIT)
						done = SDL_TRUE;

				}
			}
		}

		if (renderer) {
			SDL_DestroyRenderer(renderer);
		}
		if (window) {
			SDL_DestroyWindow(window);
		}
	}
	SDL_Quit();
	return 0;
}
