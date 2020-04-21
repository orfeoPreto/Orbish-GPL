#pragma once

namespace CommandIDs

{

	enum

	{
		newProject = 0x300000,
		open = 0x300003,
		saveProjectAs = 0x300010,
		saveProject = 0x300011,
		showProjectSettings = 0x300030
		
	};
}

namespace CommandCategories

{

	static const char* const general = "General";

	static const char* const editing = "Settings";

}