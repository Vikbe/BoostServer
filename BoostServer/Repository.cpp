#include "Repository.h"
#include "CsvReader.h"
#include <boost/filesystem.hpp>
#include "sqlite/SQLite.h"

namespace repo {

	Repository::Repository()
	{
	
		if (!boost::filesystem::exists("data.db"))
		{
			// Parse the CSV file and upload to DB
			csvreader::CsvReader();
		}
		
	}

	QueryData Repository::Search(SearchModel search)
	{
		Connection connection = Connection("data.db");

		// Create the query and print it out
		std::ostringstream oss;

		// Cast constant to float so you can search with decimals in the constant search field
		if (search.Constant != "")
		{
			oss << "SELECT * FROM Posts WHERE Date LIKE '" << search.Date << "%' AND ExternalId LIKE '" << search.Id
				<< "%' AND Number LIKE '" << search.Number << "%' AND Constant LIKE '" << stof(search.Constant) << "%' AND Digits LIKE '"
				<< search.Digits << "%' AND Decimals LIKE '" << search.Decimals << "%';";
		} 
		else
		{
			oss << "SELECT * FROM Posts WHERE Date LIKE '" << search.Date << "%' AND ExternalId LIKE '" << search.Id
				<< "%' AND Number LIKE '" << search.Number << "%' AND Constant LIKE '" << search.Constant << "%' AND Digits LIKE '"
				<< search.Digits << "%' AND Decimals LIKE '" << search.Decimals << "%';";
		}
		
		std::string query = oss.str();
		std::cout << query << std::endl;

		QueryData queryData;

		try
		{
			for (Row row : Statement(connection, query.c_str()))
			{
				DataModel rowData;

				rowData.Date = row.GetString(0);
				rowData.ExternalId = row.GetBigInt(1);
				rowData.Number = row.GetString(2);
				rowData.Constant = atof(row.GetString(3));
				rowData.Digits = row.GetInt(4);
				rowData.Decimals = row.GetInt(5);

				queryData.AddData(rowData);
			}

			return queryData;
		}
		catch (Exception const & e)
		{
			std::cerr << e.Message.c_str() << " " << e.Result;
		}
		//Return empty, client will check result
		return queryData;
	}

}