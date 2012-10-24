#include "TextureFactory.h"

#include <QImage>
#include <qgl.h>
#include <iostream>
using namespace std;

TextureFactory::TextureFactory()
{
	counter = 0;
}

TextureFactory::~TextureFactory()
{
	// alle textures remove
}

void TextureFactory::LoadTexture(string key, string filename)
{
	cout << "Texture inladen : " << key << " \n";
	counter++;

	if( counter < NR_TEXTURES )
	{
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_EQUAL, 1.0);
		textureKeys[key] = counter;

		QImage *img = new QImage();

		img->load(QString::fromStdString(filename));
		*img = QGLWidget::convertToGLFormat(*img);
		glGenTextures(1, &textures[counter]);

		// MIPMAPPED
		glBindTexture(GL_TEXTURE_2D, textures[counter]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

		/*glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/

		//glEnable (GL_BLEND); glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glBlendFunc (GL_ONE, GL_ONE);
		

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		gluBuild2DMipmaps(GL_TEXTURE_2D, 4,(GLsizei)img->width(),(GLsizei)img->height(), GL_RGBA, GL_UNSIGNED_BYTE, img->bits());
		

		//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND); // zet alles in't negatief blijkbaar

		/*glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);*/

		 //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	}
	else
		cout << "Er zijn reeds teveel textures ingeladen. Maximum aantal : " << NR_TEXTURES << "\n";
}

GLuint TextureFactory::GetTexture(string key)
{
	return textures[ textureKeys[key] ];
}

template<> TextureFactory* Singleton<TextureFactory>::ms_Singleton = 0;
TextureFactory* TextureFactory::getSingletonPtr(void)
{
	return ms_Singleton;
}
TextureFactory& TextureFactory::getSingleton(void)
{  
	return ( *ms_Singleton );  
}