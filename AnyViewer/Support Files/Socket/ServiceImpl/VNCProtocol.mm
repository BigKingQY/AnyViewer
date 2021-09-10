//
//  VNCProtocol.m
//  AnyViewer
//
//  Created by Aomei on 2021/9/9.
//

#include "VNCProtocol.h"


namespace VNCP
{
    REGISTER_CLASS(CDevMode)
        REGISTER_CLASS(COSInfo)

    const STATUS_CODE_DESC StatusCodeDescItem[] =
    {
        { ES_SUCCESS, "≥…π¶"},

        { ES_UNKNOWN_ERR, "Œ¥÷™¥ÌŒÛ" }
    };

    std::string GetStatusCodeDesc(VNCP_EXCEPTION_STATUS nStatusCode)
    {
        std::string strResult("Œ¥÷™¥ÌŒÛ");
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
    /// ‘ˆº”“ª∏ˆ–¬µƒœ‘ æƒ£ Ω£®∏√∫Ø ˝ª·»•µÙ÷ÿ∏¥µƒœ‘ æƒ£ Ω£©
    /// </summary>
    /// <param name="nWidth">øÌ</param>
    /// <param name="nHeight">∏ﬂ</param>
    /// <param name="nBitsPerPel">√ø∏ˆœÒÀÿ∫√∂‡Œª</param>
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
