#pragma once
#include<string>
#include<vector>
#include<inttypes.h>

using namespace std;

struct Simbol {
	string ime;
	string sekcija;
	int offset;
	char vidljivost;
	int rbr;
	bool global;
};

struct Operand {
	string imeSimbola;
	int neposrednaVr;
	int regBr;
	int adrTip;
	int sufHL;
};

struct Relokacija {
	int offset;
	string tip;
	int vrednost;
};

struct Sekcija {
	string ime;
	vector<int8_t> prevod;
	vector<Relokacija> relokacije;
};
