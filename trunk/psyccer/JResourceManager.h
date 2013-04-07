#ifndef __JRESOURCEMANAGER_H__
#define __JRESOURCEMANAGER_H__

#include "JDirectDraw.h"
#include "JDirectSound.h"

#ifndef PURE
#define PURE =0
#endif

struct JResourceManager
{
	virtual ~JResourceManager() {}

	virtual int ReadData(char* name,void* dt,unsigned int len) PURE;

	virtual bool LoadResource(char* filename) PURE;
	virtual bool UnloadResource(char* filename) PURE;
};

JResourceManager* CreateDXResourceManager(JDirectDraw* pdd,JDirectSound* pds);

#endif //__JRESOURCEMANAGER_H__
