///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  
/// @brief VNC协议实现
/// 
/// @author 黄丽云
/// @version 1.0
/// @date        2020-11-26
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "VNCProtocol.h"


namespace VNCP
{
	REGISTER_CLASS(CDevMode)
		REGISTER_CLASS(COSInfo)

	const STATUS_CODE_DESC StatusCodeDescItem[] =
	{
		{ ES_SUCCESS, "成功"},

		{ ES_UNKNOWN_ERR, "未知错误" }
	};

	std::string GetStatusCodeDesc(VNCP_EXCEPTION_STATUS nStatusCode)
	{
		std::string strResult("未知错误");
		const int nCount = sizeof(StatusCodeDescItem) / sizeof(StatusCodeDescItem[0]);

		for (int i = 0; i < nCount; ++i)
		{
			if (nStatusCode == VNCP_EXCEPTION_STATUS(StatusCodeDescItem[i].m_nStatusCode))
			{
				strResult = StatusCodeDescItem[i].m_strDesc;
				break;
			}
		}

		return strResult;
	}	


	/*----------------------------------CDevMode------------------------------------------------*/
	CDevMode::CDevMode(const U32 nWidth/* = 0*/, const U32 nHeight /*= 0*/, const U32 nBitsPerPel/* = 0*/)
		: m_nWidth(nWidth)
		, m_nHeight(nHeight)
		, m_nBitsPerPel(nBitsPerPel)
	{
	}

	CDevMode::CDevMode(const CDevMode & refSrc)
		: m_nWidth(refSrc.m_nWidth)
		, m_nHeight(refSrc.m_nHeight)
		, m_nBitsPerPel(refSrc.m_nBitsPerPel)
	{
	}

	bool CDevMode::operator==(const CDevMode & refSrc)
	{
		return (refSrc.m_nWidth == m_nWidth
			&& refSrc.m_nHeight == m_nHeight
			&& refSrc.m_nBitsPerPel == m_nBitsPerPel);
	}

	void CDevMode::Serialize(CJSONSerializer& refJsonSerializer) const
	{
		CSerializeEntry::Serialize(refJsonSerializer);

		refJsonSerializer.Serialize("m11", m_nWidth);
		refJsonSerializer.Serialize("m12", m_nHeight);
		refJsonSerializer.Serialize("m13", m_nBitsPerPel);
	}

	void CDevMode::DeSerialize(CJSONSerializer& refJsonSerializer)
	{
		CSerializeEntry::DeSerialize(refJsonSerializer);

		refJsonSerializer.DeSerialize("m11", m_nWidth);
		refJsonSerializer.DeSerialize("m12", m_nHeight);
		refJsonSerializer.DeSerialize("m13", m_nBitsPerPel);
	}

	/*----------------------------------CDevMode------------------------------------------------*/

	// ********************************************************************************
	/// <summary>
	/// 增加一个新的显示模式（该函数会去掉重复的显示模式）
	/// </summary>
	/// <param name="nWidth">宽</param>
	/// <param name="nHeight">高</param>
	/// <param name="nBitsPerPel">每个像素好多位</param>
	/// <created>Andy,2021/7/14</created>
	/// <changed>Andy,2021/7/14</changed>
	// ********************************************************************************
	void COSInfo::AppendDevMode(const U32 nWidth , const U32 nHeight , const U32 nBitsPerPel)
	{
		bool bExist = false;

		for (auto pDevModeEntry :m_arrDevModes)
		{
			bExist = (pDevModeEntry->GetWidth() == nWidth
				&& pDevModeEntry->GetHeight() == nHeight
				&& pDevModeEntry->GetBitsPerPel() == nBitsPerPel);

			if (bExist)
			{
				break;
			}
		}

		if (!bExist)
		{
			CDevModePtr pDevMode = std::make_shared<CDevMode>(nWidth, nHeight, nBitsPerPel);

			m_arrDevModes.push_back(pDevMode);
		}
	}

	void COSInfo::Serialize(CJSONSerializer& refJsonSerializer) const
	{
		CSerializeEntry::Serialize(refJsonSerializer);

		refJsonSerializer.Serialize("m11", m_nOS);
		refJsonSerializer.Serialize("m12", m_arrDevModes);
		refJsonSerializer.Serialize("m13", m_objCurDevMode);
		refJsonSerializer.Serialize("m14", m_nVersion);
		
	}

	void COSInfo::DeSerialize(CJSONSerializer& refJsonSerializer)
	{
		CSerializeEntry::DeSerialize(refJsonSerializer);

		refJsonSerializer.DeSerialize("m11", m_nOS);
		refJsonSerializer.DeSerialize("m12", m_arrDevModes);
		refJsonSerializer.DeSerialize("m13", m_objCurDevMode);
		refJsonSerializer.DeSerialize("m14", m_nVersion);
	}
}


