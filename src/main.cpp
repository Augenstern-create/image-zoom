#include <windows.h>
#include <string.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include <filesystem>

#pragma comment(lib, "legacy_stdio_definitions.lib")

//----------------------------------------------//
static unsigned int g_map;          // 图像纹理 ID
static float g_zoom_factor = 0.0f;  // 初始缩放因子
static ImVec2 g_screen_center = {0.0f, 0.0f}; //平移偏移
static std::string g_image_path = "\\photograph\\1-1.png";
//----------------------------------------------//
unsigned int CreateTextureFromImage(const char* path);
void ShowRadarWindow(bool* p_open, ImVec2 display_size);
//-----------------------------------------------//

int main(int, char**) {
    SetConsoleOutputCP(CP_UTF8);
    if (!glfwInit()) {
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
    GLFWwindow* window1 = glfwCreateWindow(1080, 1080, "Window 1", NULL, NULL);
    if (!window1) {
        glfwTerminate();
        return -1;
    }
    IMGUI_CHECKVERSION();
    ImGuiContext* ctx1 = ImGui::CreateContext();
    ImGui::SetCurrentContext(ctx1);
    glfwMakeContextCurrent(window1);
    ImGui_ImplGlfw_InitForOpenGL(window1, true);
    ImGui_ImplOpenGL3_Init();

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwDestroyWindow(window1);
        glfwTerminate();
        return -1;
    }
    std::filesystem::path currentPath = std::filesystem::current_path();
    std::string CurrentDirectory = currentPath.string();
    g_map = CreateTextureFromImage(std::string(CurrentDirectory + g_image_path).c_str());
    while (!glfwWindowShouldClose(window1)) {
        glfwPollEvents();
        ImGui::SetCurrentContext(ctx1);
        glfwMakeContextCurrent(window1);
        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();
        bool show_radar_window = true;
        ShowRadarWindow(&show_radar_window, ImVec2((float)1080, (float)1080));
        ImGui::Render();
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window1);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Cleanup
    glfwDestroyWindow(window1);
    glfwTerminate();
}

// 用于加载图像的函数
unsigned int CreateTextureFromImage(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }

    return textureID;
}

void ShowRadarWindow(bool* p_open, ImVec2 display_size) {
    IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing Dear ImGui context. Refer to examples app!");

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_None;                   // 0
    window_flags |= ImGuiWindowFlags_NoTitleBar;             // 禁用标题栏
    window_flags |= ImGuiWindowFlags_NoScrollbar;            // 禁用滚动条
    window_flags |= ImGuiWindowFlags_NoMove;                 // 禁止用户拖动
    window_flags |= ImGuiWindowFlags_NoResize;               // 禁止用户使用鼠标调整大小并禁用窗口边框
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;  // 聚焦时禁止将窗口置为上层
    ImGui::SetNextWindowSize({display_size.x - 20, display_size.x - 20}, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos({10, 10}, ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Windows Radar", p_open, window_flags)) {
        ImGui::End();
        return;
    }
    // 可用显示大小,超出部分不显示
    ImVec2 from_size = ImGui::GetContentRegionAvail();
    // 窗体开始位置
    ImVec2 childPos = ImGui::GetWindowPos();
    // 图片大小
    ImVec2 zoomedSize = ImVec2(from_size.x, from_size.x);
    ImGuiIO& io = ImGui::GetIO();
    // 显示起始位置(归一化)
    ImVec2 uv0 = {0.0f + (g_zoom_factor * 0.5f), 0.0f + (g_zoom_factor * 0.5f)};
    // 显示结束位置(归一化)
    ImVec2 uv1 = {1.0f - (g_zoom_factor * 0.5f), 1.0f - (g_zoom_factor * 0.5f)};
    // 鼠标滑轮滚动
    if (io.MouseWheel != 0.0f && ImGui::IsWindowHovered()) {
        // 计算缩放系数
        ImVec2 MousePos = io.MousePos;  // 鼠标位置
        float zoomSpeed = 0.05f;
        float zoomDelta = io.MouseWheel * zoomSpeed;
        float newZoomFactor = g_zoom_factor + zoomDelta;
        newZoomFactor = newZoomFactor < 0.0f ? 0.0f : newZoomFactor;
        newZoomFactor = newZoomFactor > 0.95f ? 0.95f : newZoomFactor;
        // 更新显示位置
        uv0 = {0.0f + (newZoomFactor * 0.5f), 0.0f + (newZoomFactor * 0.5f)};
        uv1 = {1.0f - (newZoomFactor * 0.5f), 1.0f - (newZoomFactor * 0.5f)};
        float imageWidth = 1.0f + (uv1.x - uv0.x) * 2.0f;
        float imageHeight = 1.0f + (uv1.y - uv0.y) * 2.0f;
        // 缩放后图片大小
        ImVec2 newImageSize = {zoomedSize.x * imageWidth, zoomedSize.y * imageHeight};

        // 鼠标在屏幕中位置
        ImVec2 fromMousePos = ImVec2((MousePos.x - childPos.x - 8.0f), (MousePos.y - childPos.y - 8.0f));
        // 鼠标在图片中位置
        ImVec2 imageMousePos = ImVec2((fromMousePos.x / from_size.x - (0.5f)), (fromMousePos.y / from_size.y - (0.5f)));
        // 计算中心偏移
        ImVec2 deltaOffset = ImVec2(imageMousePos.x * newZoomFactor, imageMousePos.y * newZoomFactor);
        g_screen_center = {deltaOffset.x, deltaOffset.y};
        g_zoom_factor = newZoomFactor;
    }
    // 鼠标拖动平移
    if (io.MouseDown[ImGuiMouseButton_Left] && ImGui::IsWindowHovered()) {
        float deltaX = io.MouseDelta.x * 0.2f;
        float deltaY = io.MouseDelta.y * 0.2f;
        if (deltaX != 0.0f || deltaY != 0.0f) {
            float imageWidth = 1.0f + (uv1.x - uv0.x) * 2.0f;
            float imageHeight = 1.0f + (uv1.y - uv0.y) * 2.0f;
            ImVec2 newImageSize = {zoomedSize.x * imageWidth, zoomedSize.y * imageHeight};
            g_screen_center.x -= deltaX / newImageSize.x;
            g_screen_center.y -= deltaY / newImageSize.y;
        }
    }
    // 计算边界,超过则补偿
    if (g_screen_center.x != 0.0f || g_screen_center.y != 0.0f) {
        uv0 = {uv0.x + g_screen_center.x, uv0.y + g_screen_center.y};
        uv1 = {uv1.x + g_screen_center.x, uv1.y + g_screen_center.y};
        if (uv0.x < 0.0f) {
            uv1 = {uv1.x - uv0.x, uv1.y};
            uv0 = {0.0f, uv0.y};
        }

        if (uv0.y < 0.0f) {
            uv1 = {uv1.x, uv1.y - uv0.y};
            uv0 = {uv0.x, 0.0f};
        }

        if (uv1.x > 1.0f) {
            uv0 = {uv0.x + (1.0f - uv1.x), uv0.y};
            uv1 = {1.0f, uv1.y};
        }

        if (uv1.y > 1.0f) {
            uv0 = {uv0.x, uv0.y + (1.0f - uv1.y)};
            uv1 = {uv1.x, 1.0f};
        }
    }
    // zoomedSize图片大小(1080*1080),uv0,起始位置,uv1,结束位置(0,0)(1,1),更改uv0与uv1完成缩放
    ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<intptr_t>(g_map)), zoomedSize, uv0, uv1);
    ImGui::End();
}
