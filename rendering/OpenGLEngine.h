/*************************************************************************************
 
	cpl - cross-platform library - v. 0.1.0.
 
	Copyright (C) 2015 Janus Lynggaard Thorborg [LightBridge Studios]
 
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
 
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
 
	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
	See \licenses\ for additional details on licenses associated with this program.
 
 **************************************************************************************
 
	file:SubpixelRendering.h
 
		Utilities and types needed for rendering subpixel graphics.

 *************************************************************************************/

#ifndef _OPENGLENGINE_H
	#define _OPENGLENGINE_H
	#include "../Common.h"
	#include "../Graphics.h"
	#include <vector>

	#ifndef GL_MULTISAMPLE
		#define GL_MULTISAMPLE  0x809D
	#endif

	/// <summary>
	/// returns true if any errors were caught. 
	/// </summary>
	#define CPL_DEBUGCHECKGL() cpl::OpenGLEngine::DebugCheckGLErrors(__FILE__, __LINE__, __FUNCTION__)


	namespace cpl
	{
		namespace OpenGLEngine
		{
			typedef GLenum GLFeatureType;
			typedef GLint GLSetting;
			typedef GLfloat Vertex;
			typedef GLfloat ColourType;


			inline const char* getGLErrorMessage(const GLenum e)
			{
				switch (e)
				{
				case GL_INVALID_ENUM:                   return "GL_INVALID_ENUM";
				case GL_INVALID_VALUE:                  return "GL_INVALID_VALUE";
				case GL_INVALID_OPERATION:              return "GL_INVALID_OPERATION";
				case GL_OUT_OF_MEMORY:                  return "GL_OUT_OF_MEMORY";
#ifdef GL_STACK_OVERFLOW
				case GL_STACK_OVERFLOW:                 return "GL_STACK_OVERFLOW";
#endif
#ifdef GL_STACK_UNDERFLOW
				case GL_STACK_UNDERFLOW:                return "GL_STACK_UNDERFLOW";
#endif
#ifdef GL_INVALID_FRAMEBUFFER_OPERATION
				case GL_INVALID_FRAMEBUFFER_OPERATION:  return "GL_INVALID_FRAMEBUFFER_OPERATION";
#endif
				default: break;
				}

				return "Unknown error";
			}



			inline bool DebugCheckGLErrors(const char * file, int line, const char * function)
			{
				bool shallDebug = false;
				GLenum e; 
				while ((e = glGetError()) != GL_NO_ERROR)
				{
					shallDebug = true;
					DBG("OpenGL Error at " << function << " (" << file << ":" << line << "): " << (int)e << " > " << cpl::OpenGLEngine::getGLErrorMessage(e));
				}
				if (shallDebug)
				{
					BreakIfDebugged();
				}
				return shallDebug;
			}

			namespace Colour
			{


			};

			namespace Texture
			{
				/// <summary>
				/// .first = width
				/// .second = height
				/// 
				/// Note: binds the texture.
				/// </summary>
				/// <param name="textureID"></param>
				/// <returns></returns>
				inline std::pair<int, int> GetBounds(GLuint textureID)
				{
					glBindTexture(GL_TEXTURE_2D, textureID);
					std::pair<int, int> bounds;
					glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &bounds.first);
					glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &bounds.second);
					return bounds;
				}

				/// <summary>
				/// Note: binds the texture.
				/// </summary>

				inline void Copy2DTextureToMemory(GLuint textureID, unsigned char * pixels, std::size_t pixelBufferSize, GLenum format = GL_RGB, GLenum type = GL_UNSIGNED_BYTE)
				{
					auto bounds = GetBounds(textureID);

					CPL_DEBUGCHECKGL();

					if (std::size_t(bounds.first) * bounds.second != pixelBufferSize)
						CPL_RUNTIME_EXCEPTION("Invalid buffer size for copying texture to main memory!");

					glGetTexImage(GL_TEXTURE_2D, 0, format, type, pixels);


				}

			}

			class MatrixModeModification
			{
			public:
				MatrixModeModification()
				{
					glGetIntegerv(GL_MATRIX_MODE, &previousMode);
				}

				~MatrixModeModification()
				{

					glMatrixMode(previousMode);
				}
			private:
				GLint previousMode = 0;
			};

			class MatrixModification
			{
			public:

				MatrixModification()
				{
					saveMatrix();
				}
				void translate(GLfloat x, GLfloat y, GLfloat z)
				{

					glTranslatef(x, y, z);
				}

				void scale(GLfloat x, GLfloat y, GLfloat z)
				{
					glScalef(x, y, z);
				}

				void rotate(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
				{
					glRotatef(angle, x, y, z);
				}


				void applyTransform3D(GraphicsND::Transform3D<Vertex> & tsf)
				{

					appliedTransform = &tsf;

					appliedTransform->applyToOpenGL();

				}

				void loadIdentityMatrix()
				{
					glLoadIdentity();
				}

				void saveMatrix()
				{
					if (!matrixPushed)
					{
						matrixPushed = true;
						glPushMatrix();
					}
				}

				~MatrixModification()
				{
					if (matrixPushed)
					{
						glPopMatrix();
					}
				}

			private:
				bool matrixPushed = false;
				GraphicsND::Transform3D<Vertex> * appliedTransform = nullptr;

			};

			class COpenGLStack
			: 
				public MatrixModification
			{

			public:

				class Rasterizer
				{
					friend class COpenGLStack;
				protected:

					~Rasterizer() { parent.rasterizerDied(this); }
					Rasterizer(COpenGLStack & parentStack)
						: parent(parentStack)
					{
						parent.attachRasterizer(this);
					}
					COpenGLStack & parent;
				};

				friend class Rasterizer;

				COpenGLStack()
					: ras(nullptr)
				{
					CPL_DEBUGCHECKGL();
					// store the old blending functions.
					if (glIsEnabled(GL_BLEND))
					{
						glGetIntegerv(GL_BLEND_DST, &oldDestinationBlend);
						glGetIntegerv(GL_BLEND_SRC, &oldSourceBlend);
					}
					glGetFloatv(GL_POINT_SIZE, &oldPointSize);
					glGetFloatv(GL_LINE_WIDTH, &oldLineSize);
				}

				void setPointSize(GLfloat newPointSize)
				{
					glPointSize(newPointSize);
				}

				void setLineSize(GLfloat newLineSize)
				{
					glLineWidth(newLineSize);
				}

				~COpenGLStack()
				{
					if (ras)
					{
						// Your openGL stack got destroyed before attached rasterizers!!
						jassertfalse;
					}


					// revert blending function
					if (glIsEnabled(GL_BLEND))
					{
						glBlendFunc(oldDestinationBlend, oldSourceBlend);
					}

					// revert features
					for (auto it = features.rbegin(); it != features.rend(); ++it)
					{
						glDisable(*it);
					}
					glPointSize(oldPointSize);
					glLineWidth(oldLineSize);

				}

				void setAntialiasingIfNeeded()
				{
					enable(GL_MULTISAMPLE);
				}

				void enable(GLFeatureType feature)
				{
					if (glIsEnabled(feature) == GL_FALSE)
					{
						glEnable(feature);
						auto error = glGetError();
						if (error == GL_NO_ERROR)
						{
							features.push_back(feature);
						}
						else
						{
							jassertfalse;
						}
					}
				}

				void setBlender(GLFeatureType source, GLFeatureType destination)
				{
					enable(GL_BLEND);
					glBlendFunc(source, destination);
					
					auto error = glGetError();
					if (error != GL_NO_ERROR)
					{
						jassertfalse;
					}
				}

				void disable(GLFeatureType feature)
				{
					// could remove it from features.. meh
					glDisable(feature);
					auto error = glGetError();
					if (error != GL_NO_ERROR)
					{
						jassertfalse;
					}
				}



			private:

				void attachRasterizer(Rasterizer * rasterizer)
				{
					if (ras || !rasterizer)
					{
						// adding a rasterizer before removing old one!
						jassertfalse;
					}
					else
					{
						ras = rasterizer;
					}
				}

				void rasterizerDied(Rasterizer * deadRasterizer)
				{
					if (!ras || deadRasterizer != ras)
					{
						jassertfalse;
					}
					else
					{
						ras = nullptr;
					}
				}

				std::vector<GLFeatureType> features;
				Rasterizer * ras;
				GLSetting oldDestinationBlend, oldSourceBlend;
				GLfloat oldPointSize, oldLineSize;
			};

		}; // {} rendering
	}; // {} cpl
#endif