#define STB_IMAGE_IMPLEMENTATION

#include "glad/glad.h"
#include <GLFW/glfw3.h>

//#include "stb/stb_image.h"

#include "source_dir.h"

#include "api/Shader.h"
#include "api/Camera.h"
#include "api/Model.h"
#include "api/Mesh.h"

#include <iostream>
#include <string>
#include <filesystem>
#include <random>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
unsigned int loadCubemap(vector<std::string> faces);
void initCoins();
void checkCoinCollisions();
void checkPerimeterCollisions();
void drawPerimeter(Shader& shader, float height);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f, 8.0f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// coins
std::vector<glm::vec3> coinPositions;
int coinsCollected = 0;

// game zone
const float gameZoneSize = 200.0f;

// Shader Paths
const std::filesystem::path cubeVertexPath = std::filesystem::path(SRC_DIR) / std::filesystem::path("resources/shaders/cube.vs");
const std::filesystem::path cubeFragmentPath = std::filesystem::path(SRC_DIR) / std::filesystem::path("resources/shaders/cube.fs");
const std::filesystem::path playerVertexPath = std::filesystem::path(SRC_DIR) / std::filesystem::path("resources/shaders/player.vs");
const std::filesystem::path playerFragmentPath = std::filesystem::path(SRC_DIR) / std::filesystem::path("resources/shaders/player.fs");
const std::filesystem::path skyboxVertexPath = std::filesystem::path(SRC_DIR) / std::filesystem::path("resources/shaders/skybox.vs");
const std::filesystem::path skyboxFragmentPath = std::filesystem::path(SRC_DIR) / std::filesystem::path("resources/shaders/skybox.fs");
const std::filesystem::path floorVertexPath = std::filesystem::path(SRC_DIR) / std::filesystem::path("resources/shaders/floor.vs");
const std::filesystem::path floorFragmentPath = std::filesystem::path(SRC_DIR) / std::filesystem::path("resources/shaders/floor.fs");
const std::filesystem::path coinVertexPath = std::filesystem::path(SRC_DIR) / std::filesystem::path("resources/shaders/coin.vs");
const std::filesystem::path coinFragmentPath = std::filesystem::path(SRC_DIR) / std::filesystem::path("resources/shaders/coin.fs");
const std::filesystem::path perimeterVertexPath = std::filesystem::path(SRC_DIR) / std::filesystem::path("resources/shaders/perimeter.vs");
const std::filesystem::path perimeterFragmentPath = std::filesystem::path(SRC_DIR) / std::filesystem::path("resources/shaders/perimeter.fs");

// Skybox Paths
// Load skybox textures into a cube map
std::vector<std::string> faces{
  std::filesystem::path(SRC_DIR) / std::filesystem::path("resources/textures/skybox/sunny_2.png"),
  std::filesystem::path(SRC_DIR) / std::filesystem::path("resources/textures/skybox/sunny_0.png"),
  std::filesystem::path(SRC_DIR) / std::filesystem::path("resources/textures/skybox/sunny_4.png"),
  std::filesystem::path(SRC_DIR) / std::filesystem::path("resources/textures/skybox/sunny_5.png"),
  std::filesystem::path(SRC_DIR) / std::filesystem::path("resources/textures/skybox/sunny_1.png"),
  std::filesystem::path(SRC_DIR) / std::filesystem::path("resources/textures/skybox/sunny_3.png")
};

// Texture Paths
const std::filesystem::path mogTexturePath = std::filesystem::path(SRC_DIR) / std::filesystem::path("resources/textures/mog.png");
const std::filesystem::path floorTexturePath = std::filesystem::path(SRC_DIR) / std::filesystem::path("resources/textures/wildgrass.png");

// Model Paths
const std::filesystem::path playerModelPath = std::filesystem::path(SRC_DIR) / std::filesystem::path("resources/models/man.obj");
const std::filesystem::path coinModelPath = std::filesystem::path(SRC_DIR) / std::filesystem::path("resources/models/coin.obj");


int main()
{
  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  // glfw window creation
  // --------------------
  GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  // tell GLFW to capture our mouse
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
  stbi_set_flip_vertically_on_load(true);

  // configure global opengl state
  // -----------------------------
  glEnable(GL_DEPTH_TEST);

  // build and compile shaders
  // -------------------------
//  Shader playerShader(playerVertexPath.string().c_str(), playerFragmentPath.string().c_str());
  Shader coinShader(coinVertexPath.string().c_str(), coinFragmentPath.string().c_str());
  Shader skyboxShader(skyboxVertexPath.string().c_str(), skyboxFragmentPath.string().c_str());
  Shader floorShader(floorVertexPath.string().c_str(), floorFragmentPath.string().c_str());
  Shader perimeterShader(perimeterVertexPath.string().c_str(), perimeterFragmentPath.string().c_str());

  float skyboxVertices[] = {
    // positions
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f,  1.0f
  };

  float floorVertices[] = {
    // positions          // texture Coords
    500.0f,  0.0f,  500.0f,  25.0f, 0.0f,
    -500.0f,  0.0f,  500.0f,  0.0f, 0.0f,
    -500.0f,  0.0f, -500.0f,  0.0f, 25.0f,

    500.0f,  0.0f,  500.0f,  25.0f, 0.0f,
    -500.0f,  0.0f, -500.0f,  0.0f, 25.0f,
    500.0f,  0.0f, -500.0f,  25.0f, 25.0f
  };

  unsigned int floorTexture = loadTexture(floorTexturePath.string().c_str());

  // skybox VAO
  unsigned int skyboxVAO, skyboxVBO;
  glGenVertexArrays(1, &skyboxVAO);
  glGenBuffers(1, &skyboxVBO);
  glBindVertexArray(skyboxVAO);
  glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

  // floor VAO
  unsigned int floorVAO, floorVBO;
  glGenVertexArrays(1, &floorVAO);
  glGenBuffers(1, &floorVBO);
  glBindVertexArray(floorVAO);
  glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), &floorVertices, GL_STATIC_DRAW);

  // Position attribute
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

  // Texture attribute
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

  // Bind floor texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, floorTexture);

  // load textures
  unsigned int cubemapTexture = loadCubemap(faces);


  skyboxShader.use();
  skyboxShader.setInt("skybox", 0);

  floorShader.use();
  floorShader.setInt("floor", 1);

  coinShader.use();
  coinShader.setInt("coin", 2);

  // load models
  // -----------
//  Model playerModel(playerModelPath.string().c_str());
  Model coinModel(coinModelPath.string());


  // draw in wireframe
//  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glm::vec3 modelPosition = glm::vec3(0.0f, 5.0f, 0.0f); // Initial model position

  // Initialize game objects and load resources
  initCoins(); // Initialize coins in random positions

  // render loop
  // -----------
  while (!glfwWindowShouldClose(window))
  {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // Input
    processInput(window);

    // Check collisions
    checkCoinCollisions();
    checkPerimeterCollisions();

    // Rendering
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();

    // Update model matrix based on modelPosition
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, modelPosition); // Use the updated modelPosition
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

    // FLOOR

    floorShader.use();
    floorShader.setMat4("projection", projection);
    floorShader.setMat4("view", view);

    glBindVertexArray(floorVAO);
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // COIN

    coinShader.use();
    coinShader.setMat4("projection", projection);
    coinShader.setMat4("view", view);

    for (const auto& coinPos : coinPositions) {
      glm::mat4 coinModelMatrix = glm::mat4(1.0f);
      coinModelMatrix = glm::translate(coinModelMatrix, coinPos); // Move the coin to its position
      coinModelMatrix = glm::rotate(coinModelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate the coin to stand on its edge

      coinShader.setMat4("model", coinModelMatrix);
      coinModel.Draw(coinShader); // Assuming coinModel is your model for a single coin
    }

    // PERIMETER
    drawPerimeter(perimeterShader, 1.0f);
    drawPerimeter(perimeterShader, 3.0f);
    drawPerimeter(perimeterShader, 5.0f);
    drawPerimeter(perimeterShader, 7.0f);

    // SKYBOX

    // draw skybox as last
    glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
    skyboxShader.use();
    view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
    skyboxShader.setMat4("view", view);
    skyboxShader.setMat4("projection", projection);
    // skybox cube
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // set depth function back to default

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glDeleteVertexArrays(1, &skyboxVAO);
  glDeleteBuffers(1, &skyboxVBO);

  // glfw: terminate, clearing all previously allocated GLFW resources.
  // ------------------------------------------------------------------
  glfwTerminate();
  return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.ProcessKeyboard(FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.ProcessKeyboard(BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.ProcessKeyboard(LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  // make sure the viewport matches the new window dimensions; note that width and
  // height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);

  if (firstMouse)
  {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

  lastX = xpos;
  lastY = ypos;

  camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrComponents;
  unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
  if (data)
  {
    GLenum format;
    if (nrComponents == 1)
      format = GL_RED;
    else if (nrComponents == 3)
      format = GL_RGB;
    else if (nrComponents == 4)
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  }
  else
  {
    std::cout << "Texture failed to load at path: " << path << std::endl;
    stbi_image_free(data);
  }

  return textureID;
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front)
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(vector<std::string> faces)
{
  unsigned int textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

  int width, height, nrChannels;
  for (unsigned int i = 0; i < faces.size(); i++)
  {
    unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
    // Flip the image vertically
    int bytesPerPixel = nrChannels;
    unsigned char *flippedData = new unsigned char[width * height * bytesPerPixel];
    for (int y = 0; y < height; y++)
    {
      memcpy(flippedData + y * width * bytesPerPixel,
             data + (height - 1 - y) * width * bytesPerPixel,
             width * bytesPerPixel);
    }

    // Use the flipped data for the texture
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, flippedData);

    // Free the flipped data memory
    delete[] flippedData;
    stbi_image_free(data);
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  return textureID;
}

void initCoins() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(-gameZoneSize / 2, gameZoneSize / 2);

  for (int i = 0; i < 30; ++i) {
    float x = dis(gen);
    float z = dis(gen);
    coinPositions.push_back(glm::vec3(x, 5.0f, z)); // Assuming y = 0 for ground-level
  }
}

void checkCoinCollisions() {
  for (auto it = coinPositions.begin(); it != coinPositions.end();) {
    // check distance without height (y) component
    if (glm::distance(glm::vec2(camera.Position.x, camera.Position.z), glm::vec2(it->x, it->z)) < 2.0f) {
      it = coinPositions.erase(it);
      camera.MovementSpeed *= 1.3f; // Increase speed on collecting a coin
      coinsCollected++;
      std::cout << "Coin collected! Total coins: " << coinsCollected << std::endl;
      std::cout << "Current speed: " << camera.MovementSpeed << std::endl;
    } else {
      ++it;
    }
  }
}

void checkPerimeterCollisions() {
  if (camera.Position.x < -gameZoneSize / 2 || camera.Position.x > gameZoneSize / 2 ||
      camera.Position.z < -gameZoneSize / 2 || camera.Position.z > gameZoneSize / 2) {
    std::cout << "Out of bounds!" << std::endl;
    glfwTerminate();
  }
}

void drawPerimeter(Shader& shader, float height) {
  // Define the vertices of the perimeter
  float vertices[] = {
          -gameZoneSize / 2, height, -gameZoneSize / 2,
          gameZoneSize / 2, height, -gameZoneSize / 2,
          gameZoneSize / 2, height, gameZoneSize / 2,
          -gameZoneSize / 2, height, gameZoneSize / 2,
          -gameZoneSize / 2, height, -gameZoneSize / 2
  };

  unsigned int VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  shader.use();
  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 300.0f);
  glm::mat4 view = camera.GetViewMatrix();
  shader.setMat4("model", model);
  shader.setMat4("view", view);
  shader.setMat4("projection", projection);

  glBindVertexArray(VAO);
  glDrawArrays(GL_LINE_STRIP, 0, 5);
  glBindVertexArray(0);

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
}
