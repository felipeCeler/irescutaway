/**
 * Tucano - A library for rapid prototying with Modern OpenGL and GLSL
 * Copyright (C) 2014
 * LCG - Laboratório de Computação Gráfica (Computer Graphics Lab) - COPPE
 * UFRJ - Federal University of Rio de Janeiro
 *
 * This file is part of Tucano Library.
 *
 * Tucano Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Tucano Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Tucano Library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __PPMIMPORTER__
#define __PPMIMPORTER__

#include <mesh.hpp>

using namespace std;

namespace Tucano
{

namespace ImageImporter
{

static void loadPPMImage (Texture* tex, string filename) __attribute__ ((unused));

/**
 * @brief Loads a texture from a PPM file.
 * The texture receives data in the range [0,1] to create a FLOAT texture
 *
 * @param tex Pointer to the texture
 * @param filename Given filename of the PPM file.
 */
static void loadPPMImage (Texture *tex, string filename)
{
    //Opening file:
    #ifdef TUCANODEBUG
    cout << "Opening PPM image file " << filename.c_str() << endl << endl;
    #endif

    ifstream in(filename.c_str(),ios::in);
    if (!in)
    {
        cerr << "Cannot open " << filename.c_str() << endl; exit(1);
    }

    
    vector<float> data;
    string header;
    in >> header;
    int w, h;
    in >> w >> h;
    float max_value;
    in >> max_value;

    float value;
    while (in >> value)
    {
        data.push_back(value/max_value);
    }
    cout << endl;

    if(in.is_open())
    {
        in.close();
    }

    
    cout << "creating texture " << endl;
    tex->create (GL_TEXTURE_2D, GL_RGBA32F, w, h, GL_RGB, GL_INT, &data[0], 0);

    #ifdef TUCANODEBUG
    Misc::errorCheckFunc(__FILE__, __LINE__);
    #endif
}

}
}
#endif
