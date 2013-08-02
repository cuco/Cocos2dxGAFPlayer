#pragma once

#ifndef __GAF_SPRITE__
#define __GAF_SPRITE__

#include "sprite_nodes/CCSprite.h"
#include "cocoa/CCAffineTransform.h"
#include <string>

typedef struct _gafBlendFuncSeparate
{
	// source blend function
	GLenum src;
	// destination blend function
	GLenum dst;
    // source alpha channel blend function
	GLenum srcAlpha;
    // destination alpha channel blend function
	GLenum dstAlpha;
    
} gafBlendFuncSeparate;

using namespace cocos2d;

class GAFSprite : public CCSprite
{
public:
	GAFSprite();
	void setExternaTransform(const CCAffineTransform& transform);
	void setChildTransform(const CCAffineTransform& transform);
    virtual CCAffineTransform nodeToParentTransform(void);
	std::string objectId;
	virtual void updateTransform(void);
	virtual void draw();
	virtual void setUniformsForFragmentShader();
	void setUseExternalTransform(bool use);
	void setUseChildTransform(bool use);
	
	inline bool isUseExternalTransform() const
	{
		return _useExternalTransform;
	}
	
	inline bool isUseChildTransform() const
	{
		return _useChildTransform;
	}
	inline const CCAffineTransform &childTransform() const
	{
		return _childTransform;
	}
	const CCAffineTransform& getExternalTransform() const
	{
		return _externalTransform;
	}
	inline void setLocator(bool locator)
	{
		_isLocator = locator;
	}
	void setAtlasScale(float scale);
protected:
	CCAffineTransform _externalTransform;
	CCAffineTransform _childTransform;
	void invalidateTransformCache();
	void invalidateChildrenTranformCache();
private:
	gafBlendFuncSeparate _blendFuncSeparate;
	bool _useExternalTransform;
	bool _useChildTransform;
	bool _useSeparateBlendFunc;
	bool _isLocator;
	GLint _blendEquation;
	float _atlasScale;
};


#endif // __GAF_SPRITE__
