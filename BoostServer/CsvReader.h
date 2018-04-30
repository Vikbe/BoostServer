#pragma  once

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "sqlite/SQLite.h"
#include <vector>
#include "DataModel.h"

namespace csvreader {

	DataModel ParseLine(std::string value)
	{
		
		std::istringstream strStream(value);
		DataModel post;
		std::string temp;

		for (int i = 0; i < 6; i++)
		{
			switch (i)
			{
			case 0: strStream >> post.Date;
				break;
			case 1: strStream >> temp;
				post.ExternalId = stoll(temp);
				break;
			case 2: strStream >> post.Number;
				break;
			case 3: strStream >> temp;
				post.Constant = stod(temp);
				break;
			case 4: strStream >> temp;
				post.Digits = stoi(temp);
				break;
			case 5: strStream >> temp;
				post.Decimals = stoi(temp);
				break;
			}
		}
		return post;
	}

	void CsvReader()
	{
		std::ifstream data("data.csv");

		if (data.is_open())
		{
			Connection connection = Connection("data.db");
			Execute(connection, "CREATE TABLE IF NOT EXISTS Posts (Date, ExternalId INTEGER, Number, Constant REAL, Digits INTEGER, Decimals INTEGER)");
			Statement insert(connection, "INSERT INTO Posts (Date, ExternalId, Number, Constant, Digits, Decimals) VALUES (?,?,?,?,?,?)");

			// Discard the first line
			std::string value;
			getline(data, value);

			Execute(connection, "BEGIN TRANSACTION");
			while (data.good())
			{
				getline(data, value);

				// Discard the final empty line
				if (value.length() != 0)
				{
					DataModel model = ParseLine(value);
					insert.Reset();
					insert.BindAll(model.Date, model.ExternalId, model.Number, model.Constant, model.Digits, model.Decimals);
					insert.Execute();
				}
			}
			Execute(connection, "END TRANSACTION");
		}
		else
		{
			std::cout << "Error could not open csv file! \n";
		}
	}
}



