//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com

#include "AXMLArchive.h"
//#include "AFramework.h"

using namespace tinyxml2;

XMLElement*  AXMLArchive::NewElement(const achar* name)
{
	XMLElement* pNewElement = m_pXmlDoc->NewElement(name);
	return pNewElement;
}

tinyxml2::XMLElement* AXMLArchive::NewElementAsChild(const achar* name)
{
	XMLElement* pNewElement = m_pXmlDoc->NewElement(name);

	GetCurrentXMLNode()->LinkEndChild(pNewElement);

	return pNewElement;
}

tinyxml2::XMLElement* AXMLArchive::GetChildElement(const achar* name, abool bFirst /*= afalse*/)
{
	return bFirst ? GetCurrentXMLNode()->FirstChildElement(name) : GetCurrentXMLNode()->LastChildElement(name);
}

AXMLArchive::XMLElementVector AXMLArchive::GetChildElements(const achar* name)
{
	XMLElementVector ret;

	auto pChildNode = GetCurrentXMLNode()->FirstChild();

	while (pChildNode)
	{
		if (strcmp(pChildNode->ToElement()->Name(), name) == 0)
			ret.push_back(pChildNode->ToElement());

		pChildNode = pChildNode->NextSibling();
	}

	return move(ret);
}

AString AXMLArchive::ReadString(const achar* key, const achar* defval)
{
	AString str = defval;
	XMLElement* pChild = GetCurrentXMLNode()->FirstChildElement();
	while(pChild)
	{
		if(_stricmp(pChild->Name(),key)==0)
			return pChild->Attribute(key);
		pChild = pChild->NextSiblingElement();
	}
	return str;
}
aint32 AXMLArchive::ReadInt32(const achar* key, aint32 defval)
{
	aint32 i = defval;
	XMLElement* pChild = GetCurrentXMLNode()->FirstChildElement();
	while(pChild)
	{
		if(_stricmp(pChild->Name(),key)==0)
			return pChild->IntAttribute(key);
		pChild = pChild->NextSiblingElement();
	}
	return i;
}
auint32 AXMLArchive::ReadUInt32(const achar* key, auint32 defval)
{
	auint32 u = defval;
	XMLElement* pChild = GetCurrentXMLNode()->FirstChildElement();
	while(pChild)
	{
		if(_stricmp(pChild->Name(),key)==0)
			return pChild->UnsignedAttribute(key);
		pChild = pChild->NextSiblingElement();
	}
	return u;
}

abool AXMLArchive::ReadBool(const achar* key, abool defval /*= afalse*/)
{
	abool u = defval;
	XMLElement* pChild = GetCurrentXMLNode()->FirstChildElement();
	while (pChild != nullptr)
	{
		if (_stricmp(pChild->Name(), key) == 0)
			return pChild->BoolAttribute(key);
		pChild = pChild->NextSiblingElement();
	}
	return u;
}

afloat32 AXMLArchive::ReadFloat32(const achar* key, afloat32 defval)
{
	afloat32 f = defval;
	XMLElement* pChild = GetCurrentXMLNode()->FirstChildElement();
	while(pChild)
	{
		if(_stricmp(pChild->Name(),key)==0)
			return pChild->FloatAttribute(key);
		pChild = pChild->NextSiblingElement();
	}
	return f;
}

AXVector3 AXMLArchive::ReadVector3(const achar* key, const AXVector3& defval)
{
	AXVector3 v = defval;
	XMLElement* pChild = GetCurrentXMLNode()->FirstChildElement();
	while(pChild)
	{
		if(_stricmp(pChild->Name(),key)==0)
		{
			v.x = pChild->FloatAttribute("x");
			v.y = pChild->FloatAttribute("y");
			v.z = pChild->FloatAttribute("z");
			break;
		}
		pChild = pChild->NextSiblingElement();
	}

	return v;
}

AXVector3 AXMLArchive::ReadVector3Cap(const achar* key, const AXVector3& defval /*= AXVector3(0.0f)*/)
{
	AXVector3 v = defval;
	XMLElement* pChild = GetCurrentXMLNode()->FirstChildElement();
	while (pChild != nullptr)
	{
		if (_stricmp(pChild->Name(), key) == 0)
		{
			v.x = pChild->FloatAttribute("X");
			v.y = pChild->FloatAttribute("Y");
			v.z = pChild->FloatAttribute("Z");
			break;
		}
		pChild = pChild->NextSiblingElement();
	}

	return v;
}

AXVector2 AXMLArchive::ReadVector2(const achar* key, const AXVector2& defval)
{
	AXVector2 v = defval;
	XMLElement* pChild = GetCurrentXMLNode()->FirstChildElement();
	while (pChild)
	{
		if (_stricmp(pChild->Name(), key) == 0)
		{
			v.x = pChild->FloatAttribute("x");
			v.y = pChild->FloatAttribute("y");
			break;
		}
		pChild = pChild->NextSiblingElement();
	}

	return v;
}

AXVector2 AXMLArchive::ReadVector2Cap(const achar* key, const AXVector2& defval)
{
	AXVector2 v = defval;
	XMLElement* pChild = GetCurrentXMLNode()->FirstChildElement();
	while (pChild != nullptr)
	{
		if (_stricmp(pChild->Name(), key) == 0)
		{
			v.x = pChild->FloatAttribute("X");
			v.y = pChild->FloatAttribute("Y");
			break;
		}
		pChild = pChild->NextSiblingElement();
	}

	return v;
}

AXVector4 AXMLArchive::ReadVector4(const achar* key, const AXVector4& defval /*= AXVector4(0.0f)*/)
{
	AXVector4 v = defval;
	XMLElement* pChild = GetCurrentXMLNode()->FirstChildElement();
	while (pChild != nullptr)
	{
		if (_stricmp(pChild->Name(), key) == 0)
		{
			v.x = pChild->FloatAttribute("x");
			v.y = pChild->FloatAttribute("y");
			v.z = pChild->FloatAttribute("z");
			v.w = pChild->FloatAttribute("w");
			break;
		}
		pChild = pChild->NextSiblingElement();
	}

	return v;
}

AXVector4 AXMLArchive::ReadVector4Cap(const achar* key, const AXVector4& defval /*= AXVector4(0.0f)*/)
{
	AXVector4 v = defval;
	XMLElement* pChild = GetCurrentXMLNode()->FirstChildElement();
	while (pChild != nullptr)
	{
		if (_stricmp(pChild->Name(), key) == 0)
		{
			v.x = pChild->FloatAttribute("X");
			v.y = pChild->FloatAttribute("Y");
			v.z = pChild->FloatAttribute("Z");
			v.w = pChild->FloatAttribute("W");
			break;
		}
		pChild = pChild->NextSiblingElement();
	}

	return v;
}

A3DCOLORVALUE AXMLArchive::ReadClrValue(const achar* key, const A3DCOLORVALUE& defval)
{
	A3DCOLORVALUE clr = defval;
	XMLElement* pChild = GetCurrentXMLNode()->FirstChildElement();
	while(pChild)
	{
		if(_stricmp(pChild->Name(),key)==0)
		{
			clr.a = pChild->FloatAttribute("a");
			clr.r = pChild->FloatAttribute("r");
			clr.g = pChild->FloatAttribute("g");
			clr.b = pChild->FloatAttribute("b");
			break;
		}
		pChild = pChild->NextSiblingElement();
	}

	return clr;
}

A3DCOLORVALUE AXMLArchive::ReadColorValueCap(const achar* key, const A3DCOLORVALUE& defval /*= A3DCOLORVALUE(1.0f)*/)
{
	A3DCOLORVALUE clr = defval;
	XMLElement* pChild = GetCurrentXMLNode()->FirstChildElement();
	while (pChild != nullptr)
	{
		if (_stricmp(pChild->Name(), key) == 0)
		{
			clr.a = pChild->FloatAttribute("A");
			clr.r = pChild->FloatAttribute("R");
			clr.g = pChild->FloatAttribute("G");
			clr.b = pChild->FloatAttribute("B");
			break;
		}
		pChild = pChild->NextSiblingElement();
	}

	return clr;
}

void AXMLArchive::WriteString(const achar* key, const achar* str)
{
	if (!str)
		str = "";
	XMLElement* pNew = m_pXmlDoc->NewElement(key);
	pNew->SetAttribute(key,str);
	GetCurrentXMLNode()->LinkEndChild(pNew);
}
void AXMLArchive::WriteInt32(const achar* key, aint32 i)
{
	XMLElement* pNew = m_pXmlDoc->NewElement(key);
	pNew->SetAttribute(key,i);
	GetCurrentXMLNode()->LinkEndChild(pNew);
}
void AXMLArchive::WriteUInt32(const achar* key, auint32 u)
{
	XMLElement* pNew = m_pXmlDoc->NewElement(key);
	pNew->SetAttribute(key,u);
	GetCurrentXMLNode()->LinkEndChild(pNew);
}

void AXMLArchive::WriteBool(const achar* key, abool b)
{
	XMLElement* pNew = m_pXmlDoc->NewElement(key);
	pNew->SetAttribute(key, b);
	GetCurrentXMLNode()->LinkEndChild(pNew);
}

void AXMLArchive::WriteFloat32(const achar* key, afloat32 f)
{
	XMLElement* pNew = m_pXmlDoc->NewElement(key);
	pNew->SetAttribute(key,f);
	GetCurrentXMLNode()->LinkEndChild(pNew);
}

void AXMLArchive::WriteVector3(const achar* key, const AXVector3& vec3)
{
	XMLElement* pNew = m_pXmlDoc->NewElement(key);
	pNew->SetAttribute("x",vec3.x);
	pNew->SetAttribute("y",vec3.y);
	pNew->SetAttribute("z",vec3.z);
	GetCurrentXMLNode()->LinkEndChild(pNew);
}

void AXMLArchive::WriteVector3Cap(const achar* key, const AXVector3& vec3)
{
	XMLElement* pNew = m_pXmlDoc->NewElement(key);
	pNew->SetAttribute("X", vec3.x);
	pNew->SetAttribute("Y", vec3.y);
	pNew->SetAttribute("Z", vec3.z);
	GetCurrentXMLNode()->LinkEndChild(pNew);
}

void AXMLArchive::WriteVector2(const achar* key, const AXVector2& vec2)
{
	XMLElement* pNew = m_pXmlDoc->NewElement(key);
	pNew->SetAttribute("x", vec2.x);
	pNew->SetAttribute("y", vec2.y);
	GetCurrentXMLNode()->LinkEndChild(pNew);
}

void AXMLArchive::WriteVector2Cap(const achar* key, const AXVector2& vec2)
{
	XMLElement* pNew = m_pXmlDoc->NewElement(key);
	pNew->SetAttribute("X", vec2.x);
	pNew->SetAttribute("Y", vec2.y);
	GetCurrentXMLNode()->LinkEndChild(pNew);
}

void AXMLArchive::WriteVector4(const achar* key, const AXVector4& vec4)
{
	XMLElement* pNew = m_pXmlDoc->NewElement(key);
	pNew->SetAttribute("x", vec4.x);
	pNew->SetAttribute("y", vec4.y);
	pNew->SetAttribute("z", vec4.z);
	pNew->SetAttribute("w", vec4.w);
	GetCurrentXMLNode()->LinkEndChild(pNew);
}

void AXMLArchive::WriteVector4Cap(const achar* key, const AXVector4& vec4)
{
	XMLElement* pNew = m_pXmlDoc->NewElement(key);
	pNew->SetAttribute("X", vec4.x);
	pNew->SetAttribute("Y", vec4.y);
	pNew->SetAttribute("Z", vec4.z);
	pNew->SetAttribute("W", vec4.w);
	GetCurrentXMLNode()->LinkEndChild(pNew);
}

void AXMLArchive::WriteClrValue(const achar* key, const A3DCOLORVALUE& clr)
{
	XMLElement* pNew = m_pXmlDoc->NewElement(key);
	pNew->SetAttribute("a",clr.a);
	pNew->SetAttribute("r",clr.r);
	pNew->SetAttribute("g",clr.g);
	pNew->SetAttribute("b",clr.b);
	GetCurrentXMLNode()->LinkEndChild(pNew);
}

void AXMLArchive::WriteClrValueCap(const achar* key, const A3DCOLORVALUE& clr)
{
	XMLElement* pNew = m_pXmlDoc->NewElement(key);
	pNew->SetAttribute("A", clr.a);
	pNew->SetAttribute("R", clr.r);
	pNew->SetAttribute("G", clr.g);
	pNew->SetAttribute("B", clr.b);
	GetCurrentXMLNode()->LinkEndChild(pNew);
}

void AXMLArchive::WriteStringAttribute(const achar* key, const achar* str)
{
	if (!str)
		str = "";
	m_pCurrentElement->SetAttribute(key, str);
}

void AXMLArchive::WriteInt32Attribute(const achar* key, aint32 i)
{
	m_pCurrentElement->SetAttribute(key, i);
}

void AXMLArchive::WriteUInt32Attribute(const achar* key, auint32 u)
{
	m_pCurrentElement->SetAttribute(key, u);
}

void AXMLArchive::WriteBoolAttribute(const achar* key, abool b)
{
	m_pCurrentElement->SetAttribute(key, b);
}

void AXMLArchive::WriteFloat32Attribute(const achar* key, afloat32 f)
{
	m_pCurrentElement->SetAttribute(key, f);
}

void AXMLArchive::WriteVector3Attribute(const AXVector3& vec3)
{
	m_pCurrentElement->SetAttribute("x", vec3.x);
	m_pCurrentElement->SetAttribute("y", vec3.y);
	m_pCurrentElement->SetAttribute("z", vec3.z);
}

void AXMLArchive::WriteVector3CapAttribute(const AXVector3& vec3)
{
	m_pCurrentElement->SetAttribute("X", vec3.x);
	m_pCurrentElement->SetAttribute("Y", vec3.y);
	m_pCurrentElement->SetAttribute("Z", vec3.z);
}

void AXMLArchive::WriteColorValueAttribute(const A3DCOLORVALUE& color)
{
	m_pCurrentElement->SetAttribute("r", color.r);
	m_pCurrentElement->SetAttribute("g", color.g);
	m_pCurrentElement->SetAttribute("b", color.b);
	m_pCurrentElement->SetAttribute("a", color.a);
}

void AXMLArchive::WriteColorValueCapAttribute(const A3DCOLORVALUE& color)
{
	m_pCurrentElement->SetAttribute("R", color.r);
	m_pCurrentElement->SetAttribute("G", color.g);
	m_pCurrentElement->SetAttribute("B", color.b);
	m_pCurrentElement->SetAttribute("A", color.a);
}

void AXMLArchive::WriteChildStringText(const achar* key, const achar* str)
{
	if (!str)
		str = "";
	XMLElement* pNew = m_pXmlDoc->NewElement(key);
	pNew->SetText(str);
	GetCurrentXMLNode()->LinkEndChild(pNew);
}

void AXMLArchive::WriteChildInt32Text(const achar* key, aint32 i)
{
	XMLElement* pNew = m_pXmlDoc->NewElement(key);
	pNew->SetText(i);
	GetCurrentXMLNode()->LinkEndChild(pNew);
}

void AXMLArchive::WriteChildUInt32Text(const achar* key, auint32 u)
{
	XMLElement* pNew = m_pXmlDoc->NewElement(key);
	pNew->SetText(u);
	GetCurrentXMLNode()->LinkEndChild(pNew);
}

void AXMLArchive::WriteChildBoolText(const achar* key, abool b)
{
	XMLElement* pNew = m_pXmlDoc->NewElement(key);
	pNew->SetText(b);
	GetCurrentXMLNode()->LinkEndChild(pNew);
}

void AXMLArchive::WriteChildFloat32Text(const achar* key, afloat32 f)
{
	XMLElement* pNew = m_pXmlDoc->NewElement(key);
	pNew->SetText(f);
	GetCurrentXMLNode()->LinkEndChild(pNew);
}

void AXMLArchive::WriteStringText(const achar* str)
{
	if (!str)
		str = "";
	m_pCurrentElement->SetText(str);
}

void AXMLArchive::WriteInt32Text(aint32 i)
{
	m_pCurrentElement->SetText(i);
}

void AXMLArchive::WriteUInt32Text(auint32 u)
{
	m_pCurrentElement->SetText(u);
}

void AXMLArchive::WriteBoolText(abool b)
{
	m_pCurrentElement->SetText(b);
}

void AXMLArchive::WriteFloat32Text(afloat32 f)
{
	m_pCurrentElement->SetText(f);
}

AString AXMLArchive::ReadStringAttribute(const achar* key, const achar* defval /*= ""*/)
{
	AString ret;
	const achar* szret = m_pCurrentElement->Attribute(key);
	ret = szret ? szret : defval;
	return ret;
}

aint32 AXMLArchive::ReadInt32Attribute(const achar* key, aint32 defval /*= 0*/)
{
	aint32 ret = defval;
	XMLError error = m_pCurrentElement->QueryIntAttribute(key, &ret);
	if (error != XML_NO_ERROR)
	{
		//g_pAFramework->DevPrintf("AXMLArchive: xml read %s: %s.", key, ConvertErrorCodeToString(error));
	}
	return ret;
}

auint32 AXMLArchive::ReadUInt32Attribute(const achar* key, auint32 defval /*= 0*/)
{
	auint32 ret = defval;
	XMLError error = m_pCurrentElement->QueryUnsignedAttribute(key, &ret);
	if (error != XML_NO_ERROR)
	{
        //g_pAFramework->DevPrintf("AXMLArchive: xml read %s: %s.", key, ConvertErrorCodeToString(error));

	}
	return ret;
}

abool AXMLArchive::ReadBoolAttribute(const achar* key, abool defval /*= afalse*/)
{
	abool ret = defval;
	XMLError error = m_pCurrentElement->QueryBoolAttribute(key, &ret);
	if (error != XML_NO_ERROR)
	{
        //g_pAFramework->DevPrintf("AXMLArchive: xml read %s: %s.", key, ConvertErrorCodeToString(error));

	}
	return ret;
}

afloat32 AXMLArchive::ReadFloat32Attribute(const achar* key, afloat32 defval /*= 0*/)
{
	afloat32 ret = defval;
	XMLError error = m_pCurrentElement->QueryFloatAttribute(key, &ret);
	if (error != XML_NO_ERROR)
	{
        //g_pAFramework->DevPrintf("AXMLArchive: xml read %s: %s.", key, ConvertErrorCodeToString(error));
	}
	return ret;
}

AXVector3 AXMLArchive::ReadVector3Attribute(const AXVector3& defval /*= AXVector3(0.0f)*/)
{
	afloat32 x = 0.0f;
	afloat32 y = 0.0f;
	afloat32 z = 0.0f;
	XMLError errorX = m_pCurrentElement->QueryFloatAttribute("x", &x);
	XMLError errorY = m_pCurrentElement->QueryFloatAttribute("y", &y);
	XMLError errorZ = m_pCurrentElement->QueryFloatAttribute("z", &z);

	if (errorX == XML_NO_ERROR && errorY == XML_NO_ERROR && errorZ == XML_NO_ERROR)
	{
		return AXVector3(x, y, z);
	}
	else
	{
		return defval;
	}
}

AXVector3 AXMLArchive::ReadVector3CapAttribute(const AXVector3& defval /*= AXVector3(0.0f)*/)
{
	afloat32 x = 0.0f;
	afloat32 y = 0.0f;
	afloat32 z = 0.0f;
	XMLError errorX = m_pCurrentElement->QueryFloatAttribute("X", &x);
	XMLError errorY = m_pCurrentElement->QueryFloatAttribute("Y", &y);
	XMLError errorZ = m_pCurrentElement->QueryFloatAttribute("Z", &z);

	if (errorX == XML_NO_ERROR && errorY == XML_NO_ERROR && errorZ == XML_NO_ERROR)
	{
		return AXVector3(x, y, z);
	}
	else
	{
		return defval;
	}
}

A3DCOLORVALUE AXMLArchive::ReadColorValueAttribute(const A3DCOLORVALUE& defval /*= A3DCOLORVALUE(1.0f)*/)
{
	afloat32 r = 0.0f;
	afloat32 g = 0.0f;
	afloat32 b = 0.0f;
	afloat32 a = 0.0f;
	XMLError errorR = m_pCurrentElement->QueryFloatAttribute("r", &r);
	XMLError errorG = m_pCurrentElement->QueryFloatAttribute("g", &g);
	XMLError errorB = m_pCurrentElement->QueryFloatAttribute("b", &b);
	XMLError errorA = m_pCurrentElement->QueryFloatAttribute("a", &a);

	if (errorR == XML_NO_ERROR && errorG == XML_NO_ERROR && errorB == XML_NO_ERROR && errorA == XML_NO_ERROR)
	{
		return A3DCOLORVALUE(r, g, b, a);
	}
	else
	{
		return defval;
	}
}

A3DCOLORVALUE AXMLArchive::ReadColorValueCapAttribute(const A3DCOLORVALUE& defval /*= A3DCOLORVALUE(1.0f)*/)
{
	afloat32 r = 0.0f;
	afloat32 g = 0.0f;
	afloat32 b = 0.0f;
	afloat32 a = 0.0f;
	XMLError errorR = m_pCurrentElement->QueryFloatAttribute("R", &r);
	XMLError errorG = m_pCurrentElement->QueryFloatAttribute("G", &g);
	XMLError errorB = m_pCurrentElement->QueryFloatAttribute("B", &b);
	XMLError errorA = m_pCurrentElement->QueryFloatAttribute("A", &a);

	if (errorR == XML_NO_ERROR && errorG == XML_NO_ERROR && errorB == XML_NO_ERROR && errorA == XML_NO_ERROR)
	{
		return A3DCOLORVALUE(r, g, b, a);
	}
	else
	{
		return defval;
	}
}

AString AXMLArchive::ReadChildStringText(const achar* key, const achar* defval)
{
	tinyxml2::XMLElement* pChildXML = GetCurrentXMLNode()->LastChildElement(key);
	if (pChildXML != nullptr)
	{
		return pChildXML->GetText();
	}
	else
	{
		return defval;
	}
}

aint32 AXMLArchive::ReadChildInt32Text(const achar* key, aint32 defval /*= 0*/)
{
	tinyxml2::XMLElement* pChildXML = GetCurrentXMLNode()->LastChildElement(key);
	if (pChildXML != nullptr)
	{
		aint32 value = 0;
		if (pChildXML->QueryIntText(&value) == XML_NO_ERROR)
		{
			return value;
		}
	}
	return defval;
}

auint32 AXMLArchive::ReadChildUInt32Text(const achar* key, auint32 defval /*= 0*/)
{
	tinyxml2::XMLElement* pChildXML = GetCurrentXMLNode()->LastChildElement(key);
	if (pChildXML != nullptr)
	{
		auint32 value = 0;
		if (pChildXML->QueryUnsignedText(&value) == XML_NO_ERROR)
		{
			return value;
		}
	}
	return defval;
}

abool AXMLArchive::ReadChildBoolText(const achar* key, abool defval /*= afalse*/)
{
	tinyxml2::XMLElement* pChildXML = GetCurrentXMLNode()->LastChildElement(key);
	if (pChildXML != nullptr)
	{
		abool value = 0;
		if (pChildXML->QueryBoolText(&value) == XML_NO_ERROR)
		{
			return value;
		}
	}
	return defval;
}

afloat32 AXMLArchive::ReadChildFloat32Text(const achar* key, afloat32 defval /*= 0*/)
{
	tinyxml2::XMLElement* pChildXML = GetCurrentXMLNode()->LastChildElement(key);
	if (pChildXML != nullptr)
	{
		afloat32 value = 0;
		if (pChildXML->QueryFloatText(&value) == XML_NO_ERROR)
		{
			return value;
		}
	}
	return defval;
}

AString AXMLArchive::ReadStringText()
{
	return m_pCurrentElement->GetText();
}

aint32 AXMLArchive::ReadInt32Text(aint32 defval /*= 0*/)
{
	aint32 value = 0;
	XMLError error = m_pCurrentElement->QueryIntText(&value);
	if (error == XML_NO_ERROR)
	{
		return value;
	}
	else
	{
		return defval;
	}
}

auint32 AXMLArchive::ReadUInt32Text(auint32 defval /*= 0*/)
{
	auint32 value = 0;
	XMLError error = m_pCurrentElement->QueryUnsignedText(&value);
	if (error == XML_NO_ERROR)
	{
		return value;
	}
	else
	{
		return defval;
	}
}

abool AXMLArchive::ReadBoolText(abool defval /*= afalse*/)
{
	abool value = 0;
	XMLError error = m_pCurrentElement->QueryBoolText(&value);
	if (error == XML_NO_ERROR)
	{
		return value;
	}
	else
	{
		return defval;
	}
}

afloat32 AXMLArchive::ReadFloat32Text(afloat32 defval /*= 0*/)
{
	afloat32 value = 0;
	XMLError error = m_pCurrentElement->QueryFloatText(&value);
	if (error == XML_NO_ERROR)
	{
		return value;
	}
	else
	{
		return defval;
	}
}

tinyxml2::XMLElement* AXMLArchive::BackToParentElement()
{
    m_pCurrentElement = GetCurrentXMLNode()->Parent()->ToElement();
    return m_pCurrentElement;
}
/*
AXQuaternion AXMLArchive::ReadQuaternion(const achar* key, const AXQuaternion& defval)
{
    AXQuaternion q = defval;
    XMLElement* pChild = GetCurrentXMLNode()->FirstChildElement(key);
    if (pChild)
    {
        q.x = pChild->FloatAttribute("x");
        q.y = pChild->FloatAttribute("y");
        q.z = pChild->FloatAttribute("z");
        q.w = pChild->FloatAttribute("w");
    }

    return q;
}

void AXMLArchive::WriteQuaternion(const achar* key, const AXQuaternion& qu)
{
    XMLElement* pNew = m_pXmlDoc->NewElement(key);
    pNew->SetAttribute("x", qu.x);
    pNew->SetAttribute("y", qu.y);
    pNew->SetAttribute("z", qu.z);
    pNew->SetAttribute("w", qu.w);
    GetCurrentXMLNode()->LinkEndChild(pNew);
}
*/
AXMatrix4 AXMLArchive::ReadMatrix4(const achar* key, const AXMatrix4& defval)
{
	AXMatrix4 ret= defval;
	XMLElement* pChild = GetCurrentXMLNode()->FirstChildElement(key);
	if (pChild)
	{
		tinyxml2::XMLElement *pRowElem = pChild->FirstChildElement();
		for (aint32 nRow = 0; nRow < 4; nRow++)
		{
			ret.m[nRow][0] = pRowElem->FloatAttribute("Col_0");
			ret.m[nRow][1] = pRowElem->FloatAttribute("Col_1");
			ret.m[nRow][2] = pRowElem->FloatAttribute("Col_2");
			ret.m[nRow][3] = pRowElem->FloatAttribute("Col_3");

			pRowElem = pRowElem->NextSiblingElement();
		}
	}

	return ret;
}

void AXMLArchive::WriteMatrix4(const achar* key, const AXMatrix4& mat4)
{
	XMLElement* pNew = m_pXmlDoc->NewElement(key);
	
	for (aint32 nRow = 0; nRow < 4; nRow++)
	{
		XMLElement* pRowElement = m_pXmlDoc->NewElement("Row");
		pRowElement->SetAttribute("Col_0", mat4.m[nRow][0]);
		pRowElement->SetAttribute("Col_1", mat4.m[nRow][1]);
		pRowElement->SetAttribute("Col_2", mat4.m[nRow][2]);
		pRowElement->SetAttribute("Col_3", mat4.m[nRow][3]);

		pNew->LinkEndChild(pRowElement);
	}

	GetCurrentXMLNode()->LinkEndChild(pNew);
}
/*
AXPackedNormal AXMLArchive::ReadPackedNormal(const achar* key, const AXPackedNormal defval)
{
	AXPackedNormal ret = defval;
	XMLElement *pChild = GetCurrentXMLNode()->FirstChildElement(key);
	if (pChild)
	{
		ret.x = pChild->UnsignedAttribute("x");
		ret.y = pChild->UnsignedAttribute("y");
		ret.z = pChild->UnsignedAttribute("z");
		ret.w = pChild->UnsignedAttribute("w");
	}

	return ret;
}

void AXMLArchive::WritePackedNormal(const achar* key, const AXPackedNormal& packedNorm)
{
	XMLElement *pNew = m_pXmlDoc->NewElement(key);

	pNew->SetAttribute("x", (unsigned int)packedNorm.x);
	pNew->SetAttribute("y", (unsigned int)packedNorm.y);
	pNew->SetAttribute("z", (unsigned int)packedNorm.z);
	pNew->SetAttribute("w", (unsigned int)packedNorm.w);

	GetCurrentXMLNode()->LinkEndChild(pNew);
}
*/
const achar* AXMLArchive::ConvertErrorCodeToString(tinyxml2::XMLError error)
{
	switch (error)
	{
	case tinyxml2::XML_SUCCESS:
		return "XML_SUCCESS";;
	case tinyxml2::XML_NO_ATTRIBUTE:
		return "XML_NO_ATTRIBUTE";
	case tinyxml2::XML_WRONG_ATTRIBUTE_TYPE:
		return "XML_WRONG_ATTRIBUTE_TYPE";
	case tinyxml2::XML_ERROR_FILE_NOT_FOUND:
		return "XML_ERROR_FILE_NOT_FOUND";
	case tinyxml2::XML_ERROR_FILE_COULD_NOT_BE_OPENED:
		return "XML_ERROR_FILE_COULD_NOT_BE_OPENED";
	case tinyxml2::XML_ERROR_FILE_READ_ERROR:
		return "XML_ERROR_FILE_READ_ERROR";
	case tinyxml2::XML_ERROR_ELEMENT_MISMATCH:
		return "XML_ERROR_ELEMENT_MISMATCH";
	case tinyxml2::XML_ERROR_PARSING_ELEMENT:
		return "XML_ERROR_PARSING_ELEMENT";
	case tinyxml2::XML_ERROR_PARSING_ATTRIBUTE:
		return "XML_ERROR_PARSING_ATTRIBUTE";
	case tinyxml2::XML_ERROR_IDENTIFYING_TAG:
		return "XML_ERROR_IDENTIFYING_TAG";
	case tinyxml2::XML_ERROR_PARSING_TEXT:
		return "XML_ERROR_PARSING_TEXT";
	case tinyxml2::XML_ERROR_PARSING_CDATA:
		return "XML_ERROR_PARSING_CDATA";
	case tinyxml2::XML_ERROR_PARSING_COMMENT:
		return "XML_ERROR_PARSING_COMMENT";
	case tinyxml2::XML_ERROR_PARSING_DECLARATION:
		return "XML_ERROR_PARSING_DECLARATION";
	case tinyxml2::XML_ERROR_PARSING_UNKNOWN:
		return "XML_ERROR_PARSING_UNKNOWN";
	case tinyxml2::XML_ERROR_EMPTY_DOCUMENT:
		return "XML_ERROR_EMPTY_DOCUMENT";
	case tinyxml2::XML_ERROR_MISMATCHED_ELEMENT:
		return "XML_ERROR_MISMATCHED_ELEMENT";
	case tinyxml2::XML_ERROR_PARSING:
		return "XML_ERROR_PARSING";
	case tinyxml2::XML_CAN_NOT_CONVERT_TEXT:
		return "XML_CAN_NOT_CONVERT_TEXT";
	case tinyxml2::XML_NO_TEXT_NODE:
		return "XML_NO_TEXT_NODE";
	case tinyxml2::XML_ERROR_COUNT:
		return "XML_ERROR_COUNT";
	default:
		return "Unknown error code";
	}
}