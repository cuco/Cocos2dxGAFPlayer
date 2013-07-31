#include "GAFTextureEffectsConverter.h"
#include "CCRenderTexture.h"
#include "CCGLProgram.h"
#include "CCShaderCache.h"
#include "cocoa/CCDictionary.h"



GAFTextureEffectsConverter::GAFTextureEffectsConverter()
{
	_vertexShaderUniforms = new CCDictionary();
}

GAFTextureEffectsConverter::~GAFTextureEffectsConverter()
{
	CC_SAFE_RELEASE(_vertexShaderUniforms);
}

static GAFTextureEffectsConverter * _sharedConverter = NULL;

GAFTextureEffectsConverter * GAFTextureEffectsConverter::sharedConverter()
{
	if (!_sharedConverter)
	{
		_sharedConverter = new GAFTextureEffectsConverter();
	}
	return _sharedConverter;
}

CCRenderTexture * GAFTextureEffectsConverter::gaussianBlurredTextureFromTexture(CCTexture2D * aTexture, const CCRect& rect, float aBlurRadiusX, float aBlurRadiusY)
{
	const int kGaussianKernelSize = 9;
    
    CCSize rTextureSize = CCSizeMake(rect.size.width + 2 * (kGaussianKernelSize / 2) * aBlurRadiusX,
                                     rect.size.height + 2 * (kGaussianKernelSize / 2) * aBlurRadiusY);
	
	CCRenderTexture *rTexture1 = CCRenderTexture::create(rTextureSize.width, rTextureSize.height);
	CCRenderTexture *rTexture2 = CCRenderTexture::create(rTextureSize.width, rTextureSize.height);
	CCGLProgram * shader = programForBlurShaderWithName("GaussianBlur", "GaussianBlurVertexShader.vs", "GaussianBlurFragmentShader.fs");
	if (!shader)
	{
		return NULL;
	}
	GLint texelWidthOffset = (GLint)glGetUniformLocation(shader->getProgram(), "texelWidthOffset");
    GLint texelHeightOffset = (GLint)glGetUniformLocation(shader->getProgram(), "texelHeightOffset");
	CHECK_GL_ERROR_DEBUG();
    {
        // Render rTexture2 to rTexture1 (horizontal)
        GLfloat texelWidthValue = aBlurRadiusX / (GLfloat)rTextureSize.width;
        GLfloat texelHeightValue = 0;
        
        rTexture2->getSprite()->setPosition(CCPointMake(rTextureSize.width / 2, rTextureSize.height / 2));
        rTexture2->getSprite()->setShaderProgram(shader);
		shader->use();
        glUniform1f(texelWidthOffset, texelWidthValue);
        glUniform1f(texelHeightOffset, texelHeightValue);        
        rTexture2->getSprite()->setBlendFunc((ccBlendFunc){ GL_ONE, GL_ZERO });
        rTexture1->beginWithClear(0, 0, 0, 0);
		rTexture2->getSprite()->visit();
		rTexture1->end();
    }
	
	CHECK_GL_ERROR_DEBUG();    
    {
        // Render rTexture1 to rTexture2 (vertical)
        GLfloat texelWidthValue = aBlurRadiusX / (GLfloat)rTextureSize.width;
        GLfloat texelHeightValue = 0;
        
        rTexture1->getSprite()->setPosition(CCPointMake(rTextureSize.width / 2, rTextureSize.height / 2));
        rTexture1->getSprite()->setShaderProgram(shader);
		shader->use();
        glUniform1f(texelWidthOffset, texelWidthValue);
        glUniform1f(texelHeightOffset, texelHeightValue);
        rTexture1->getSprite()->setBlendFunc((ccBlendFunc){ GL_ONE, GL_ZERO });
        rTexture2->beginWithClear(0, 0, 0, 0);
		rTexture1->getSprite()->visit();
		rTexture2->end();
    }    
    CHECK_GL_ERROR_DEBUG();
    
    return rTexture2;
}

CCGLProgram * GAFTextureEffectsConverter::programForBlurShaderWithName(const char * aShaderName, const char * aVertexShaderFile, const char * aFragmentShaderFile)
{
	CCGLProgram *program = CCShaderCache::sharedShaderCache()->programForKey(aShaderName);
	if (!program)
	{
		program = new CCGLProgram();
		program->initWithVertexShaderFilename(aVertexShaderFile, aFragmentShaderFile);
		if (program)
		{
			program->addAttribute("position", kCCVertexAttrib_Position);
			program->addAttribute("inputTextureCoordinate", kCCVertexAttrib_TexCoords);
			program->link();
			program->updateUniforms();
            CHECK_GL_ERROR_DEBUG();
			CCShaderCache::sharedShaderCache()->addProgram(program, aShaderName);
		}
		else
		{
			CCLOGWARN("Cannot load program for %s.", aShaderName);
			return NULL;
		}
	}
	return program;
}