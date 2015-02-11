#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include "SettingsFile.h"

#define BUFFER_SIZE 1500

/*
int main(){
	SettingsFile s("D:\\Users\\Kevin\\documents\\visual studio 2012\\Projects\\Settings File\\Debug\\hi");
	//s.reloadFile();
	printf("%s\n", s.getValue("Durf  ").c_str());
	//printf("%s\n", s.getValue("Narudafsp").c_str());
	while(true);
}
*/
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
		printf(line);
		printf("\n");
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
//returns null if key not found.
string SettingsFile::getValue(string key)
{
	
	int index= -1;
	
	for(int i =0 ; i < keys.size(); i++)
	{
		if(strcmp(keys[i].c_str(), key.c_str()) == 0)
		{
			index = i;
		}
	}

	//not found, return null string
	if (index == -1)
	{
		return NULL;
	}

	return values[index];
}