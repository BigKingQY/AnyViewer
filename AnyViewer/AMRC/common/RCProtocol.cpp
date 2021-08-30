/******************************************************************************
*
* SaxListCtrl
* Version
* Copyright (C) 2007 Ax3soft Co., Ltd.
* All Rights Reserved
* $Header: /cvsdata/vc/Common/GeneralFunctions.h,v 1.17 2014/12/13 01:42:27 administrator Exp $
*
*******************************************************************************
*
* Description:General functions define,Dependent MFC library
******************************************************************************/
#include "stdafx.h"
#include "RCProtocol.h"
#include <sstream>

namespace RCP
{
	REGISTER_CLASS(CRelaySererStatus)
		REGISTER_CLASS(CRelaySererInfo)
		REGISTER_CLASS(CRCClientInfo)
		REGISTER_CLASS(CConnectionRequest)
		REGISTER_CLASS(CAuthenticationRequest)
		REGISTER_CLASS(CReadyConnRequest)
		REGISTER_CLASS(CNewRelayTaskRequest)

		const STATUS_CODE_DESC StatusCodeDescItem[] =
	{
		{ ES_SUCCESS, "成功"},
		{ ES_INVALID_ACCOUNT, "用户名或密码错误"},
		{ ES_CREATE_ACCOUNT_FAILED, "创建账号失败"},
		{ ES_INVALID_NAME, "用户名或密码错误"} ,
		{ ES_EXIST_NAME, "账号已存在" },
		{ ES_INVALID_PASSWD, "用户名或密码错误"},
		{ ES_REPEAT_LOGIN, "重复登录" },
		{ ES_TIME_OUT, "等待超时"},
		{ ES_NOT_LOGGED_IN,"未登录" },
		{ ES_OPEN_FILE_UNSUCCESSFULLY, "打开文件失败" },
		{ ES_SERVICE_ERR, "服务器错误" },
		{ ES_INVALID_SN ,"无效激活码"},
		{ ES_REACH_AUTHORIZATION_LIMIT,"授权数已经满,无法在激活"},
		{ ES_SEND_FAILED, "发送数据失败" },
		{ ES_CREATE_CUSTORM_UNSUCCESSFULLY, "Fail to create a new custorm record!" },
		{ ES_NOT_FOUND_CUSTORM, "Customer information is not found!" },			
		{ ES_UNKNOWN_ERR, "未知错误" }
	};

	std::string GetStatusCodeDesc(EXCEPTION_STATUS nStatusCode)
	{
		std::string strResult("未知错误");
		const int nCount = sizeof(StatusCodeDescItem) / sizeof(StatusCodeDescItem[0]);

		for (int i = 0; i < nCount; ++i)
		{
			if (nStatusCode == EXCEPTION_STATUS(StatusCodeDescItem[i].m_nStatusCode))
			{
				strResult = StatusCodeDescItem[i].m_strDesc;
				break;
			}
		}

		return strResult;
	}

	/*--------------------CRelaySererStatus------------------------------------------*/

	void CRelaySererStatus::Serialize(CJSONSerializer& refJsonSerializer) const
	{
		CSerializeEntry::Serialize(refJsonSerializer);

		refJsonSerializer.Serialize("m11", m_nCPUOccupy);
		refJsonSerializer.Serialize("m12", m_nMemTotal);
		refJsonSerializer.Serialize("m13", m_nMemFree);
		refJsonSerializer.Serialize("m14", m_nFreeBand);
		refJsonSerializer.Serialize("m15", m_nSessions);
	}

	void CRelaySererStatus::DeSerialize(CJSONSerializer& refJsonSerializer)
	{
		CSerializeEntry::DeSerialize(refJsonSerializer);

		refJsonSerializer.DeSerialize("m11", m_nCPUOccupy);
		refJsonSerializer.DeSerialize("m12", m_nMemTotal);
		refJsonSerializer.DeSerialize("m13", m_nMemFree);
		refJsonSerializer.DeSerialize("m14", m_nFreeBand);
		refJsonSerializer.DeSerialize("m15", m_nSessions);
	}

	/*--------------------CRelaySererInfo------------------------------------------*/

	void CRelaySererInfo::Serialize(CJSONSerializer& refJsonSerializer) const
	{
		CSerializeEntry::Serialize(refJsonSerializer);

		refJsonSerializer.Serialize("m11", m_strName);
		refJsonSerializer.Serialize("m12", m_objStatus);
		refJsonSerializer.Serialize("m13", m_strRegion);
		refJsonSerializer.Serialize("m14", m_nPort);
		refJsonSerializer.Serialize("m15",m_strGUID);
		
	}

	void CRelaySererInfo::DeSerialize(CJSONSerializer& refJsonSerializer)
	{
		CSerializeEntry::DeSerialize(refJsonSerializer);

		refJsonSerializer.DeSerialize("m11", m_strName);
		refJsonSerializer.DeSerialize("m12", m_objStatus);
		refJsonSerializer.DeSerialize("m13", m_strRegion);
		refJsonSerializer.DeSerialize("m14", m_nPort);
		refJsonSerializer.DeSerialize("m15", m_strGUID);
	}

	/*--------------------CRCClientInfo------------------------------------------*/

	// ********************************************************************************
/// <summary>
/// 创建一个key
/// </summary>
/// <returns>返回创建的key</returns>
/// <created>Andy,2020/10/22</created>
/// <changed>Andy,2020/10/22</changed>
// ********************************************************************************
	std::string CRCClientInfo::CreateKey()
	{
		std::ostringstream  ossKey;

		ossKey << GetMachineCode() << GetMac();

		return ossKey.str();
	}



	void CRCClientInfo::Serialize(CJSONSerializer& refJsonSerializer) const
	{
		CSerializeEntry::Serialize(refJsonSerializer);

		refJsonSerializer.Serialize("m11", m_strIPs);
		refJsonSerializer.Serialize("m12", m_strIPMasks);
		refJsonSerializer.Serialize("m13", m_strMachineCode);
		refJsonSerializer.Serialize("m14", m_nMac);
		refJsonSerializer.Serialize("m15", m_nAuthMethodMask);
		refJsonSerializer.Serialize("m16", m_strPwd);
		refJsonSerializer.Serialize("m17", m_strTmpPwd);
		refJsonSerializer.Serialize("m18", m_nDeviceID);
		refJsonSerializer.Serialize("m19", m_strRouteIPs);
		refJsonSerializer.Serialize("m20", m_nProtocolVer);
		refJsonSerializer.Serialize("m21", m_nRFBPort1);
		refJsonSerializer.Serialize("m22", m_nRFBPort2);
		refJsonSerializer.Serialize("m23", m_strNickName);
		refJsonSerializer.Serialize("m24", m_nAppVer);
		refJsonSerializer.Serialize("m25", m_strRegion);
	}

	void CRCClientInfo::DeSerialize(CJSONSerializer& refJsonSerializer)
	{
		CSerializeEntry::DeSerialize(refJsonSerializer);

		refJsonSerializer.DeSerialize("m11", m_strIPs);
		refJsonSerializer.DeSerialize("m12", m_strIPMasks);
		refJsonSerializer.DeSerialize("m13", m_strMachineCode);
		refJsonSerializer.DeSerialize("m14", m_nMac);
		refJsonSerializer.DeSerialize("m15", m_nAuthMethodMask);
		refJsonSerializer.DeSerialize("m16", m_strPwd);
		refJsonSerializer.DeSerialize("m17", m_strTmpPwd);
		refJsonSerializer.DeSerialize("m18", m_nDeviceID);
		refJsonSerializer.DeSerialize("m19", m_strRouteIPs);
		refJsonSerializer.DeSerialize("m20", m_nProtocolVer);
		refJsonSerializer.DeSerialize("m21", m_nRFBPort1);
		refJsonSerializer.DeSerialize("m22", m_nRFBPort2);
		refJsonSerializer.DeSerialize("m23", m_strNickName);
		refJsonSerializer.DeSerialize("m24", m_nAppVer);
		refJsonSerializer.DeSerialize("m25", m_strRegion);
	}

	/*--------------------------------CConnectionRequest----------------------------------------*/
	void CConnectionRequest::Serialize(CJSONSerializer& refJsonSerializer) const
	{
		CSerializeEntry::Serialize(refJsonSerializer);

		refJsonSerializer.Serialize("m11", m_strNickName);
		refJsonSerializer.Serialize("m12", m_nSrcID);
		refJsonSerializer.Serialize("m13", m_nDstID);
		refJsonSerializer.Serialize("m14", m_nAuthMothed);
		refJsonSerializer.Serialize("m15", m_nRejectMode);
		
	}

	void CConnectionRequest::DeSerialize(CJSONSerializer& refJsonSerializer)
	{
		CSerializeEntry::DeSerialize(refJsonSerializer);

		refJsonSerializer.DeSerialize("m11", m_strNickName);
		refJsonSerializer.DeSerialize("m12", m_nSrcID);
		refJsonSerializer.DeSerialize("m13", m_nDstID);
		refJsonSerializer.DeSerialize("m14", m_nAuthMothed);
		refJsonSerializer.DeSerialize("m15", m_nRejectMode);
	}

	/*--------------------------------CConnectionRequest----------------------------------------*/
	void CAuthenticationRequest::Serialize(CJSONSerializer& refJsonSerializer) const
	{
		CSerializeEntry::Serialize(refJsonSerializer);

		refJsonSerializer.Serialize("m11", m_nDstID);
		refJsonSerializer.Serialize("m12", m_strPwd);
	}

	void CAuthenticationRequest::DeSerialize(CJSONSerializer& refJsonSerializer)
	{
		CSerializeEntry::DeSerialize(refJsonSerializer);

		refJsonSerializer.DeSerialize("m11", m_nDstID);
		refJsonSerializer.DeSerialize("m12", m_strPwd);
	}

	/*--------------------------------CReadyConnRequest----------------------------------------*/
	void CReadyConnRequest::Serialize(CJSONSerializer& refJsonSerializer) const
	{
		CSerializeEntry::Serialize(refJsonSerializer);

		refJsonSerializer.Serialize("m11", m_nSessionID);
		refJsonSerializer.Serialize("m12", m_nPeerID);
		refJsonSerializer.Serialize("m13", m_nPeerIP);
		refJsonSerializer.Serialize("m14", m_nCommunicationMode);
		refJsonSerializer.Serialize("m15", m_nRoleType);
		refJsonSerializer.Serialize("m16", m_nProtocolVer);
		refJsonSerializer.Serialize("m17", m_nPeerPort);
		refJsonSerializer.Serialize("m18", m_strNickName);
	}

	void CReadyConnRequest::DeSerialize(CJSONSerializer& refJsonSerializer)
	{
		CSerializeEntry::DeSerialize(refJsonSerializer);

		refJsonSerializer.DeSerialize("m11", m_nSessionID);
		refJsonSerializer.DeSerialize("m12", m_nPeerID);
		refJsonSerializer.DeSerialize("m13", m_nPeerIP);
		refJsonSerializer.DeSerialize("m14", m_nCommunicationMode);
		refJsonSerializer.DeSerialize("m15", m_nRoleType);
		refJsonSerializer.DeSerialize("m16", m_nProtocolVer);
		refJsonSerializer.DeSerialize("m17", m_nPeerPort);
		refJsonSerializer.DeSerialize("m18", m_strNickName);
	}

	/*--------------------------------CNewRelayTaskRequest----------------------------------------*/
	void CNewRelayTaskRequest::Serialize(CJSONSerializer& refJsonSerializer) const
	{
		CSerializeEntry::Serialize(refJsonSerializer);

		refJsonSerializer.Serialize("m11", m_nSessionID);
		refJsonSerializer.Serialize("m12", m_nSrcID);
		refJsonSerializer.Serialize("m13", m_nDstID);
		refJsonSerializer.Serialize("m14", m_nRelaySvrID);
	}

	void CNewRelayTaskRequest::DeSerialize(CJSONSerializer& refJsonSerializer)
	{
		CSerializeEntry::DeSerialize(refJsonSerializer);

		refJsonSerializer.DeSerialize("m11", m_nSessionID);
		refJsonSerializer.DeSerialize("m12", m_nSrcID);
		refJsonSerializer.DeSerialize("m13", m_nDstID);
		refJsonSerializer.DeSerialize("m14", m_nRelaySvrID);
	}
}


