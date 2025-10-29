#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Obj3D.h"
#include "Camera.h"
using namespace std;

float RandomFloat(float min, float max);
void processInput(GLFWwindow* window);

//DIMENSÕES DA TELA
const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;

//VARIAVEIS PARA O "JOGO"
float cooldown;
float cooldownTime = 0.5f;
bool fire = false;
int points = 0;

//CRIAÇÃO DA CAMERA
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

//VETOR DE OBJETOS
vector<Obj3D*> objects;

//CLASSE DO TIRO
class Bullet {
public:
	glm::vec3 Front;
	float spawnTime;
	float lifespan = 1.0f;
	float speed = 10.0f;
	Obj3D* mesh = new Obj3D("ball.obj", (camera.Position + Front * 2.0f), glm::vec3(0.5f));

	Bullet()
	{
		Front = camera.Front;
		objects.push_back(mesh);
		spawnTime = glfwGetTime();
	}
};

void checkColl(Bullet* bull, Obj3D* objy);
void UpdateBull(Bullet* bullet);

float deltaTime = 0.0f;	
float lastFrame = 0.0f;

//VETOR DE TIROS
vector<Bullet*> bullets;

int main()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OBJ3D", NULL, NULL);
	if (!window) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(window);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glewExperimental = GL_TRUE;
	glewInit();

	std::cout << "" << std::endl;
	std::cout << "" << "OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "" << "OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "" << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "" << "OpenGL Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	std::cout << "" << std::endl;

	//CRIAÇÃO DO SHADER
	Shader shader("vShader.vs", "fShader.fs");
		
	shader.use();

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//CHÃO DO JOGO
	Obj3D* ground = new Obj3D("cubo1.obj", glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(10.0f, 0.25f, 10.0f));
	objects.push_back(ground);

	//CUBO BASE
	Obj3D* enemy = new Obj3D("cubo1.obj", glm::vec3(0.0f, 0.25f, 0.0f));

	int lastTime = 0;

	while (!glfwWindowShouldClose(window)) {

		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.1, 0.35, 0.5, 1.0);

		shader.use();

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		shader.setMat4("projection", projection);

		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("view", view);

		//DESENHO DOS OBJETOS
		if (objects.size() != 0)
		{
			for (size_t i = 0; i < objects.size(); i++)
			{
				shader.setMat4("model", objects[i]->transform);
				objects[i]->draw();
				if (bullets.size() != 0)
				{
					//CHECAR COLISSÃO DOS TIROS COM OS OBJETOS
					for (size_t j = 0; j < bullets.size(); j++) {
						if (objects[i] != bullets[j]->mesh)
						{
							checkColl(bullets[j], objects[i]);
						}
					}
				}
			}
			//ATUALIZAR OS TIROS
			for (Bullet* bull : bullets) {
				UpdateBull(bull);
			}
		}

		//RESETAR O COOLDOWN DOS TIROS
		if (glfwGetTime() >= cooldown + cooldownTime)
		{
				fire = false;
		}

		//CRIAÇÃO DOS CUBOS
		int time = glfwGetTime();
		if (time >= lastTime + 5)
		{	
			//cout << "5 second Timer" << endl;
			if (objects.size() > 1)
			{
				for (Obj3D* obj : objects) {
					if (obj->name == "Friend")
					{
						objects.erase(remove(objects.begin(), objects.end(), obj), objects.end());
						points += 1;
						cout << "Points: " << points << endl;
						cout << "Friend Saved" << endl;
					}
				}
			}
			enemy->move(glm::vec3(RandomFloat(-5.0f - enemy->Pos.x, 5.0f - enemy->Pos.x), 0.0f, RandomFloat(-5.0f - enemy->Pos.z, 5.0f - enemy->Pos.z)));
			objects.push_back(enemy);
			srand((unsigned)std::time(0));
			int quebravel = rand() % 2;
			if (quebravel == 0)
			{
				enemy->quebravel = true;
				enemy->name = "Enemy";
				cout << "New Enemy" << endl;
			}
			else
			{
				enemy->quebravel = false;
				enemy->name = "Friend";
				cout << "New Friend" << endl;
			}
				lastTime = time;
		}


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;

}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		camera.MovementSpeed = 7.5f;
	}
	else {
		camera.MovementSpeed = 2.5f;
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		if (!fire)
		{
			fire = true;
			cooldown = glfwGetTime();
			Bullet* newbull = new Bullet();
			bullets.push_back(newbull);
		}
	}

}

float RandomFloat(float min, float max)
{
	assert(max > min);
	float random = ((float)rand()) / (float)RAND_MAX;


	float range = max - min;
	return (random * range) + min;
}

void UpdateBull(Bullet* bullet)
{
	if (glfwGetTime() >= bullet->spawnTime + bullet->lifespan)
	{
		objects.erase(remove(objects.begin(), objects.end(), bullet->mesh), objects.end());
		bullets.erase(remove(bullets.begin(), bullets.end(), bullet), bullets.end());
	}
	float velocity = bullet->speed * deltaTime;
	bullet->mesh->move(bullet->Front * velocity);
}

void checkColl(Bullet* bull, Obj3D* objy) 
{
	bool collX = bull->mesh->max.x >= objy->min.x && objy->max.x >= bull->mesh->min.x;
	bool collY = bull->mesh->max.y >= objy->min.y && objy->max.y >= bull->mesh->min.y;
	bool collZ = bull->mesh->max.z >= objy->min.z && objy->max.z >= bull->mesh->min.z;
	if (collX && collY && collZ)
	{
		//cout << "collided" << endl;
		if (objy->quebravel)
		{
			objects.erase(remove(objects.begin(), objects.end(), objy), objects.end());
			points += 1;
			cout << "Points: " << points << endl;
		}
		else
		{
			bull->Front *= -1.0;
			objects.erase(remove(objects.begin(), objects.end(), objy), objects.end());
			points -= 1;
			cout << "Points: " << points << endl;
		}
	}
	
}