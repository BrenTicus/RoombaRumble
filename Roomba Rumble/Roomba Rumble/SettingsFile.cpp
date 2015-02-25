#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <mscoree.h>
#include "SettingsFile.h"

#define BUFFER_SIZE 1500

using namespace std;

SettingsFile::SettingsFile(std::string path)
{
	fpath = path;

	reloadFile();
}

SettingsFile::~SettingsFile()
{
	keys.clear();			//throw out old values
	values.clear();
}


//reloads the file and all its properties
//returns true if file opened and read successfully false otherwise
bool SettingsFile::reloadFile()
{
	keys.clear();			//throw out old values
	values.clear();


	ifstream stream(fpath);
	//check if file open
	if (stream.is_open() == false)
	{
		return false;
	}


	char line[1500];
	while(stream.getline(line, BUFFER_SIZE))
	{
		string strLine(line, BUFFER_SIZE);

		int equalIndex = -1;
		for(int i =0 ; i < BUFFER_SIZE; i++)
		{
			if (line[i] == '=')
			{
				equalIndex = i;
				break;
			}
		}

		//found = sign take values
		if (equalIndex >= 0)
		{
			char key[BUFFER_SIZE];
			char value[BUFFER_SIZE];

			strncpy_s(key, line, equalIndex);		//copy key
			strcpy_s(value, line + equalIndex +1);
			keys.push_back(key);
			values.push_back(value);
			
		}
	}

	stream.close();

	return true;
}


//gets the value associated with the key in a file. 
//returns empty string if key not found.
string SettingsFile::getValue(string key)
{
	
	int index= -1;
	
	for(int i =0 ; i < keys.size(); i++)
	{
		if(strcmp(keys[i].c_str(), key.c_str()) == 0)
		{
			index = i;
			break;
		}
	}

	//not found, return null string
	if (index == -1)
	{
		return "";
	}

	return values[index];
}

int SettingsFile::getInt(string key)
{
	string value = getValue(key);

	if (&value == NULL){
		return -1;
	}
	return atoi(value.c_str());
}

//prints your current working directory
void SettingsFile::printWorkingDir(){
	
	char hi[300];
	GetCurrentDirectory(300,hi);
	printf("Working Dir: %s\n", hi);
}

