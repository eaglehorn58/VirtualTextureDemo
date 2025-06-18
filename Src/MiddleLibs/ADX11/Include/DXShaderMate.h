//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#ifndef _DXSHADERMATE_H_
#define _DXSHADERMATE_H_

#include <d3d11.h>

///////////////////////////////////////////////////////////////////////////

class CDXVertexShaderMate
{
public:		//	Types

public:		//	Constructors and Destructors

	virtual ~CDXVertexShaderMate() {}

public:		//	Attributes

public:		//	Operations

	//	Get input element desc number
	virtual int GetInputElemDescNum() = 0;
	//	Get input element desc
	virtual const D3D11_INPUT_ELEMENT_DESC* GetInputElemDesc() = 0;
	//	Get number of const buffer this mate has
	virtual int GetConstBufNum() = 0;
	//	Get const buffer length
	virtual int GetConstBufLen(int slot) = 0;
	//	Get const buffer data
	virtual const void* GetConstBufData(int slot) = 0;

protected:	//	Attributes

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////

class CDXPixelShaderMate
{
public:		//	Types

public:		//	Constructors and Destructors

	virtual ~CDXPixelShaderMate() {}

public:		//	Attributes

public:		//	Operations

	//	Get number of const buffer this mate has
	virtual int GetConstBufNum() = 0;
	//	Get const buffer length
	virtual int GetConstBufLen(int slot) = 0;
	//	Get const buffer data
	virtual const void* GetConstBufData(int slot) = 0;

protected:	//	Attributes

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////

class CDXComputeShaderMate
{
public:		//	Types

public:		//	Constructors and Destructors

	virtual ~CDXComputeShaderMate() {}

public:		//	Attributes

public:		//	Operations

	//	Get number of const buffer this mate has
	virtual int GetConstBufNum() = 0;
	//	Get const buffer length
	virtual int GetConstBufLen(int slot) = 0;
	//	Get const buffer data
	virtual const void* GetConstBufData(int slot) = 0;

protected:	//	Attributes

protected:	//	Operations

};



#endif	//	_DXSHADERMATE_H_

