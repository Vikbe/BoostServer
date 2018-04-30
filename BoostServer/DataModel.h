#pragma once

#include <boost/serialization/vector.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <string>



struct DataModel
{
	std::string Date;
	long long ExternalId;
	std::string Number;
	double Constant;
	int Digits;
	int Decimals;

	template <class Archive>
	void serialize(Archive& ar, unsigned int version)
	{
		ar & Date;
		ar & ExternalId;
		ar & Number;
		ar & Constant;
		ar & Digits;
		ar & Decimals;
	}
}; 

struct SearchModel
{
	std::string Date;
	std::string Id;
	std::string Number;
	std::string Constant;
	std::string Digits;
	std::string Decimals;

	template <class Archive>
	void serialize(Archive& ar, unsigned int version)
	{
		ar & Date;
		ar & Id;
		ar & Number;
		ar & Constant;
		ar & Digits;
		ar & Decimals;
	}
};

struct QueryData
{
	std::vector<DataModel> QueryResult;

	void AddData(const DataModel& row)
	{
		QueryResult.push_back(row);
	}

	template <class Archive>
	void serialize(Archive& ar, unsigned int version)
	{
		ar & QueryResult;
	}
};