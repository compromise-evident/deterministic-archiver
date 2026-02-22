/*Version 1.0.0                                                                 Run it: "apt install g++ geany". Open the .cpp in Geany. Hit F9 once. F5 to run.
The world's first deterministic archiver. Turn any folder
into a REPRODUCIBLE text file and back. Yes, a text file--
another world's first! It's clean, readable, and scrollable.*/

#include <fstream>
#include <iostream>
#include <vector>
using namespace std;
int main()
{	int raw_byte;
	char file_byte;
	ifstream in_stream;
	ofstream out_stream;
	
	cout << "\n(1) Create archive"
	     << "\n(2) Unpack archive"
	
	     << "\n\nOption: ";
	
	int user_option; cin >> user_option;
	if((user_option != 1) && (user_option != 2)) {cout << "\nInvalid.\n"; return 0;}
	
	//Gets path, fixes it, tries it.
	if(user_option == 1) {cout << "Drop/enter folder:\n"      ;}
	if(user_option == 2) {cout << "Drop/enter archive file:\n";}
	char path[100000] = {'\0'}; cin.getline(path, 100000); if(path[0] == '\0') {cin.getline(path, 100000);}
	if(path[0] == '\'') {for(int bm = 0, a = 0; a < 100000; a++) {if(path[a] != '\'') {path[bm] = path[a]; if(path[bm] == '\\') {path[bm] = '\'';} bm++;}}}
	for(int a = 99999; a >= 0; a--) {if(path[a] != '\0') {if(path[a] == ' ') {path[a] = '\0';} break;}}
	in_stream.open(path); if(in_stream.fail() == true) {cout << "\nNo path " << path << "\n"; return 0;} in_stream.close();
	
	//Create archive.___________________________________________________________________________________________________________________
	if(user_option == 1)
	{	string find;
		find = "find \""; find += path; find += "\" -mindepth 1 -type d -printf '%P\n' | sort -V > archive.txt"  ; system(find.c_str()); //Adds folder names to empty archive.     find "/path/to/FOLDER" -mindepth 1 -type d -printf '%P\n' | sort -V > archive.txt
		find = "find \""; find += path; find += "\" -mindepth 1 -type f -printf '%P\n' | sort -V > f_short_paths"; system(find.c_str()); //Gets list of files (short paths.)       find "/path/to/FOLDER" -mindepth 1 -type f -printf '%P\n' | sort -V > f_short_paths
		find = "find \""; find += path; find += "\"             -type f                | sort -V > f"            ; system(find.c_str()); //Gets list of files.                     find "/path/to/FOLDER"             -type f                | sort -V > f
		
		//Gets number of files.
		long long number_of_files = 0;
		in_stream.open("f"); in_stream.get(file_byte);
		for(; in_stream.eof() == false; in_stream.get(file_byte)) {if(file_byte == '\n') {number_of_files++;}}
		in_stream.close();
		
		//Adds files to archive.
		if(number_of_files > 0)
		{	//Opens all files.
			vector <ifstream> in_stream_n;
			bool ok = true;
			in_stream.open("f");
			for(long long a = 0; a < number_of_files; a++)
			{	char name[100000]; in_stream.getline(name, 100000); in_stream_n.push_back(ifstream(name));
				if(in_stream_n[a].fail() == true) {cout << "CAN'T OPEN " << name << "\n"; ok = false;}
			}
			in_stream.close();
			if(ok == false) {return 0;}
			
			//Begins.
			out_stream.open("archive.txt", ios::app); out_stream << "\n";
			in_stream.open("f_short_paths");
			for(long long a = 0; a < number_of_files; a++)
			{	//Adds file name.
				char name[100000]; in_stream.getline(name, 100000); out_stream << name << "\n";
				
				//Adds file content.
				char symbols[17] = {"0123456789abcdef"};
				in_stream_n[a].get(file_byte); if(in_stream_n[a].eof() == true) {out_stream << "EMPTY FILE\n\n"; continue;}
				for(int strip = 0; in_stream_n[a].eof() == false; in_stream_n[a].get(file_byte))
				{	raw_byte = file_byte; if(raw_byte < 0) {raw_byte += 256;}
					out_stream << symbols[raw_byte >>  4];
					out_stream << symbols[raw_byte &  15];
					strip++; if(strip == 5000) {out_stream << "\n"; strip = 0;}
				}
				out_stream << "\n\n";
			}
			in_stream.close();
			out_stream.close();
		}
		
		//Saves & prints sha256sum of archive.txt.
		system("sha256sum archive.txt > sha256sum");
		in_stream.open("sha256sum"); char line[100000]; in_stream.getline(line, 100000); cout << "\n" << line << "\n"; in_stream.close();
		remove("f"); remove("f_short_paths");
	}
	
	//Unpack archive.___________________________________________________________________________________________________________________
	if(user_option == 2)
	{	//Creates empty folder "unpacked".
		system("rm -r -f unpacked"); system("mkdir unpacked");
		
		//If empty archive.
		in_stream.open(path); in_stream.get(file_byte); if(in_stream.eof() == true) {in_stream.close(); return 0;} in_stream.close();
		
		//If folders.
		if(file_byte != '\n')
		{	in_stream.open(path);
			char name[100000]; in_stream.getline(name, 100000);
			for(; name[0] != '\0'; in_stream.getline(name, 100000))
			{	string mkdir = "mkdir \"unpacked/"; mkdir += name; mkdir += '"'; system(mkdir.c_str()); //mkdir "unpacked/FOLDER NAME"
			}
			in_stream.getline(name, 100000); if(name[0] == '\0') {in_stream.close(); return 0;}
			in_stream.close();
		}
		
		//If files.
		in_stream.open(path);
		char line[100000]; in_stream.getline(line, 100000);
		if(file_byte != '\n') {for(; line[0] != '\0'; in_stream.getline(line, 100000)) {}}
		for(;;)
		{	//Creates empty file.
			in_stream.getline(line, 100000);
			if(line[0] == '\0') {in_stream.close(); return 0;}
			string name = "unpacked/"; name += line;
			out_stream.open(name); out_stream.close();
			
			//Adds file content.
			in_stream.getline(line, 100000);
			if(line[0] != 'E')
			{	out_stream.open(name);
				//All strips.
				for(;;)
				{	//1 strip.
					for(int a = 0; line[a] != '\0'; a += 2)
					{	int first  = line[a    ];
						int second = line[a + 1];
						if(first  < 58) {first  -= 48;} else {first  -= 87;}
						if(second < 58) {second -= 48;} else {second -= 87;}
						first <<= 4;
						out_stream.put(first + second);
					}
					in_stream.getline(line, 100000);
					if(line[0] == '\0') {break;}
				}
				out_stream.close();
			}
			else {in_stream.getline(line, 100000);}
		}
		in_stream.close();
	}
}
