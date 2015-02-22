#pragma once

using namespace std;

struct StringHasher {
    size_t operator()(const std::string& t) const {
          //calculate hash here.
		return 10;			//not implemented.
    }
};




class SettingsFile{

private:
	string fpath;
	//unordered_map<string, string, StringHasher> properties;
	vector<string> keys;
	vector<string> values;
public:
	
	SettingsFile(std::string);
	~SettingsFile();

	bool reloadFile();
	string SettingsFile::getValue(string key);

};

