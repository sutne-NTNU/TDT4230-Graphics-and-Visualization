#include "shapes.hpp"

#include <glm/glm.hpp>

#include "utils.hpp"

namespace SHAPES
{
    Mesh Cube(float size)
    {
        glm::vec3 corner = glm::vec3(size, size, size) * 0.5f;

        //              TBL-------TBR
        //             / |       / |
        //           TFL------TFR  |
        //           |   |      |  |
        //           |  BBL-----|-BBR
        //           | /        | /
        //           BFL-------BFR

        glm::vec3 TBR = corner * glm::vec3(1, 1, 1);
        glm::vec3 TBL = corner * glm::vec3(-1, 1, 1);
        glm::vec3 TFL = corner * glm::vec3(-1, 1, -1);
        glm::vec3 TFR = corner * glm::vec3(1, 1, -1);
        glm::vec3 BBR = corner * glm::vec3(1, -1, 1);
        glm::vec3 BBL = corner * glm::vec3(-1, -1, 1);
        glm::vec3 BFL = corner * glm::vec3(-1, -1, -1);
        glm::vec3 BFR = corner * glm::vec3(1, -1, -1);

        Mesh mesh;
        mesh.addQuad(TBR, TBL, TFL, TFR); // Top
        mesh.addQuad(BBR, BFR, BFL, BBL); // Bottom
        mesh.addQuad(TBL, BBL, BFL, TFL); // Left
        mesh.addQuad(TBR, TFR, BFR, BBR); // Right
        mesh.addQuad(BFR, TFR, TFL, BFL); // Front
        mesh.addQuad(BBL, TBL, TBR, BBR); // Back
        return mesh;
    }



    Mesh Pyramid(float height, float baseWidth)
    {
        //               BL---BR
        //               |\   /|
        //               | TOP |
        //               |/   \|
        //               FL---FR

        glm::vec3 TOP = glm::vec3(0, height, 0) * 0.5f;
        glm::vec3 BR  = glm::vec3(baseWidth, -height, baseWidth) * 0.5f;
        glm::vec3 BL  = glm::vec3(-baseWidth, -height, baseWidth) * 0.5f;
        glm::vec3 FL  = glm::vec3(-baseWidth, -height, -baseWidth) * 0.5f;
        glm::vec3 FR  = glm::vec3(baseWidth, -height, -baseWidth) * 0.5f;

        Mesh mesh;
        mesh.addQuad(FL, BL, BR, FR);  // Bottom
        mesh.addTriangle(TOP, FL, FR); // Front
        mesh.addTriangle(TOP, FR, BR); // Right
        mesh.addTriangle(TOP, BR, BL); // Back
        mesh.addTriangle(TOP, BL, FL); // Left
        return mesh;
    }



    Mesh Prism(float height, float width, float thickness)
    {

        //                  BT
        //                 / | \  
        //               BL--|-BR
        //               |  FT  |
        //               | /  \ |
        //               FL----FR

        glm::vec3 BT = 0.5f * glm::vec3(0, height, thickness);
        glm::vec3 BR = 0.5f * glm::vec3(width, -height, thickness);
        glm::vec3 BL = 0.5f * glm::vec3(-width, -height, thickness);
        glm::vec3 FT = 0.5f * glm::vec3(0, height, -thickness);
        glm::vec3 FR = 0.5f * glm::vec3(width, -height, -thickness);
        glm::vec3 FL = 0.5f * glm::vec3(-width, -height, -thickness);

        Mesh mesh;
        mesh.addQuad(BL, BR, FR, FL); // Bottom
        mesh.addQuad(FT, BT, BL, FL); // Left
        mesh.addQuad(BT, FT, FR, BR); // Right
        mesh.addTriangle(FT, FL, FR); // Front
        mesh.addTriangle(BT, BR, BL); // Back
        return mesh;
    }



    Mesh Cylinder(float radius, float height)
    {
        // Consider a sylinder just a bunch of slices of cake, where each slice is a partial prism
        unsigned int slices = radius * detailLevel;

        float y = height * 0.5f;

        glm::vec3 TC = glm::vec3(0, y, 0);  // top - cylinder center
        glm::vec3 BC = glm::vec3(0, -y, 0); // bottom - cylinder center

        Mesh mesh;

        float angle, x, z;
        for (int slice = 0; slice < slices; slice++)
        {
            // Each slice looks like this (looking downwards)):
            //            TC
            //           /  \  
            //         TL----TR
            //         |  BC  |
            //         | /  \ |
            //         BL----BR

            angle = 2 * PI * slice / slices;
            x     = radius * glm::cos(angle);
            z     = radius * glm::sin(angle);

            glm::vec3 TL = glm::vec3(x, y, z);
            glm::vec3 BL = glm::vec3(x, -y, z);

            angle = 2 * PI * (slice + 1) / slices;
            x     = radius * glm::cos(angle);
            z     = radius * glm::sin(angle);

            glm::vec3 TR = glm::vec3(x, y, z);
            glm::vec3 BR = glm::vec3(x, -y, z);

            mesh.addTriangle(TC, TL, TR);        // Top
            mesh.addTriangle(BC, BR, BL);        // Bottom
            mesh.addQuad(TL, BL, BR, TR, CURVE); // Edge
        }

        return mesh;
    }



    Mesh Sphere(float radius)
    {
        unsigned int slices = radius * detailLevel, layers = radius * detailLevel;

        Mesh mesh;
        for (int slice = 0; slice < slices; slice++)
        {
            for (int layer = 0; layer < layers; layer++)
            {
                // Consider this a "patch" on the surface of the sphere
                //        TL---TR
                //        |     |
                //        BL---BR

                glm::vec3 BL = UTILS::sphereCoordinates(radius, slice, slices, layer, layers);
                glm::vec3 TL = UTILS::sphereCoordinates(radius, slice, slices, layer + 1, layers);
                glm::vec3 BR = UTILS::sphereCoordinates(radius, slice + 1, slices, layer, layers);
                glm::vec3 TR = UTILS::sphereCoordinates(radius, slice + 1, slices, layer + 1, layers);

                mesh.addQuad(TL, BL, BR, TR, SPHERE);
            }
        }
        return mesh;
    }
}