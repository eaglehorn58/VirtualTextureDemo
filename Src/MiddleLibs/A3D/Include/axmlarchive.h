//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#ifndef __A_XML_ARCHIVE__
#define __A_XML_ARCHIVE__

#include "AXTypes.h"
//#include "AXPackedNormal.h"
#include "tinyxml2.h"
#include "AMemory.h"
#include "AString.h"
#include <vector>

class AXMLArchive
{
public:

	using XMLDocument = tinyxml2::XMLDocument;
	using XMLElement = tinyxml2::XMLElement;
	using XMLNode = tinyxml2::XMLNode;
	using XMLError = tinyxml2::XMLError;
	using XMLElementVector = std::vector<XMLElement*>;

	AXMLArchive(XMLDocument* pXmlDoc, XMLElement* pRoot, abool bLoad) 
		:m_pXmlDoc(pXmlDoc), m_bLoad(bLoad), m_pCurrentElement(pRoot) {};
	virtual ~AXMLArchive(){};
	
	abool IsLoading(){ return m_bLoad; }
	void WriteString(const achar* key, const achar* str);
	void WriteInt32(const achar* key, aint32 i);
	void WriteUInt32(const achar* key, auint32 u);
	void WriteBool(const achar* key, abool b);
	void WriteFloat32(const achar* key, afloat32 f);
	void WriteVector3(const achar* key, const AXVector3& vec3);
	void WriteVector3Cap(const achar* key, const AXVector3& vec3);
	void WriteVector2(const achar* key, const AXVector2& vec2);
	void WriteVector2Cap(const achar* key, const AXVector2& vec2);
	void WriteVector4(const achar* key, const AXVector4& vec4);
	void WriteVector4Cap(const achar* key, const AXVector4& vec4);
	void WriteClrValue(const achar* key, const A3DCOLORVALUE& clr);
	void WriteClrValueCap(const achar* key, const A3DCOLORVALUE& clr);
//    void WriteQuaternion(const achar* key, const AXQuaternion& qu);
	void WriteMatrix4(const achar* key, const AXMatrix4& mat4);
//	void WritePackedNormal(const achar* key, const AXPackedNormal& packedNorm);

	void WriteStringAttribute(const achar* key, const achar* str);
	void WriteInt32Attribute(const achar* key, aint32 i);
	void WriteUInt32Attribute(const achar* key, auint32 u);
	void WriteBoolAttribute(const achar* key, abool b);
	void WriteFloat32Attribute(const achar* key, afloat32 f);

	void WriteVector3Attribute(const AXVector3& vec3);
	void WriteVector3CapAttribute(const AXVector3& vec3);
	void WriteColorValueAttribute(const A3DCOLORVALUE& color);
	void WriteColorValueCapAttribute(const A3DCOLORVALUE& color);

	void WriteChildStringText(const achar* key, const achar* str);
	void WriteChildInt32Text(const achar* key, aint32 i);
	void WriteChildUInt32Text(const achar* key, auint32 u);
	void WriteChildBoolText(const achar* key, abool b);
	void WriteChildFloat32Text(const achar* key, afloat32 f);

	void WriteStringText(const achar* str);
	void WriteInt32Text(aint32 i);
	void WriteUInt32Text(auint32 u);
	void WriteBoolText(abool b);
	void WriteFloat32Text(afloat32 f);

	AString ReadString(const achar* key, const achar* defval = "");
	aint32 ReadInt32(const achar* key, aint32 defval = 0);
	auint32 ReadUInt32(const achar* key, auint32 defval = 0);
	abool ReadBool(const achar* key, abool defval = afalse);
	afloat32 ReadFloat32(const achar* key, afloat32 defval = 0);
	AXVector3 ReadVector3(const achar* key, const AXVector3& defval = AXVector3(0.0f));
	AXVector3 ReadVector3Cap(const achar* key, const AXVector3& defval = AXVector3(0.0f));
	AXVector2 ReadVector2(const achar* key, const AXVector2& defval = AXVector2(0.0f));
	AXVector2 ReadVector2Cap(const achar* key, const AXVector2& defval = AXVector2(0.0f));
	AXVector4 ReadVector4(const achar* key, const AXVector4& defval = AXVector4(0.0f));
	AXVector4 ReadVector4Cap(const achar* key, const AXVector4& defval = AXVector4(0.0f));
	A3DCOLORVALUE ReadClrValue(const achar* key, const A3DCOLORVALUE& defval = A3DCOLORVALUE(1.0f));
	A3DCOLORVALUE ReadColorValueCap(const achar* key, const A3DCOLORVALUE& defval = A3DCOLORVALUE(1.0f));
//    AXQuaternion ReadQuaternion(const achar* key, const AXQuaternion& defval = AXQuaternion(1.0f, 0, 0, 0));
	AXMatrix4 ReadMatrix4(const achar* key, const AXMatrix4& defval = AXMatrix4());
//	AXPackedNormal ReadPackedNormal(const achar* key, const AXPackedNormal defval = AXPackedNormal(0));

	AString ReadStringAttribute(const achar* key, const achar* defval = "");
	aint32 ReadInt32Attribute(const achar* key, aint32 defval = 0);
	auint32 ReadUInt32Attribute(const achar* key, auint32 defval = 0);
	abool ReadBoolAttribute(const achar* key, abool defval = afalse);
	afloat32 ReadFloat32Attribute(const achar* key, afloat32 defval = 0);

	AXVector3 ReadVector3Attribute(const AXVector3& defval = AXVector3(0.0f));
	AXVector3 ReadVector3CapAttribute(const AXVector3& defval = AXVector3(0.0f));
	A3DCOLORVALUE ReadColorValueAttribute(const A3DCOLORVALUE& defval = A3DCOLORVALUE(1.0f));
	A3DCOLORVALUE ReadColorValueCapAttribute(const A3DCOLORVALUE& defval = A3DCOLORVALUE(1.0f));

	AString ReadChildStringText(const achar* key, const achar* defval = "");
	aint32 ReadChildInt32Text(const achar* key, aint32 defval = 0);
	auint32 ReadChildUInt32Text(const achar* key, auint32 defval = 0);
	abool ReadChildBoolText(const achar* key, abool defval = afalse);
	afloat32 ReadChildFloat32Text(const achar* key, afloat32 defval = 0);

	AString ReadStringText();
	aint32 ReadInt32Text(aint32 defval = 0);
	auint32 ReadUInt32Text(auint32 defval = 0);
	abool ReadBoolText(abool defval = afalse);
	afloat32 ReadFloat32Text(afloat32 defval = 0);

	XMLDocument* GetXMLDocument() { return m_pXmlDoc; }

	XMLElement* NewElement(const achar* name);
	XMLElement* NewElementAsChild(const achar* name);
	// 获得第一个（或者最后一个）叫 name 的 ChildElement
	XMLElement* GetChildElement(const achar* name, abool bFirst = afalse);
	XMLElementVector GetChildElements(const achar* name);

	XMLElement* GetCurrentXMLElement() const {return m_pCurrentElement;}
	void SetCurrentXMLElement(XMLElement* p) {m_pCurrentElement = p;}

	XMLNode* GetCurrentXMLNode() const {return m_pCurrentElement? (XMLNode*)m_pCurrentElement : (XMLNode*)m_pXmlDoc;}

	XMLElement* BackToParentElement();

	const achar * ConvertErrorCodeToString(XMLError error);

#pragma region Deprecated

	// 同 GetCurrentXMLElement，由于命名有些混淆，现标记为 Deprecated，为兼容老代码保留，以后不要使用了
	XMLElement* GetXMLElementParent(){ return GetCurrentXMLElement();}
	// 同 SetCurrentXMLElement，由于命名有些混淆，现标记为 Deprecated，为兼容老代码保留，以后不要使用了
	void SetXMLElementParent(XMLElement* pParent){SetCurrentXMLElement(pParent);}

#pragma endregion

protected:
	abool m_bLoad;
	XMLDocument* m_pXmlDoc;
	XMLElement* m_pCurrentElement;
};

#endif

