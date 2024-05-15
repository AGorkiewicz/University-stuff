#pragma once
#include "utils.cpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

unsigned int load_texture_stbi(const char* path)
{
	stbi_set_flip_vertically_on_load(1);
 
	int width, height, num_components;		
	unsigned char* image_data = stbi_load(path, &width, &height, &num_components, 0);
 
	uint textureID = 0;
	glGenTextures(1, &textureID);

	if (image_data){
		GLenum format{};
 
		if (num_components == 1) format = GL_RED;
		if (num_components == 3) format = GL_RGB;
		if (num_components == 4) format = GL_RGBA;		
 
		glBindTexture(GL_TEXTURE_2D, textureID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image_data);
		glGenerateMipmap(GL_TEXTURE_2D);
 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 
		cerr << "Image loaded OK\n";
	}else cerr << "Image failed to load\n";

	stbi_image_free(image_data);

	return textureID;
}

uint load_texture(const char* path)
{
	static unsigned char header[1005];
	static char filecode[4]; 

	FILE* file = fopen(path, "rb"); 

	if (file == NULL) return printf("Error loading file %s\n", path), 0;

	fread(filecode, 1, 4, file);

	if (strncmp(filecode, "DDS ", 4) != 0) return fclose(file), load_texture_stbi(path);

	fread(&header, 124, 1, file); 

	unsigned int height      = *(unsigned int*) & (header[8]);
	unsigned int width	     = *(unsigned int*) & (header[12]);
	unsigned int linearSize	 = *(unsigned int*) & (header[16]);
	unsigned int mipMapCount = *(unsigned int*) & (header[24]);
	unsigned int fourCC      = *(unsigned int*) & (header[80]);

	unsigned char* buffer;
	unsigned int bufsize;
	bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize; 
	buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char)); 

	fread(buffer, 1, bufsize, file); 
	fclose(file);

	unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4; 
	unsigned int format;

	switch(fourCC) 
	{ 
		case FOURCC_DXT1: { format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; break; }
		case FOURCC_DXT3: { format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; break; }
		case FOURCC_DXT5: { format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; break; } 
		default: return free(buffer), 0;
	}

	uint textureID = 0;

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16; 
	unsigned int offset = 0;

	for (unsigned int level = 0; level < mipMapCount && (width || height); ++level) // load the mipmaps 
	{ 
		unsigned int size = ((width+3)/4)*((height+3)/4)*blockSize; 
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height, 0, size, buffer + offset); 

		offset += size; 
		width  /= 2; 
		height /= 2; 

		if (width < 1) width = 1;
		if (height < 1) height = 1;
	}

	free(buffer); 

	cerr << "Image loaded OK\n";

	return textureID;
}
