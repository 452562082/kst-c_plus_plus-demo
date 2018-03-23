// asv_rpc_demo.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "asv_rpc.h"
#include "waveIO.h"

#pragma comment (lib,"kst_sdk.lib")

int _tmain(int argc, _TCHAR* argv[])
{
	AsvRpcEngine engine("47.104.11.15",9191,1000);
	//��ӽڵ�
	if(engine.KvpInsertNode("testnode")){
		return -1;
	}

	//˵����ע��
	const char* filename = "./gabzh.wav";
	short *utt = nullptr;
	unsigned long utt_size = 0;
	if(!WaveLoad(filename,utt,utt_size)){
		std::cout << "�����ļ���ʽ����" << std::endl;
		return -1;
	}

	std::vector<short> utt_arr;
	for(int i = 0;i < utt_size;i++) {
		utt_arr.push_back(utt[i]);
	}
	_Rpc_ModelInfo* _mdlinfo= engine.KvpRegisterSpeakerByStream(utt_arr, 22050, "testnode", "djsayrwq2ry7dehswjd8");
	if (_mdlinfo->RetCode != 0){
		std::cout << "Failed: " << _mdlinfo->ErrCode << "\t" << _mdlinfo->ErrMsg << std::endl;
	}
	engine.Delete_Asv_Type(_RPC_ModelInfo, _mdlinfo);

	//˵����ʶ��
	vector<string> vp_node_arr;
	vp_node_arr.push_back("testnode");
	_Rpc_TopSpeakerInfo *rt = engine.KvpIdentifyTopSpeakerByStream(utt_arr, 22050, vp_node_arr, 1, 1, 0);
	if(rt->RetCode != 0){
		std::cout << "Failed: " << rt->ErrCode << "\t" << rt->ErrMsg << std::endl;
	}else{
		for(int i = 0;i < rt->Scores.size();++i){
			std::cout << "Spkid:" << rt->Scores[i].Spkid << " Scores:" << rt->Scores[i].Score << std::endl; 
		}
	}
	engine.Delete_Asv_Type(_RPC_TopSpeakerInfo, rt);
	delete[] utt;

	//ɾ��˵����
	engine.KvpModelRemoveBySpkid("testnode", "djsayrwq2ry7dehswjd8");

	//��ȡ��Ȩ��Ϣ
	_Rpc_LicenceInfo* ret = engine.KvpGetLicenceInfo();
	if(ret->RetCode == 0){
		std::cout << "dateStr:" << ret->dateStr << std::endl;
		std::cout << "fingerprint:" << ret->fingerprint << std::endl;
	}
	engine.Delete_Asv_Type(_RPC_LicenceInfo,ret);

	system("PAUSE"); 
	return 0;
}