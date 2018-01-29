#version 330 core

// Входные данные вершин, различные для всех запусков этого шейдера
layout(location = 0) in vec3 vertexPosition_modelspace;
//layout(location = 1) in vec2 vertexUV;

//in vec2 position;
//in vec2 uv;

// Выходные данные, которые будут интерполированы для каждого фрагмента
out vec2 UV;

// Значения, которые останутся неизменными для всего объекта
//uniform mat4 MVP;

void main() {
    // Выходная позиция вершины
//    gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
//    gl_Position = vec4(position, 0.0, 1.0);
    gl_Position = vec4(vertexPosition_modelspace, 1);

    UV = (vertexPosition_modelspace.xy + vec2(1, 1)) / 2.0;

    // UV-координаты вершины.
//    UV = vertexUV;
}
