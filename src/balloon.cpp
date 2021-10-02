#include <iostream>
#include <ctime>
#include <iomanip>
#include <getopt.h>
#include <unistd.h>
#include "deflate.h"
#include "inflate.h"

int DeflateWork(const char* filepath){
	// parser the file_path into : drive letter, directory path, filename, filename extension
   	// char drive[_MAX_DRIVE];  
   	// char dir[_MAX_DIR];  
   	// char fname[_MAX_FNAME];  
   	// char ext[_MAX_EXT];
	// _splitpath(file_path, );
	Deflate deflate;
	deflate.Compress("./test/1.txt", "./test/1.bal", "1.txt");
	return 0;
}

int InflateWork(const char* filepath){
	// parser the filepath into : drive letter, directory path, filename, filename extension

	Inflate inflate;
	inflate.Uncompress("./test/1.bal", "./test/1t.txt");
	return 0;
}

// display the version infomation(include version, author, website and so on)
void DispalyVersionInformation(){
	std::cout << "    __          ____                \n";
	std::cout << "   / /_  ____ _/ / /___  ____  ____ \n";
	std::cout << "  / __ \\/ __ `/ / / __ \\/ __ \\/ __ \\\n";
	std::cout << " / /_/ / /_/ / / / /_/ / /_/ / / / /\n";
	std::cout << "/_.___/\\__,_/_/_/\\____/\\____/_/ /_/ \n";
	std::cout << "\nballoon version 1.0.0(@Yaozh)";
	return ;
}

// display the help information(usage of option)
void DisplayHelpInformation(){
	std::cout << "Useage: balloon [option] ...\n";
	std::cout << "Options:\n";
	std::cout << std::left << "\t" << std::setw(30) << "--help or -h" << "display the information of usage.\n";
	std::cout << std::left << "\t" << std::setw(30) << "--version or -v" << "display the version information of the balloon.exe and detail of the program.\n";
	std::cout << std::left << "\t" << std::setw(30) << "--deflate or -d <args>" << "compress the file. <args>: the file path that you want to compress.\n";
	std::cout << std::left << "\t" << std::setw(30) << "--inflate or -i <args>" << "uncompress the file. <args>: the file path that you want to uncompress.\n";
	return ;
}

int main(int argc,char *argv[]) {
	// get command option ...
	int option;
	int option_index;
	char *single_options_string = (char*)"d:i:hv";
	static struct option long_options[] = {
		{"deflate", required_argument, 	NULL, 'd'},
		{"inflate", required_argument, 	NULL, 'i'},
		{"help", 	no_argument, 		NULL, 'h'},
		{"version", no_argument,       	NULL, 'v'},
		{0, 		0, 					0, 		0}
	};
	// TODO: deal with other args input...
	// simple solution is following...
	if(argc < 2){
		std::cout << argv[0] << ":" << " please use option [--help] to get more information.";
	}
	// using getopt_long function to get every option user input until return -1
	while ((option = getopt_long(argc, argv, single_options_string, long_options, &option_index)) != -1){
			switch (option){
			case 'd':
				// --deflate -d: compress file in the optarg(file_path)
				DeflateWork(optarg);
				break;
			case 'i':
				// --inflate -i: uncompress file in the optarg(file_path)
				InflateWork(optarg);
				break;
			case 'v':
				// --version -v
				DispalyVersionInformation();
				break;
			case 'h':
				// --help -h
				DisplayHelpInformation();
				break;
			default:
				break;
			}
	}
	return 0;
}