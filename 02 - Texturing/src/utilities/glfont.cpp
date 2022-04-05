#include "glfont.h"
#include <iostream>

Mesh generateTextGeometryBuffer(std::string text, float totalTextWidth)
{
    float characterWidth  = totalTextWidth / float(text.length());
    float characterHeight = (39.0 / 29.0) * characterWidth;

    float textureCharacterWidth  = 1.0 / 128.0;
    float textureCharacterHeight = 1.0;

    unsigned int vertexCount = 4 * text.length();
    unsigned int indexCount  = 6 * text.length();

    Mesh mesh;

    mesh.vertices.resize(vertexCount);
    mesh.indices.resize(indexCount);
    mesh.normals.resize(vertexCount);
    mesh.textureCoordinates.resize(vertexCount);

    for (unsigned int i = 0; i < text.length(); i++)
    {
        float baseXCoordinate = float(i) * characterWidth;

        mesh.vertices.at(4 * i + 0) = { baseXCoordinate, 0, 0 };
        mesh.vertices.at(4 * i + 1) = { baseXCoordinate + characterWidth, 0, 0 };
        mesh.vertices.at(4 * i + 2) = { baseXCoordinate + characterWidth, characterHeight, 0 };
        mesh.vertices.at(4 * i + 3) = { baseXCoordinate, characterHeight, 0 };

        mesh.normals.at(4 * i + 0) = { 0, 0, 1 };
        mesh.normals.at(4 * i + 1) = { 0, 0, 1 };
        mesh.normals.at(4 * i + 2) = { 0, 0, 1 };
        mesh.normals.at(4 * i + 3) = { 0, 0, 1 };

        mesh.indices.at(6 * i + 0) = 4 * i + 0;
        mesh.indices.at(6 * i + 1) = 4 * i + 1;
        mesh.indices.at(6 * i + 2) = 4 * i + 2;
        mesh.indices.at(6 * i + 3) = 4 * i + 0;
        mesh.indices.at(6 * i + 4) = 4 * i + 2;
        mesh.indices.at(6 * i + 5) = 4 * i + 3;

        float baseXTexture = float(text[i]) * textureCharacterWidth;

        mesh.textureCoordinates.at(4 * i + 0) = { baseXTexture, 0 };
        mesh.textureCoordinates.at(4 * i + 1) = { baseXTexture + textureCharacterWidth, 0 };
        mesh.textureCoordinates.at(4 * i + 2) = { baseXTexture + textureCharacterWidth, textureCharacterHeight };
        mesh.textureCoordinates.at(4 * i + 3) = { baseXTexture, textureCharacterHeight };
    }

    return mesh;
}