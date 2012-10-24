#include "Config.h"
template<> Config* Singleton<Config>::ms_Singleton = 0;
Config* Config::getSingletonPtr(void)
	{
		return ms_Singleton;
	}
Config& Config::getSingleton(void)
	{  
		return ( *ms_Singleton );  
	}