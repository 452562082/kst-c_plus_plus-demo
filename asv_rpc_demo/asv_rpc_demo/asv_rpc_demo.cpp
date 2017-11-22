// asv_rpc_demo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <vector>
#include "test_rpc.h"
#include "waveIO.h"

bool KvpServiceClient_Open(HINSTANCE dll_handle,const char* ip,int port);
bool KvpServiceClient_close(HINSTANCE dll_handle);
bool KvpInsertNode(HINSTANCE dll_handle, const char* node_name);
bool KvpRegisterSpeakerByStream(HINSTANCE dll_handle, _Rpc_ModelInfo* &ret, int16_t* utt, int utt_size, const char* vp_node, const char* vp_dir, const char* spk_id);
void Delete_Rpc_ModelInfo(HINSTANCE dll_handle, _Rpc_ModelInfo* ptr);
bool KvpIdentifyTopSpeakerByStream(HINSTANCE dll_handle, _Rpc_TopSpeakerInfo* &ret, int16_t* utt, int utt_size, const char** vp_node_arr, int vp_node_arr_size, int node_num, int top_n, int utt_type);
void Delete_Rpc_TopSpeakerInfo(HINSTANCE dll_handle, _Rpc_TopSpeakerInfo* ptr);
bool KvpModelRemoveBySpkid(HINSTANCE dll_handle,const char* vp_node, const char* vp_dir, const char* spk_id);
bool KvpGetLicenceInfo(HINSTANCE dll_handle, _Rpc_LicenceInfo* &ret);
void Delete_Rpc_LicenceInfo(HINSTANCE dll_handle, _Rpc_LicenceInfo* ptr);

int _tmain(int argc, _TCHAR* argv[])
{
	HINSTANCE dll_handle = LoadLibrary(L"./lib/test_rpc.dll");
	if(!dll_handle){
		std::cout << GetLastError() << std::endl;
		return -1;
	}

	//IP端口请向技术产商索要
	if(!KvpServiceClient_Open(dll_handle,"47.104.11.15",9191)){
		return -1;
	}

	//添加节点
	if(!KvpInsertNode(dll_handle, "testnode")){
		return -1;
	}

	//说话人注册
	const char* filename = "./gabzh.wav";
	short *utt = nullptr;
	unsigned long utt_size = 0;
	if(!WaveLoad(filename,utt,utt_size)){
		std::cout << "语音文件格式错误" << std::endl;
		return -1;
	}

	_Rpc_ModelInfo* _mdlinfo = nullptr;
	KvpRegisterSpeakerByStream(dll_handle, _mdlinfo, utt, utt_size, "testnode", "/tmp/asv", "djsayrwq2ry7dehswjd8");
	if (_mdlinfo->RetCode != 0){
		std::cout << "Failed: " << _mdlinfo->ErrCode << "\t" << _mdlinfo->ErrMsg << std::endl;
	}
	Delete_Rpc_ModelInfo(dll_handle,_mdlinfo);//内部删除指针

	//说话人识别
	_Rpc_TopSpeakerInfo *rt = nullptr;
	const char* vp_node = "testnode";
	KvpIdentifyTopSpeakerByStream(dll_handle,rt, utt, utt_size, &vp_node, 1,1, 1, 0);
	if(rt->RetCode != 0){
		std::cout << "Failed: " << rt->ErrCode << "\t" << rt->ErrMsg << std::endl;
	}else{
		for(int i = 0;i < rt->Scores_size;++i){
			std::cout << "Spkid:" << rt->Scores[i]->Spkid << " Scores:" << rt->Scores[i]->Score << std::endl; 
		}
	}
	Delete_Rpc_TopSpeakerInfo(dll_handle, rt);
	delete[] utt;

	//删除说话人
	KvpModelRemoveBySpkid(dll_handle,"testnode", "/tmp/asv", "djsayrwq2ry7dehswjd8");

	//获取授权信息
	_Rpc_LicenceInfo* ret = nullptr;
	KvpGetLicenceInfo(dll_handle, ret);
	if(ret->RetCode == 0){
		std::cout << "dateStr:" << ret->dateStr << std::endl;
		std::cout << "fingerprint:" << ret->fingerprint << std::endl;
	}
	Delete_Rpc_LicenceInfo(dll_handle,ret);

	if(!KvpServiceClient_close(dll_handle)){
		return -1;
	}

	system("PAUSE"); 
	return 0;
}

bool KvpServiceClient_Open(HINSTANCE dll_handle,const char* ip,int port)
{
    try{
        if(dll_handle == nullptr) {
            return false;
        }

        typedef void* (*KvpServiceClient_Open)(const char* ip,int port);
        KvpServiceClient_Open KvpServiceClient_OpenFunc = (KvpServiceClient_Open)GetProcAddress(dll_handle, "KvpServiceClient_Open");
        if(!KvpServiceClient_OpenFunc){
			std::cout << "KvpServiceClient_Open func can't be found." << std::endl;
            return false;
        }
        KvpServiceClient_OpenFunc(ip, port);
        std::cout << "KvpServiceClient_Open success." << std::endl;
        return true;
    }catch(std::exception ex)
    {
		std::cout << ex.what() << std::endl;
        return false;
    }
}

bool KvpServiceClient_close(HINSTANCE dll_handle)
{
    try{
        if(dll_handle == nullptr) {
            return false;
        }

        typedef void* (*KvpServiceClient_close)();
        KvpServiceClient_close KvpServiceClient_closeFunc = (KvpServiceClient_close)GetProcAddress(dll_handle, "KvpServiceClient_close");
        if(!KvpServiceClient_closeFunc){
			std::cout << "KvpServiceClient_close func can't be found." << std::endl;
            return false;
        }
        KvpServiceClient_closeFunc();
		std::cout << "KvpServiceClient_close success." << std::endl;
        return true;
    }catch(std::exception ex)
    {
		std::cout << ex.what() << std::endl;
        return false;
    }
}

bool KvpInsertNode(HINSTANCE dll_handle, const char* node_name)
{
    try{
        if(dll_handle == nullptr) {
            return false;
        }

        typedef int32_t (*KvpInsertNode)(const char* node_name);
        KvpInsertNode KvpInsertNodeFunc = (KvpInsertNode)GetProcAddress(dll_handle, "KvpInsertNode");
        if(!KvpInsertNodeFunc){
            std::cout << "KvpInsertNode func can't be found." <<std::endl;
            return false;
        }
        int ret = KvpInsertNodeFunc(node_name);
        if(ret == 0){
            std::cout << "KvpInsertNode success." << std::endl;
			return true;
        }else{
            std::cout << "KvpInsertNode failed." << std::endl;
			return false;
        }
    }catch(std::exception ex)
    {
		std::cout << ex.what() << std::endl;
        return false;
    }
}

bool KvpRegisterSpeakerByStream(HINSTANCE dll_handle, _Rpc_ModelInfo* &ret, int16_t* utt, int utt_size, const char* vp_node, const char* vp_dir, const char* spk_id)
{
    try{
        if(dll_handle == nullptr) {
            return false;
        }

        typedef _Rpc_ModelInfo* (*KvpRegisterSpeakerByStream)(int16_t* utt, int utt_size, const char* vp_node, const char* vp_dir, const char* spk_id);
        KvpRegisterSpeakerByStream KvpRegisterSpeakerByStreamFunc = (KvpRegisterSpeakerByStream)GetProcAddress(dll_handle, "KvpRegisterSpeakerByStream");
        if(!KvpRegisterSpeakerByStreamFunc){
            std::cout << "KvpRegisterSpeakerByStream func can't be found." << std::endl;
            return false;
        }
        ret = KvpRegisterSpeakerByStreamFunc(utt,utt_size,vp_node,vp_dir,spk_id);
		std::cout << "KvpRegisterSpeakerByStream success." << std::endl;
        return true;
    }catch(std::exception ex)
    {
		std::cout << ex.what() << std::endl;
        return false;
    }
}

void Delete_Rpc_ModelInfo(HINSTANCE dll_handle, _Rpc_ModelInfo* ptr)
{
    try{
        if(dll_handle == nullptr) {
            return;
        }

        typedef void (*Delete_Rpc_ModelInfo)(_Rpc_ModelInfo *ptr);
        Delete_Rpc_ModelInfo Delete_Rpc_ModelInfoFunc = (Delete_Rpc_ModelInfo)GetProcAddress(dll_handle, "Delete_Rpc_ModelInfo");
        if(!Delete_Rpc_ModelInfoFunc){
            std::cout << "Delete_Rpc_ModelInfo func can't be found." << std::endl;
            return;
        }
        Delete_Rpc_ModelInfoFunc(ptr);
        std::cout << "Delete_Rpc_ModelInfo success." << std::endl;
    }catch(std::exception ex)
    {
		std::cout << ex.what() << std::endl;
    }
}

bool KvpIdentifyTopSpeakerByStream(HINSTANCE dll_handle, _Rpc_TopSpeakerInfo* &ret, int16_t* utt, int utt_size, const char** vp_node_arr, int vp_node_arr_size, int node_num, int top_n, int utt_type)
{
    try{
        if(dll_handle == nullptr) {
            return false;
        }

        typedef _Rpc_TopSpeakerInfo* (*KvpIdentifyTopSpeakerByStream)(int16_t* utt, int utt_size, const char** vp_node_arr, int vp_node_arr_size, int node_num, int top_n, int utt_type);
        KvpIdentifyTopSpeakerByStream KvpIdentifyTopSpeakerByStreamFunc = (KvpIdentifyTopSpeakerByStream)GetProcAddress(dll_handle, "KvpIdentifyTopSpeakerByStream");
        if(!KvpIdentifyTopSpeakerByStreamFunc){
            std::cout << "KvpIdentifyTopSpeakerByStream func can't be found." << std::endl;
            return false;
        }
        ret = KvpIdentifyTopSpeakerByStreamFunc(utt,utt_size,vp_node_arr,vp_node_arr_size,node_num,top_n,utt_type);
        std::cout << "KvpIdentifyTopSpeakerByStream success." << std::endl;
        return true;
    }catch(std::exception ex)
    {
		std::cout << ex.what() << std::endl;
        return false;
    }
}

void Delete_Rpc_TopSpeakerInfo(HINSTANCE dll_handle, _Rpc_TopSpeakerInfo* ptr)
{
    try{
        if(dll_handle == nullptr) {
            return;
        }

        typedef void (*Delete_Rpc_TopSpeakerInfo)(_Rpc_TopSpeakerInfo *ptr);
        Delete_Rpc_TopSpeakerInfo Delete_Rpc_TopSpeakerInfoFunc = (Delete_Rpc_TopSpeakerInfo)GetProcAddress(dll_handle, "Delete_Rpc_TopSpeakerInfo");
        if(!Delete_Rpc_TopSpeakerInfoFunc){
            std::cout << "Delete_Rpc_TopSpeakerInfo func can't be found." << std::endl;
            return;
        }
        Delete_Rpc_TopSpeakerInfoFunc(ptr);
        std::cout << "Delete_Rpc_TopSpeakerInfo success." << std::endl;
    }catch(std::exception ex)
    {
		std::cout << ex.what() << std::endl;
    }
}

bool KvpModelRemoveBySpkid(HINSTANCE dll_handle,const char* vp_node, const char* vp_dir, const char* spk_id)
{
	try{
        if(dll_handle == nullptr) {
            return false;
        }

        typedef int32_t (*KvpModelRemoveBySpkid)(const char* vp_node, const char* vp_dir, const char* spk_id);
        KvpModelRemoveBySpkid KvpModelRemoveBySpkidFunc = (KvpModelRemoveBySpkid)GetProcAddress(dll_handle, "KvpModelRemoveBySpkid");
        if(!KvpModelRemoveBySpkidFunc){
            std::cout << "KvpModelRemoveBySpkid func can't be found." << std::endl;
            return false;
        }
        int32_t ret = KvpModelRemoveBySpkidFunc(vp_node,vp_dir,spk_id);
		if(ret == 0){
			std::cout << "KvpModelRemoveBySpkid success." << std::endl;
			return true;
		}else{
			std::cout << "KvpModelRemoveBySpkid failed, code is " << ret << std::endl;
			return false;
		}
    }catch(std::exception ex)
    {
		std::cout << ex.what() << std::endl;
		return false;
    }
}

bool KvpGetLicenceInfo(HINSTANCE dll_handle, _Rpc_LicenceInfo* &ret)
{
    try{
        if(dll_handle == nullptr) {
            return false;
        }

        typedef _Rpc_LicenceInfo* (*KvpGetLicenceInfo)();
        KvpGetLicenceInfo KvpGetLicenceInfoFunc = (KvpGetLicenceInfo)GetProcAddress(dll_handle, "KvpGetLicenceInfo");
        if(!KvpGetLicenceInfoFunc){
            std::cout << "KvpGetLicenceInfo func can't be found." << std::endl;
            return false;
        }
        ret = KvpGetLicenceInfoFunc();
        std::cout << "KvpGetLicenceInfo success." << std::endl;
        return true;
    }catch(std::exception ex)
    {
		std::cout << ex.what() << std::endl;
        return false;
    }
}

void Delete_Rpc_LicenceInfo(HINSTANCE dll_handle, _Rpc_LicenceInfo* ptr)
{
    try{
        if(dll_handle == nullptr) {
            return;
        }

        typedef void (*Delete_Rpc_LicenceInfo)(_Rpc_LicenceInfo *ptr);
        Delete_Rpc_LicenceInfo Delete_Rpc_LicenceInfoFunc = (Delete_Rpc_LicenceInfo)GetProcAddress(dll_handle, "Delete_Rpc_LicenceInfo");
        if(!Delete_Rpc_LicenceInfoFunc){
            std::cout << "Delete_Rpc_LicenceInfo func can't be found." << std::endl;
            return;
        }
        Delete_Rpc_LicenceInfoFunc(ptr);
        std::cout << "Delete_Rpc_LicenceInfo success." << std::endl;
    }catch(std::exception ex)
    {
		std::cout << ex.what() << std::endl;
    }
}