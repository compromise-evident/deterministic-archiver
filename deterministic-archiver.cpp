/*Version 2.0.0                                                                 Run it: "apt install g++ geany". Open the .cpp in Geany. Hit F9 once. F5 to run.
The world's first deterministic archiver. Turn any folder
into a REPRODUCIBLE text file and back. Yes, a text file--
another world's first! It's clean, readable, and scrollable.*/

#include <fstream>
#include <iostream>
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
	if((user_option != 1) && (user_option != 2)) {cout << "\nInvalid.\n"; return 1;}
	
	//Gets path, fixes it, tries it.
	if(user_option == 1) {cout << "Drop/enter folder:\n"      ;}
	if(user_option == 2) {cout << "Drop/enter archive file:\n";}
	char path[100000] = {'\0'}; cin.getline(path, 100000); if(path[0] == '\0') {cin.getline(path, 100000);}
	if(path[0] == '\'') {for(int bm = 0, a = 0; a < 100000; a++) {if(path[a] != '\'') {path[bm] = path[a]; if(path[bm] == '\\') {path[bm] = '\'';} bm++;}}}
	for(int a = 99999; a >= 0; a--) {if(path[a] != '\0') {if(path[a] == ' ') {path[a] = '\0';} break;}}
	in_stream.open(path); if(in_stream.fail() == true) {cout << "\nNo path " << path << "\n"; return 1;} in_stream.close();
	
	//Create archive.___________________________________________________________________________________________________________________
	if(user_option == 1)
	{	string find;
		find = "find \""; find += path; find += "\" -mindepth 1 -type d -printf '%P\n' | sort -V > archive.txt"; system(find.c_str()); //Adds folder names to empty archive.     find "/path/to/FOLDER" -mindepth 1 -type d -printf '%P\n' | sort -V > archive.txt
		find = "find \""; find += path; find += "\" -mindepth 1 -type f -printf '%P\n' | sort -V > short_paths"; system(find.c_str()); //Gets list of files (short paths.)       find "/path/to/FOLDER" -mindepth 1 -type f -printf '%P\n' | sort -V > short_paths
		find = "find \""; find += path; find += "\"             -type f                | sort -V > full_paths" ; system(find.c_str()); //Gets list of files (full paths.)        find "/path/to/FOLDER"             -type f                | sort -V > full_paths
		
		//Gets number of files.
		long long number_of_files = 0;
		in_stream.open("full_paths"); if(in_stream.fail() == true) {cout << "\nERROR 1\n"; return 1;} in_stream.get(file_byte);
		for(; in_stream.eof() == false; in_stream.get(file_byte)) {if(file_byte == '\n') {number_of_files++;}}
		in_stream.close();
		
		//Adds files to archive.
		if(number_of_files > 0)
		{	ifstream in_stream_short_paths;
			ifstream in_stream_full_paths;
			in_stream_short_paths.open("short_paths"); if(in_stream_short_paths.fail() == true) {cout << "\nERROR 2\n"; return 1;}
			in_stream_full_paths.open ("full_paths" ); if(in_stream_full_paths.fail()  == true) {cout << "\nERROR 3\n"; return 1;}
			out_stream.open("archive.txt", ios::app); out_stream << "\n";
			for(long long a = 0; a < number_of_files; a++)
			{	//Adds file name.
				string line; getline(in_stream_short_paths, line); out_stream << line << "\n";
				
				//Opens file.
				getline(in_stream_full_paths, line); in_stream.open(line); if(in_stream.fail() == true) {cout << "\nERROR 4\n"; return 1;}
				
				//Adds file content.
				char symbols[17] = {"0123456789abcdef"};
				in_stream.get(file_byte); if(in_stream.eof() == true) {out_stream << "EMPTY FILE\n\n"; in_stream.close(); continue;}
				for(long long strip = 0; in_stream.eof() == false; in_stream.get(file_byte))
				{	raw_byte = file_byte; if(raw_byte < 0) {raw_byte += 256;}
					out_stream << symbols[raw_byte >>  4];
					out_stream << symbols[raw_byte &  15];
					strip++; if(strip == 5000) {out_stream << "\n"; strip = 0;} //5,000 file bytes per line but in hex, so 10,000 characters are shown.
				}
				out_stream << "\n\n";
				in_stream.close();
			}
			in_stream_short_paths.close();
			in_stream_full_paths.close();
			out_stream.close();
		}
		
		//Saves & prints sha256sum of archive.txt.
		system("sha256sum archive.txt > sha256sum");
		in_stream.open("sha256sum"); if(in_stream.fail() == true) {cout << "\nERROR 5\n"; return 1;}
		char line[100000]; in_stream.getline(line, 100000); cout << "\n" << line << "\n"; in_stream.close();
		remove("short_paths"); remove("full_paths");
	}
	
	//Unpack archive.___________________________________________________________________________________________________________________
	if(user_option == 2)
	{	//Creates empty folder "unpacked".
		system("rm -r -f unpacked"); system("mkdir unpacked");
		
		//If empty archive.
		in_stream.open(path); if(in_stream.fail()  == true) {cout << "\nERROR 6\n"; return 1;}
		in_stream.get(file_byte); if(in_stream.eof() == true) {in_stream.close(); return 0;} in_stream.close();
		
		//If folders.
		if(file_byte != '\n')
		{	in_stream.open(path); if(in_stream.fail()  == true) {cout << "\nERROR 7\n"; return 1;}
			char name[100000]; in_stream.getline(name, 100000);
			for(; name[0] != '\0'; in_stream.getline(name, 100000))
			{	string mkdir = "mkdir \"unpacked/"; mkdir += name; mkdir += '"'; system(mkdir.c_str()); //mkdir "unpacked/FOLDER NAME"
			}
			in_stream.getline(name, 100000); if(name[0] == '\0') {in_stream.close(); return 0;}
			in_stream.close();
		}
		
		//If files.
		in_stream.open(path); if(in_stream.fail()  == true) {cout << "\nERROR 8\n"; return 1;}
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
