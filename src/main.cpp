#include <iostream>
#include <fstream>
#include <vector>

#define HEADER_LUMPS 64
#define HEADER_LUMP_ENTDATA 0


struct lump_t {
	int	fileofs;	// offset into file (bytes)
	int	filelen;	// length of lump (bytes)
	int	version;	// lump format version
	char fourCC[4];	// lump ident code
};

struct dheader_t
{
	int	ident;                // BSP file identifier
	int	version;              // BSP file version
	lump_t	lumps[HEADER_LUMPS];  // lump directory array
	int	mapRevision;			// the map's revision (iteration, version) number
};


int main(int argc, const char* argv[])
{
	std::cout << "Galaco/DormantLemon's CS:GO game_text newline inserter\n\n";

	if (argc != 3) {
		std::cerr << "Usage: <path_to_filename/filename.bsp> <placeholder character>";
		return EXIT_FAILURE;
	}

	const char* filename = argv[1];
	const char* newlinePlaceholder = argv[2];

	//Open file
	std::ifstream file(filename, std::ios::binary);
	if (!file) {
		std::cerr << "Could not open file. No changes were made.\n";
		return EXIT_FAILURE;
	}

	//Just some file info
	std::cout << "File name: " << filename << std::endl;
	file.seekg(0, std::ios::end);
	std::cout << "File size: " << file.tellg()/1000 << "kb\n" << "Symbol to replace: " << newlinePlaceholder << "\n\n";

	//Read BSP to get the entdata lump
	file.seekg(0, std::ios::beg);
	dheader_t header;
	file.read((char *)&header, sizeof(header));
	lump_t entlump = header.lumps[HEADER_LUMP_ENTDATA];

	char* entdata_buf = new char[entlump.filelen];
	file.seekg(entlump.fileofs, std::ios::beg);
	file.read(entdata_buf, entlump.filelen);

	//Prepare entdata
	std::string entdata = std::string(entdata_buf);
	//Build offsets to replace
	int offset = std::string::npos, symbolCount = 0;
	file.seekg(0, std::ios::beg);
	while (std::string::npos != (offset = entdata.find(newlinePlaceholder, file.tellg()))) {
		entdata_buf[offset] = '\n';
		file.seekg(offset + 1, std::ios::beg);
		symbolCount++;
	}

	if (symbolCount > 0) {
		//backup existing bsp
		try {
			std::cout << "Backing up existing bsp...\n";
			std::ofstream backup(std::string(filename) + ".bak", std::ios::binary);
			file.seekg(0, std::ios::beg);
			backup << file.rdbuf();
			backup.close();
			std::cout << "Backup successful. continuing...\n";
		}
		catch (std::exception e) {
			delete entdata_buf;
			std::cerr << "Failed during backup. No changes were made.\n";
			return EXIT_FAILURE;
		}
	}
	else {

		file.close();
		delete entdata_buf;
		std::cout << "Nothing to replace. Exiting...";
		return EXIT_SUCCESS;
	}
	file.close();

	//Insert modified entdata into bsp
	try {
		std::ofstream output(filename, std::fstream::binary | std::fstream::out | std::fstream::in);
		output.seekp(entlump.fileofs);
		output << entdata_buf;
		output.close();
		std::cout << "Replaced " << symbolCount << " symbols \n";
	}
	catch (std::exception e) {
		delete entdata_buf;
		std::cerr << "Failed replacing entdata. No changes were made.\n";
		return EXIT_FAILURE;
	}

	//Cleanup
	delete entdata_buf;

	std::cout << "Complete!";

	return EXIT_SUCCESS;
}