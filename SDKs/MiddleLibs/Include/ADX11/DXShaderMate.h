/*
 * FILE: DXShaderMate.h
 *
 * CREATED BY: duyuxin, 2018/10/17
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _DXSHADERMATE_H_
#define _DXSHADERMATE_H_

#include <d3d11.h>

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CDXVertexShaderMate
//
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
//
//	Class CDXPixelShaderMate
//
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
//
//	Class CDXComputeShaderMate
//
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

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_DXSHADERMATE_H_

