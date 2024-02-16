#include "pch.h"

#include "NRDSettings.h"
#include "NRD.h"
#include "NRDDescs.h"


void startDenoise(struct nrd::InstanceCreationDesc& creationDetails,  struct nrd::Instance* instanceinfo,struct nrd::CommonSettings& settings,bool debug) {
	nrd::Instance* instancecopy = instanceinfo;
	if (debug) {
		settings.debug = 1;
	}
	//TODO: Fix repo to include more of NRD source files without having them on the realTimeRayTrace repo
	 //nrd::CreateInstance(creationDetails, instancecopy);
	 //nrd::SetCommonSettings(*instancecopy,settings);
	frameprep(0, instancecopy);
}

void endDenoise(struct nrd::Instance* instanceinfo) {
	nrd::Instance* instancecopy = instanceinfo;
	//DestroyInstance(*instancecopy);
}
void frameprep(int framePosition, nrd::Instance* instancecopy) {
	//Need IMGUI setup to continue
}
