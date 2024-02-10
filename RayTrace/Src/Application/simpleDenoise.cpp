#include "pch.h"

#include "NRDSettings.h"
#include "NRD.h"
#include "NRDDescs.h"


void startDenoise(struct nrd::InstanceCreationDesc& creationDetails,  struct nrd::Instance* instanceinfo,struct nrd::CommonSettings& settings,bool debug) {
	nrd::Instance* instancecopy = instanceinfo;
	if (debug) {
		settings.debug = 1;
	}
	//TODO: Fix error on compiling 
	//CreateInstance(creationDetails, instancecopy);

	//SetCommonSettings(*instancecopy,settings);

}

void endDenoise(struct nrd::Instance* instanceinfo) {
	nrd::Instance* instancecopy = instanceinfo;
	//DestroyInstance(*instancecopy);
}
void frameprep(int framePosition) {

}
