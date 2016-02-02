#include "Engine/Utilities/XMLProcessor.hpp"


XMLProcessor::XMLProcessor(void)
{

}

void load_file(char*  filename)
{
	TiXmlDocument doc(filename);
	bool loadOkay = doc.LoadFile();

	if ( loadOkay )
	{

		XMLElement* titleElement = doc.FirstChildElement( "PLAY" )->FirstChildElement( "TITLE" );
	}
	else
	{
		// load failed
	}
}


XMLProcessor::~XMLProcessor(void)
{
}
