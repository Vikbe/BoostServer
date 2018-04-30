#pragma once

#include "DataModel.h" 
#include "Repository.h"


namespace repo {
	
	class Repository
	{

	public:
		Repository();



		QueryData Search(SearchModel search);
	};
}