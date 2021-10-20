// Hayes, Jude       CS230 Section 12159  4/27/2021
// Third Laboratory Assignment – Cache Simulation

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <bitset>
using namespace std;

#define addressSize 16
#define dataBlockSize 4

struct commands {//A struct of all of the commands in the input

	vector<vector<int>> binaryAddressI, index_OffesetPos;//Saves input address in binary and binaryindex and offset of each command
	vector<string> tag, binaryAddress;//strings of the tag and binaryaddress of each command

	commands(vector<vector<int>> command, int tagSize, int cacheIndexSize, int bitOffsetSize) {//constructor to build all vectors for commands struct

		index_OffesetPos.resize(command.size());
		binaryAddressI.resize(command.size());

		for (size_t i = 0; i < binaryAddressI.size(); i++) {

			for (int j = command.at(i).at(1); j > 0; j /= 2)
				binaryAddressI.at(i).emplace_back(j % 2);

			binaryAddressI.at(i).resize(addressSize);
			reverse(binaryAddressI.at(i).begin(), binaryAddressI.at(i).end());
		}

		for (size_t i = 0; i < binaryAddressI.size(); i++) {

			ostringstream tagConvert, addressConvert;
			int cacheIndexConvert = 0, bitOffsetConvert = 0;

			for (int j = 0; j < addressSize; j++)
				addressConvert << binaryAddressI.at(i).at(j);
			for (int j = 0; j < tagSize; j++)
				tagConvert << binaryAddressI.at(i).at(j);
			for (int j = 1; j < cacheIndexSize; j++)
				cacheIndexConvert += binaryAddressI.at(i).at(tagSize + cacheIndexSize - j) * int(pow(2, j - 1));
			for (int j = 1; j <= bitOffsetSize; j++)
				bitOffsetConvert += binaryAddressI.at(i).at(binaryAddressI.at(i).size() - j) * int(pow(2, j - 1));

			binaryAddress.emplace_back(addressConvert.str());
			tag.emplace_back(tagConvert.str());
			index_OffesetPos.at(i).emplace_back(cacheIndexConvert);
			index_OffesetPos.at(i).emplace_back(bitOffsetConvert);
		}
	}
};

struct simulationVariables {//contains all the variables read in by the input file and calculates them to prepare for the simulation

	int lineSize = 0, totalSizeC = 0, assoc = 0, totalSizeM = 0, cacheLineSize = 0, tagSize = 0, cacheIndexSize = 0, bitOffsetSize = 0;

	simulationVariables(int lineSizeT, int totalSizeCT, int assocT, int totalSizeMT) :
		lineSize(lineSizeT), totalSizeC(totalSizeCT), assoc(assocT), totalSizeM(totalSizeMT), cacheLineSize(totalSizeCT / lineSizeT / assocT) {

		if (!(lineSize && totalSizeC && assoc && totalSizeM)) {
			cout << "Value for A-D not entered" << endl;
			exit(EXIT_FAILURE);
		}
		if ((lineSize < 1 || lineSize % 2) || (totalSizeC < 1 || totalSizeC % 2) || (assoc < 1 || assoc % 2) || (totalSizeM < 1 || totalSizeM % 2)) {
			cout << "Invalid value for A-D";
			exit(EXIT_FAILURE);
		}
		if (lineSize > (.1 * totalSizeC)) {
			cout << "Line size excedes ten percent of the cache size" << endl;
			exit(EXIT_FAILURE);
		}
		if (totalSizeC > (.1 * totalSizeM)) {
			cout << "Line size excedes ten percent of the memory size" << endl;
			exit(EXIT_FAILURE);
		}

		for (int i = 1; pow(2, i) <= lineSize; i++)
			bitOffsetSize = i;
		for (int i = 1; pow(2, i) <= cacheLineSize; i++)
			cacheIndexSize = i;

		tagSize = addressSize - bitOffsetSize - cacheIndexSize;
	}
};

class simulationAddresses {

private:
	commands* cmd = NULL;

public:
	vector<vector<int>> memoryAddressI, cacheLAddressI;//stores the memory and cahce addressses as ints
	vector<vector<string>> cacheAddressRel;//stores all of the cache addressses as strings
	vector<string> memoryAddressRel, cacheAddress;//save all the relative memory addresses, stores cache addressses as strings
	vector<int>  memoryAddress;//stores memory addresses as an int

	simulationAddresses(int totalSizeM, int cacheLineSize, int assoc, int cacheIndexSize, int tagSize, commands* cmdT) : cmd(cmdT) {
		memoryAddressI.resize(totalSizeM / addressSize);
		cacheLAddressI.resize(cacheLineSize);
		cacheAddressRel.resize(cacheLineSize, vector<string>(3 * assoc));

		for (int i = 0; i < cacheLineSize; i++) {//creates binary address for length of cachelinesize in ints
			for (int j = i; j > 0; j /= 2)
				cacheLAddressI.at(i).emplace_back(j % 2);

			cacheLAddressI.at(i).resize(cacheIndexSize);
			reverse(cacheLAddressI.at(i).begin(), cacheLAddressI.at(i).end());
		}

		for (size_t i = 0; i < cacheLAddressI.size(); i++) {//converts address to string, adds _ in front to symbolize being empty

			ostringstream addressConvert;
			for (int j = 0; j < cacheIndexSize; j++)
				addressConvert << cacheLAddressI.at(i).at(j);

			for (int j = 0; j < addressSize - cacheIndexSize - tagSize; j++)
				addressConvert << "_";

			for (int j = 0; j < assoc; j++)
				cacheAddressRel.at(i).at(1 + j * 3) = addressConvert.str();
		}

		for (size_t i = 0; i < cacheAddressRel.size(); i++) {//converts address to string, adds _ in back to symbolize offset

			ostringstream addressCreate;
			for (int j = 0; j < tagSize; j++)
				addressCreate << "_";

			for (int j = 0; j < assoc; j++) {
				cacheAddressRel.at(i).at(j * 3) = addressCreate.str();
				cacheAddressRel.at(i).at(2 + j * 3) = cacheAddressRel.at(i).at(0) + cacheAddressRel.at(i).at(1);
			}
		}

		for (size_t i = 0; i < memoryAddressI.size(); i++) {//creates memory addresses in binary
			for (int j = i; j > 0; j /= 2)
				memoryAddressI.at(i).emplace_back(j % 2);

			memoryAddressI.at(i).resize(addressSize);
			reverse(memoryAddressI.at(i).begin(), memoryAddressI.at(i).end());
		}

		for (size_t i = 0; i < memoryAddressI.size(); i++) {//adds _ to symbolize offset and converts to string

			ostringstream addressConvert;
			for (int j = 0; j < addressSize; j++) {

				if (j >= cacheIndexSize + tagSize)
					addressConvert << "_";
				else
					addressConvert << memoryAddressI.at(i).at(j + 4);
			}

			memoryAddressRel.emplace_back(addressConvert.str());
		}

		for (size_t i = 0; i < memoryAddressI.size(); i++)
			memoryAddress.emplace_back(i * addressSize);
	}

	void updateCacheAddress(int index, string newData) {//Updates cache address when something new is moved in the cache

		cacheAddressRel.at(cmd->index_OffesetPos.at(index).at(0)).emplace(cacheAddressRel.at(cmd->index_OffesetPos.at(index).at(0)).begin(), "");
		cacheAddressRel.at(cmd->index_OffesetPos.at(index).at(0)).emplace(cacheAddressRel.at(cmd->index_OffesetPos.at(index).at(0)).begin(), cacheAddressRel.at(cmd->index_OffesetPos.at(index).at(0)).at(2));
		cacheAddressRel.at(cmd->index_OffesetPos.at(index).at(0)).emplace(cacheAddressRel.at(cmd->index_OffesetPos.at(index).at(0)).begin(), newData);
		cacheAddressRel.at(cmd->index_OffesetPos.at(index).at(0)).at(2) = cmd->tag.at(index) + cacheAddressRel.at(cmd->index_OffesetPos.at(index).at(0)).at(1);

		if (cacheAddressRel.at(cmd->index_OffesetPos.at(index).at(0)).size() > 9)
			cacheAddressRel.at(cmd->index_OffesetPos.at(index).at(0)).erase(cacheAddressRel.at(cmd->index_OffesetPos.at(index).at(0)).end() - 4, cacheAddressRel.at(cmd->index_OffesetPos.at(index).at(0)).end() - 1);
	}
};

class simulation {//a class of the simulated cache and memory and relasted variables
private:
	commands* cmd = NULL;

public:
	vector<vector<int>> memory, cache;
	vector<vector<string>> tag;
	vector<vector<bool>> VD;

	simulation(int totalSizeM, int cacheLineSize, int assoc, commands* cmdT) : cmd(cmdT) {//resizes all vectors to the size of simulation variables

		memory.resize(totalSizeM / addressSize, vector<int>(addressSize / dataBlockSize));
		cache.resize(cacheLineSize, vector<int>(assoc * dataBlockSize));
		tag.resize(cacheLineSize, vector<string>(assoc));
		VD.resize(cacheLineSize, vector<bool>(assoc * 2));
	}

	int dataInCache(int assocIndex, int index) {//returns current data in cache

		return cache.at(cmd->index_OffesetPos.at(index).at(0)).at((cmd->index_OffesetPos.at(index).at(1) / dataBlockSize) + dataBlockSize * assocIndex);
	}

	string dataInTag(int assocIndex, int index) {//returns data in tag

		return tag.at(cmd->index_OffesetPos.at(index).at(0)).at(assocIndex);
	}

	void setDataInCache(int index, int newData) {//sets new data in the cache

		cache.at(cmd->index_OffesetPos.at(index).at(0)).at((cmd->index_OffesetPos.at(index).at(1) / dataBlockSize)) = newData;
	}

	void setDataInTag(int index, string newData) {//sets new data to the tag
		tag.at(cmd->index_OffesetPos.at(index).at(0)).at(0) = newData;
	}

	void swapDataInCache(int assocIndex, int index) {//swaps data in the cache

		for (int i = dataBlockSize - 1; i >= 0; i--) {
			cache.at(cmd->index_OffesetPos.at(index).at(0)).emplace(cache.at(cmd->index_OffesetPos.at(index).at(0)).begin(), cache.at(cmd->index_OffesetPos.at(index).at(0)).at(dataBlockSize + assocIndex * dataBlockSize - 1));
			cache.at(cmd->index_OffesetPos.at(index).at(0)).erase(cache.at(cmd->index_OffesetPos.at(index).at(0)).begin() + dataBlockSize + assocIndex * dataBlockSize);
		}
	}

	void swapDataInTag(int assocIndex, int index) {//swaps data in the tag

		for (int i = assocIndex - 1; i >= 0; i--) {
			tag.at(cmd->index_OffesetPos.at(index).at(0)).emplace(tag.at(cmd->index_OffesetPos.at(index).at(0)).begin(), tag.at(cmd->index_OffesetPos.at(index).at(0)).at(assocIndex));
			tag.at(cmd->index_OffesetPos.at(index).at(0)).erase(tag.at(cmd->index_OffesetPos.at(index).at(0)).begin() + assocIndex + 1);
		}
	}

	void moveCacheToMemory(int index, int assoc, int bitOffsetSize, simulationAddresses* simA) {//moves cache line to the memory

		for (size_t i = 0; i < memory.size(); i++) {

			if (simA->cacheAddressRel.at(cmd->index_OffesetPos.at(index).at(0)).at(assoc * 3 - 1) == simA->memoryAddressRel.at(i)) {

				for (int j = 0; j < dataBlockSize; j++) {

					memory.at(i).at(j) = cache.at(cmd->index_OffesetPos.at(index).at(0)).at(j + (assoc - 1) * dataBlockSize);
					cache.at(cmd->index_OffesetPos.at(index).at(0)).at(j + (assoc - 1) * dataBlockSize) = 0;
				}
				return;
			}
		}
		cout << "Could Not Insert Number To Cache Because The Cache Address Is Not In The Memory" << endl;
	}

	void setMem(int index, int assoc, int bitOffsetSize, vector<vector<int>> command, simulationAddresses* simA) {//moves data from the memory to the cache and cache to the memory

		for (size_t i = 0; i < memory.size(); i++) {

			if (simA->cacheAddressRel.at(cmd->index_OffesetPos.at(index).at(0)).at(assoc * 3 - 1) == simA->memoryAddressRel.at(i)) {

				for (int j = 0; j < dataBlockSize; j++) {

					int memTemp = cache.at(cmd->index_OffesetPos.at(index).at(0)).at(j + (assoc - 1) * dataBlockSize);
					cache.at(cmd->index_OffesetPos.at(index).at(0)).at(j + (assoc - 1) * dataBlockSize) = memory.at(cmd->index_OffesetPos.at(index).at(0)).at(j);
					memory.at(cmd->index_OffesetPos.at(index).at(0)).at(j) = memTemp;
				}

				return;
			}
		}
	}
};

class print {//class to print differnt parts of the simulation

private:
	simulationVariables* simV = NULL;
	commands* cmd = NULL;
	vector<vector<int>> command;

public:

	print(simulationVariables* simVT, commands* cmdT, vector<vector<int>> commandT) :simV(simVT), cmd(cmdT), command(commandT) { printCMD(); };

	void printCMD() {//prints all of the commands of from the input file and the command class

		cout << endl << "Line Size: " << simV->lineSize << " Size Of Cache: " << simV->totalSizeC << " Associativity: " << simV->assoc << " Size Of Memory: " << simV->totalSizeM << endl << endl;

		for (size_t i = 0; i < cmd->binaryAddress.size(); i++) {//Prints E and F

			if (char(command.at(i).at(0)) == 'E' && char(command.at(i).at(2)) == 'W')
				cout << char(command.at(i).at(0)) << command.at(i).at(1) << char(command.at(i).at(2)) << command.at(i).at(3) << ": ";
			else
				cout << char(command.at(i).at(0)) << command.at(i).at(1) << ": ";

			cout << cmd->binaryAddress.at(i);

			cout << endl <<//Prints A B C D
				"Tag: " << cmd->tag[i] <<
				" CacheIndex: " << cmd->index_OffesetPos.at(i).at(0) <<
				" BitOffset: " << cmd->index_OffesetPos.at(i).at(1) << endl << endl;
		}

		cout << endl << endl;
	}

	void printCache(simulation sim, simulationAddresses simA) {//prints the cache and the related items

		for (size_t i = 0; i < sim.cache.size(); i++) {//Prints Cache

			cout << endl;

			for (int j = 0; j < simV->assoc; j++)
				cout << simA.cacheAddressRel.at(i).at(2 + j * 3) << "  ";

			cout << "  | ";

			if (sim.tag.at(i).at(0) != "")
				cout << sim.tag.at(i).at(0);
			else
				cout << "      ";

			if (sim.tag.at(i).at(1) != "")
				cout << " " << sim.tag.at(i).at(1);
			else
				cout << "       ";

			for (int j = 0; j < simV->assoc; j++) {
				if (sim.VD.at(i).at(0 + j * 2))
					cout << "|    V ";
				else
					cout << "|      ";

				if (sim.VD.at(i).at(1 + j * 2))
					cout << "    D  ";
				else
					cout << "       ";
			}

			for (size_t j = 0; j < sim.cache.at(i).size(); j++) {

				if (j % dataBlockSize == 0)
					cout << " | ";

				cout << sim.cache.at(i).at(j) << " ";
			}
		}
	}

	void printMemory(simulation sim, simulationAddresses simA) {//prints the memory in groups of 4

		cout << endl << endl;
		for (size_t i = 0; i < sim.memory.size() / 4; i++) {
			for (size_t k = 0; k < 4; k++) {

				cout << "    ";
				for (int j = i + k * sim.memory.size() / 4; j < 1000; j = j * 10 + 1)
					cout << " ";

				cout << simA.memoryAddress.at(i + k * sim.memory.size() / 4) << " | " << simA.memoryAddressRel.at(i + k * sim.memory.size() / 4) << " | ";

				for (size_t j = 0; j < sim.memory.at(i).size(); j++)
					cout << sim.memory.at(i + k * sim.memory.size() / 4).at(j) << " ";

			}
			cout << endl;
		}
	}

	void printAddress(simulation sim, simulationAddresses simA, int index) {//prints the addresses requested by the F command

		cout << "Display Address: " << command.at(index).at(1) << endl;

		if (cmd->index_OffesetPos.at(index).at(0) < simV->cacheLineSize && cmd->index_OffesetPos.at(index).at(1) < simV->assoc * addressSize) {
			cout << "Address: " << cmd->binaryAddress.at(index) << endl;

			if (command.at(index).at(1) < simA.memoryAddress.at(simA.memoryAddress.size() - 1))
				cout << "Memory: " << sim.memory.at(command.at(index).at(1) / addressSize).at(cmd->index_OffesetPos.at(index).at(1) / dataBlockSize) << endl;
			else
				cout << "Memory: -1" << endl;

			for (size_t i = 0; i < sim.cache.size(); i++) {

				for (int j = 0; j < simV->assoc; j++)
					if (cmd->tag.at(index) == sim.tag.at(i).at(j)) {

						for (int k = 0; k < dataBlockSize; k++) {

							if (cmd->index_OffesetPos.at(index).at(0) == i && cmd->index_OffesetPos.at(index).at(1) / dataBlockSize == k) {
								cout << "Cache: " << sim.cache.at(i).at(j * dataBlockSize + k) << endl << endl;
								return;
							}
						}
					}

				if (i == sim.cache.size() - 1) {
					cout << " Cache: -1" << endl << endl;
				}
			}
		}
	}
};

int main() {

	vector<vector<int>> command;
	vector<char> printCommand;

	ifstream inFile("input.txt");
	size_t index = 0;
	int tempIn[4] = { 0 };
	int	in = 0;
	char cIn = ' ';

	while (inFile >> cIn && !inFile.eof()) {//read in input and sorts date accordingly

		if (cIn == 'A' && inFile >> in)
			tempIn[0] = in;
		else if (cIn == 'B' && inFile >> in)
			tempIn[1] = in;
		else if (cIn == 'C' && inFile >> in)
			tempIn[2] = in;
		else if (cIn == 'D' && inFile >> in)
			tempIn[3] = in;

		else if (cIn == 'E' || cIn == 'F') {//saves E and F to command vector

			printCommand.resize(printCommand.size() + 1);
			command.resize(command.size() + 1);
			command.at(command.size() - 1).emplace_back(cIn);

			if (cIn == 'E' || cIn == 'F') {

				inFile >> in;
				command.at(command.size() - 1).emplace_back(in);

				if (char(command.at(command.size() - 1).at(0)) == 'E' && inFile >> cIn) {

					command.at(command.size() - 1).emplace_back(cIn);
					inFile >> in;
					command.at(command.size() - 1).emplace_back(in);
				}
			}
		}
		else if (cIn == 'G' || cIn == 'H') {//saves print commands to print vector
			printCommand.at(printCommand.size() - 1) = cIn;
			printCommand.resize(printCommand.size() + 1);
		}

		else
			cout << "Invalid Input: " << cIn << endl << endl;
	}
	inFile.close();

	//creates instances of classes/structs
	simulationVariables simV(tempIn[0], tempIn[1], tempIn[2], tempIn[3]);
	commands cmd(command, simV.tagSize, simV.cacheIndexSize, simV.bitOffsetSize);
	simulation sim(simV.totalSizeM, simV.cacheLineSize, simV.assoc, &cmd);
	simulationAddresses simA(simV.totalSizeM, simV.cacheLineSize, simV.assoc, simV.cacheIndexSize, simV.tagSize, &cmd);
	print prt(&simV, &cmd, command);

	for (index = 0; index < command.size(); index++) {

		if (char(command.at(index).at(0)) == 'E') {//Runs Function For E

			if (char(command.at(index).at(2)) == 'W') {//Runs write function

				if (sim.dataInCache(0, index) == 0 && (sim.dataInTag(0, index) == "" || sim.dataInTag(0, index) == cmd.tag.at(index))) {//cache at assoc 1 is empty, has the same tg and is empty, place
					simA.updateCacheAddress(index, cmd.tag.at(index));
					sim.setDataInCache(index, command.at(index).at(3));
					sim.setDataInTag(index, cmd.tag.at(index));

					sim.VD.at(cmd.index_OffesetPos.at(index).at(0)).at(0) = true;//update valid dirty
					sim.VD.at(cmd.index_OffesetPos.at(index).at(0)).at(1) = true;
				}

				else {
					for (int i = 1; i < simV.assoc; i++) {

						if (sim.dataInCache(i, index) == 0 && sim.dataInTag(i, index) == cmd.tag.at(index)) {//inserts data into existing tag
							simA.updateCacheAddress(index, cmd.tag.at(index));
							sim.swapDataInCache(i, index);
							sim.setDataInCache(index, command.at(index).at(3));
							sim.swapDataInTag(i, index);

							sim.VD.at(cmd.index_OffesetPos.at(index).at(0)).at(0 + 2 * i) = true;//update valid dirty
							sim.VD.at(cmd.index_OffesetPos.at(index).at(0)).at(1 + 2 * i) = true;
						}
						else if (sim.dataInTag(i, index) == "") {//inserts data into assoc column
							simA.updateCacheAddress(index, cmd.tag.at(index));
							sim.swapDataInCache(i, index);
							sim.setDataInCache(index, command.at(index).at(3));
							sim.swapDataInTag(i, index);
							sim.setDataInTag(index, cmd.tag.at(index));

							sim.VD.at(cmd.index_OffesetPos.at(index).at(0)).at(0 + 2 * i) = true;//update valid dirty
							sim.VD.at(cmd.index_OffesetPos.at(index).at(0)).at(1 + 2 * i) = true;
						}
						//inserts data and moves old value to cache
						else if (i == simV.assoc - 1 && sim.dataInTag(simV.assoc - 1, index) != "" && sim.dataInTag(simV.assoc - 1, index) == cmd.tag.at(cmd.index_OffesetPos.at(index).at(0))) {
							sim.moveCacheToMemory(index, simV.assoc, simV.bitOffsetSize, &simA);
							simA.updateCacheAddress(index, cmd.tag.at(index));
							sim.swapDataInCache(i, index);
							sim.setDataInCache(index, command.at(index).at(3));
							sim.swapDataInTag(i, index);
							sim.setDataInTag(index, cmd.tag.at(index));

							sim.VD.at(cmd.index_OffesetPos.at(index).at(0)).at(0 + 2 * i) = true;//update valid dirty
							sim.VD.at(cmd.index_OffesetPos.at(index).at(0)).at(1 + 2 * i) = true;
						}
					}
				}
			}
			else if (char(command.at(index).at(2)) == 'R') {//runs read function

				for (int i = 0; i < simV.assoc; i++)
					if (sim.tag.at(cmd.index_OffesetPos.at(index).at(0)).at(i) == cmd.tag.at(index)) {

						cout << "Address: " << command.at(index).at(1) << " Read Already In Cache" << endl << endl;
					}
					else {

						for (int i = 0; i < dataBlockSize; i++)
							command.at(index).emplace_back(sim.memory.at(command.at(index).at(1) / addressSize).at(cmd.index_OffesetPos.at(index).at(1) / dataBlockSize + i));

						if (sim.dataInCache(0, index) == 0 && (sim.dataInTag(0, index) == "" || sim.dataInTag(0, index) == cmd.tag.at(index))) {//cache at assoc 1 is empty, has the same tg and is empty, place

							sim.VD.at(cmd.index_OffesetPos.at(index).at(0)).at(0) = true;//update valid dirty
							sim.VD.at(cmd.index_OffesetPos.at(index).at(0)).at(1) = false;

							simA.updateCacheAddress(index, cmd.tag.at(index));

							for (int i = 0; i < dataBlockSize; i++)
								sim.setDataInCache(index, command.at(index).at(3 + i));

							sim.setDataInTag(index, cmd.tag.at(index));
						}
						else {
							for (int i = 1; i < simV.assoc; i++) {

								if (sim.dataInTag(i, index) == "") {//inserts data into assoc column
									simA.updateCacheAddress(index, cmd.tag.at(index));
									sim.swapDataInCache(i, index);

									for (int j = 0; j < dataBlockSize; j++)
										sim.setDataInCache(index, command.at(index).at(3 + j));

									sim.swapDataInTag(i, index);
									sim.setDataInTag(index, cmd.tag.at(index));

									sim.VD.at(cmd.index_OffesetPos.at(index).at(0)).at(0 + 2 * i) = true;//update valid dirty
									sim.VD.at(cmd.index_OffesetPos.at(index).at(0)).at(1 + 2 * i) = false;
								}

								else if (i == simV.assoc - 1 && sim.dataInTag(simV.assoc - 1, index) != "") {//inserts data and moves old value to cache

									simA.updateCacheAddress(index, cmd.tag.at(index));
									sim.setMem(index, simV.assoc, simV.bitOffsetSize, command, &simA);
									sim.swapDataInCache(i, index);
									sim.swapDataInTag(i, index);
									sim.setDataInTag(index, cmd.tag.at(index));

									sim.VD.at(cmd.index_OffesetPos.at(index).at(0)).at(0 + 2 * i) = true;//update valid dirty
									sim.VD.at(cmd.index_OffesetPos.at(index).at(0)).at(1 + 2 * i) = false;
								}
							}
						}
					}
			}
		}
		else if (char(command.at(index).at(0)) == 'F') //Runs Function For F
			prt.printAddress(sim, simA, index);

		if (printCommand.at(index) == 'G')//prints cache
			prt.printCache(sim, simA);

		else if (printCommand.at(index) == 'H')//prints memory
			prt.printMemory(sim, simA);
	}

	if (command.size() < printCommand.size()) {//runs extra commands enter after command was parsed though

		if (printCommand.at(index) == 'G')
			prt.printCache(sim, simA);

		else if (printCommand.at(index) == 'H')
			prt.printMemory(sim, simA);
	}

	return 0;
};