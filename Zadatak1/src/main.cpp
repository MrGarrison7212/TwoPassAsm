#include<fstream>
#include<string>
#include<cstring>
#include<iostream>
#include<iomanip>
#include<map>
#include "struct.h"
#include <math.h>

using namespace std;

map<string, Simbol*> TS;
vector<Sekcija*> sekcije;
bool greska = false;
int linenum = 0;
const string APS = "APS";
const string PCREL = "PCREL";
vector< pair<string,string> > paroviMinus;
vector< pair<string,string> > paroviPlus;
vector<string> byteArgs;
vector<string> memDirSims;


int getOpCode(string kw) {
	if(kw == "halt"){
		return 1;
	}
	else if (kw == "xchg"){
		return 2;
	}
	else if (kw == "int"){
		return 3;
	}
	else if (kw == "mov"){
		return 4;
	}
	else if (kw == "add"){
		return 5;
	}
	else if (kw == "sub"){
		return 6;
	}
	else if (kw == "mul"){
		return 7;
	}
	else if (kw == "div"){
		return 8;
	}
	else if (kw == "cmp"){
		return 9;
	}
	else if (kw == "not"){
		return 10;
	}
	else if (kw == "and"){
		return 11;
	}
	else if (kw == "or"){
		return 12;
	}
	else if (kw == "xor"){
		return 13;
	}
	else if (kw == "test"){
		return 14;
	}
	else if (kw == "shl"){
		return 15;
	}
	else if (kw == "shr"){
		return 16;
	}
	else if (kw == "push"){
		return 17;
	}
	else if (kw == "pop"){
		return 18;
	}
	else if (kw == "jmp"){
		return 19;
	}
	else if (kw == "jeq"){
		return 20;
	}
	else if (kw == "jne"){
		return 21;
	}
	else if (kw == "jgt"){
		return 22;
	}
	else if (kw == "call"){
		return 23;
	}
	else if (kw == "ret"){
		return 24;
	}
	else if (kw == "iret"){
		return 25;
	}
	else
		return -1;

}

Simbol* pretraga(string ime) {
	map<string, Simbol*>::iterator it = TS.find(ime);
	if (it == TS.end())
		return NULL;
	return it->second;
}

int isNum(string str) {
	int ret = 10;
	for (int i = 0; i < str.size(); i++) {
		if (!isdigit(str[i])) {
			ret = -1;
			break;
		}
	}
	if (ret == 10)
		return ret;
	string tip = str.substr(0, 2);
	if (tip == "0x") {
		ret = 16;
		for (int i = 2; i < str.size(); i++) {
			if (!isdigit(str[i]) && (str[i] > 'f' || str[i] < 'a')) {
				ret = -1;
				break;
			}
		}
		if (ret == 16)
			return ret;
	}
	if (tip == "0b") {
		ret = 2;
		for (int i = 2; i < str.size(); i++) {
			if (str[i] != '0' && str[i] != '1') {
				ret = -1;
				break;
			}
		}
		if (ret == 2)
			return ret;
	}
	return -1;
}

int parseInt(string str, int osnova) {
	int ret = 0;
	if (osnova == 16) {
		for (int i = 2; i < str.size(); i++) {
			if (isdigit(str[i]))
				ret = ret * osnova + str[i] - '0';
			else
				ret = ret * osnova + str[i] - 'a' + 10;
		}
		return ret;
	}
	if (osnova == 10) {
		for (int i = 0; i < str.size(); i++) {
			ret = ret * osnova + str[i] - '0';
		}
		return ret;
	}
	if (osnova == 2) {
		for (int i = 2; i < str.size(); i++) {
			ret = ret * osnova + str[i] - '0';
		}
		return ret;
	}
	return ret;

}

Operand analizaOperanda(string operand, char suf) {
	Operand op;
	op.imeSimbola = "";
	op.regBr = 0;
	op.sufHL = 0;

	if (operand[0] == '*') {
		op.adrTip = 5;
		string pom = operand.substr(1);
		int osnova = isNum(pom);
		if (osnova != -1) {
			op.neposrednaVr = parseInt(pom, osnova);
		} else {
			cout << "Greska ! Argument na liniji " << linenum << " nije broj."
					<< endl;
			greska = true;
		}
	} else if (operand[0] == '&') {
		op.adrTip = 0;
		string reg = operand.substr(1);
		op.imeSimbola = reg;
	} else if (operand[0] == '$') {
		if (suf == 'b')
			op.adrTip = 3;
		else
			op.adrTip = 4;
		op.regBr = 7;
		op.imeSimbola = operand.substr(1);
	} else if (operand[0] == '[') {
		op.adrTip = 2;
		string reg = operand.substr(1, 2);
		if (reg == "pc")
			op.regBr = 7;
		else if (reg == "sp")
			op.regBr = 6;
		else
			op.regBr = reg[1] - '0';
	} else if (operand == "pc" || operand == "sp" || operand == "r0"
			|| operand == "r1" || operand == "r2" || operand == "r3"
			|| operand == "r4" || operand == "r5" || operand == "r6"
			|| operand == "r7" || operand == "pcl" || operand == "spl"
			|| operand == "r0l" || operand == "r1l" || operand == "r2l"
			|| operand == "r3l" || operand == "r4l" || operand == "r5l"
			|| operand == "r6l" || operand == "r7l" || operand == "pch"
			|| operand == "sph" || operand == "r0h" || operand == "r1h"
			|| operand == "r2h" || operand == "r3h" || operand == "r4h"
			|| operand == "r5h" || operand == "r6h" || operand == "r7h") {
		op.adrTip = 1;
		string reg = operand;

		if (operand.size() == 3) {
			char lh = operand[2];
			if (lh == 'l')
				op.sufHL = 1;
			else
				op.sufHL = 0;
			reg = operand.substr(0, 2);
		}
		if (reg == "pc")
			op.regBr = 7;
		else if (reg == "sp")
			op.regBr = 6;
		else
			op.regBr = reg[1] - '0';
	} else if (operand.size() > 4 && operand[2] == '[') {
		if (suf == 'b')
			op.adrTip = 3;
		else
			op.adrTip = 4;
		string reg = operand.substr(0, 2);
		if (reg == "pc")
			op.regBr = 7;
		else if (reg == "sp")
			op.regBr = 6;
		else
			op.regBr = reg[1] - '0';
		string pomeraj = operand.substr(3, operand.size() - 4);
		int osnova = isNum(pomeraj);
		if (osnova != -1) {
			op.neposrednaVr = parseInt(pomeraj, osnova);
		} else {
			op.imeSimbola = pomeraj;
		}
	} else {
		int osnova = isNum(operand);
		if (osnova != -1) {
			op.neposrednaVr = parseInt(operand, osnova);
			op.adrTip = 0;
		} else {
			op.adrTip = 5;
			op.imeSimbola = operand;
			memDirSims.push_back(operand);
		}
	}

	return op;
}

int main(int argc, char* argv[]) {
	char* imeul = argv[3];
	ifstream ulaz(imeul);
	string line;
	int LC = 0;
	string sekcija = "UND";
	while (getline(ulaz, line)) {
		linenum++;
		char* tok = strtok(const_cast<char*>(line.c_str()), " ,	");
		if (tok == NULL)
			continue;
		string kw = tok;
		if (kw[kw.size() - 1] == ':') {
			string lname = kw.substr(0, kw.size() - 1);

			Simbol* sim = pretraga(lname);
			if (sim) {
				if (sim->sekcija == "UND") {
					if (sim->vidljivost == 'g' && !sim->global) {
						cout << "Greska ! Externi simbol se definise na liniji "
								<< linenum << endl;
						greska = true;
					}
					else {
						sim->sekcija = sekcija;
						sim->offset = LC;
					}
				} else {
					cout << "Greska ! Visestruko definisanje simbola na liniji "
							<< linenum << endl;
					greska = true;
				}
			} else {
				sim = new Simbol;
				sim->ime = lname;
				sim->sekcija = sekcija;
				sim->offset = LC;
				sim->vidljivost = 'l';
				sim->rbr = TS.size();
				TS[lname] = sim;
			}

			tok = strtok(NULL, " ,	");
			if (tok == NULL)
				continue;
			kw = tok;
		}
		if (kw == ".end") {
			break;
		} else if (kw == ".equ") {
			tok = strtok(NULL, " ,	");
			if (tok == NULL) {
				cout << "Ne postoji argument na liniji " << linenum << endl;
				greska = true;
				continue;
			}
			string cime = tok;
			tok = strtok(NULL, " ,	");
			if (tok == NULL) {
				cout << "Ne postoji argument na liniji " << linenum << endl;
				greska = true;
				continue;
			}
			int osnova = isNum(tok);
			if (osnova == -1) {
				cout << "Greska ! Argument na liniji " << linenum
						<< " nije broj. " << endl;
				greska = true;
			} else {
				int cvred = parseInt(tok, osnova);

				Simbol* sim = pretraga(cime);
				if (sim) {
					if (sim->sekcija == "UND") {
						sim->sekcija = "CONST";
						sim->offset = cvred;
					} else {
						cout
								<< "Greska ! Visestruko definisanje simbola na liniji "
								<< linenum << endl;
						greska = true;
					}
				} else {
					sim = new Simbol;
					sim->ime = cime;
					sim->sekcija = "CONST";
					sim->offset = cvred;
					sim->vidljivost = 'l';
					sim->rbr = TS.size();
					TS[cime] = sim;
				}

				tok = strtok(NULL, " ,	");
				if (tok) {
					cout << "Visak argumenata na liniji " << linenum << endl;
					greska = true;
				}
			}
		} else if (kw == ".skip") {
			int temp;
			tok = strtok(NULL, " ,	");
			if (tok == NULL) {
				cout << "Ne postoji argument na liniji " << linenum << endl;
				greska = true;
				continue;
			} else {
				int osnova = isNum(tok);
				if (osnova == -1) {
					cout << "Greska ! Argument na liniji " << linenum
							<< " nije broj. " << endl;
					greska = true;
				} else {
					temp = parseInt(tok, osnova);
					LC += temp;
				}
				tok = strtok(NULL, " ,	");
				if (tok) {
					cout << "Visak argumenata na liniji " << linenum << endl;
					greska = true;
				}
			}
		} else if (kw == ".align") {
			int temp;
			tok = strtok(NULL, " ,	");
			if (tok == NULL) {
				cout << "Ne postoji argument na liniji " << linenum << endl;
				greska = true;
				continue;
			}
			int osnova = isNum(tok);
			if (osnova == -1) {
				cout << "Greska ! Argument na liniji " << linenum
						<< " nije broj. " << endl;
				greska = true;
			} else {
				temp = parseInt(tok, osnova);
				temp = pow(2, temp);
				if (LC % temp) {
					int temp1 = LC % temp;
					LC += temp - temp1;
				}
				tok = strtok(NULL, " ,	");
				if (tok) {
					cout << "Visak argumenata na liniji " << linenum << endl;
					greska = true;
				}
			}
		} else if (kw == ".extern" || kw == ".global") {
			tok = strtok(NULL, " ,	");
			while (tok) {
				string arg = tok;

				Simbol* sim = pretraga(arg);
				if (sim) {
					if (sim->vidljivost == 'g') {

						cout
								<< "Greska ! Visestruko proglasivanje simbola za globalni na liniji "
								<< linenum << endl;
						greska = true;
					} else {
						sim->vidljivost = 'g';
						if (kw == ".global") {
							sim->global = true;
						} else {
							sim->global = false;
						}
					}
				} else {
					sim = new Simbol;
					sim->ime = arg;
					sim->sekcija = "UND";
					sim->offset = 0;
					sim->vidljivost = 'g';
					if (kw == ".global") {
						sim->global = true;
					} else {
						sim->global = false;
					}
					sim->rbr = TS.size();
					TS[arg] = sim;
				}
				tok = strtok(NULL, " ,	");
			}
		} else if (kw == ".byte" || kw == ".word") {
			int temp = 0;
			tok = strtok(NULL, " ,	");
			while (tok) {
				temp++;
				string arg = tok;

				size_t indeksZ = arg.find_first_of("+-");
				if(indeksZ != string::npos){
					char znak = arg[indeksZ];
					string firstopZ = arg.substr(0,indeksZ);
					int osnova1 = isNum(firstopZ);
					Simbol* sim1 = NULL;
					if (osnova1 == -1) {
						sim1 = pretraga(firstopZ);
						if (sim1 == NULL) {
							sim1 = new Simbol;
							sim1->ime = firstopZ;
							sim1->sekcija = "UND";
							sim1->offset = 0;
							sim1->vidljivost = 'l';
							sim1->rbr = TS.size();
							TS[firstopZ] = sim1;
						}
						if (kw == ".byte") {
							byteArgs.push_back(firstopZ);
						}
					}
					string secondopZ = arg.substr(indeksZ+1);
					int osnova2 = isNum(secondopZ);
					Simbol* sim2 = NULL;
					if (osnova2 == -1) {
						sim2 = pretraga(secondopZ);
						if (sim2 == NULL) {
							sim2 = new Simbol;
							sim2->ime = secondopZ;
							sim2->sekcija = "UND";
							sim2->offset = 0;
							sim2->vidljivost = 'l';
							sim2->rbr = TS.size();
							TS[secondopZ] = sim2;
						}
						if (kw == ".byte") {
							byteArgs.push_back(secondopZ);
						}
					}
					if(sim1 != NULL && sim2 != NULL){
						if(znak == '+'){
							paroviPlus.push_back(pair<string, string>(sim1->ime,sim2->ime));
						}
						else {
							paroviMinus.push_back(pair<string, string>(sim1->ime,sim2->ime));
						}
					}
				}
				else {
					int osnova = isNum(arg);
					if (osnova == -1) {
						Simbol* sim = pretraga(arg);
						if (sim == NULL) {
							sim = new Simbol;
							sim->ime = arg;
							sim->sekcija = "UND";
							sim->offset = 0;
							sim->vidljivost = 'l';
							sim->rbr = TS.size();
							TS[arg] = sim;
						}
						if (kw == ".byte") {
							byteArgs.push_back(arg);
						}
					}
				}
				tok = strtok(NULL, " ,	");
			}
			if (kw == ".byte") {
				LC += temp;
			} else {
				LC += temp * 2;
			}
		} else if (kw == ".text" || kw == ".data" || kw == ".bss"
				|| kw == ".section") {
			LC = 0;
			sekcija = kw;
			if (kw == ".section") {
				tok = strtok(NULL, " ,	");
				if (tok == NULL) {
					cout << "Ne postoji argument na liniji " << linenum << endl;
					greska = true;
					continue;
				}
				sekcija = tok;
			}

			Simbol* sim = pretraga(sekcija);
			if (sim) {
				if (sim->sekcija == "UND") {
					sim->sekcija = sekcija;
					sim->offset = LC;
				} else {
					cout << "Visestruka definicija na liniji " << linenum
							<< endl;
					greska = true;
				}
			} else {
				sim = new Simbol;
				sim->ime = sekcija;
				sim->sekcija = sekcija;
				sim->offset = LC;
				sim->vidljivost = 'l';
				sim->rbr = TS.size();
				TS[sekcija] = sim;
			}
			tok = strtok(NULL, " ,	");
			if (tok) {
				cout << "Visak argumenata na liniji " << linenum << endl;
				greska = true;
			}
		} else {
			char suf = kw[kw.size() - 1];
			kw = kw.substr(0, kw.size() - 1);
			if (kw == "halt" || kw == "ret" || kw == "iret") {
				LC++;
				tok = strtok(NULL, " ,	");
				if (tok) {
					cout << "Visak argumenata na liniji " << linenum << endl;
					greska = true;
				}
			} else if (kw == "push" || kw == "pop" || kw == "jmp" || kw == "jeq"
					|| kw == "jne" || kw == "jgt" || kw == "call" || kw == "not"
					|| kw == "int") {
				tok = strtok(NULL, " ,	");
				if (tok == NULL) {
					cout << "Ne postoji argument na liniji " << linenum << endl;
					greska = true;
					continue;
				}

				Operand op1 = analizaOperanda(tok, suf);
				LC += 2;
				if (op1.adrTip != 1 && op1.adrTip != 2) {
					LC++;
					if (suf == 'w') {
						LC++;
					}
					else if (op1.adrTip == 5) {
						cout << "Greska na liniji " << linenum << " Ne moze se koristiti memorijsko adresiranje ako je sufix operacije b !"
									<< endl;
						greska = true;
					}

				}
				if (kw != "push" && kw != "jmp" && kw != "int" && kw != "jeq"
						&& kw != "jne" && kw != "jgt" && kw != "call")
					if (op1.adrTip == 0) {
						cout<< "Greska na liniji " << linenum << " . Ne moze se koristiti neposredno adresiranje za dati operand !"
								<< endl;
						greska = true;
					}

				if (!op1.imeSimbola.empty()) {
					Simbol* sim = pretraga(op1.imeSimbola);
					if (sim == NULL) {
						sim = new Simbol;
						sim->ime = op1.imeSimbola;
						sim->sekcija = "UND";
						sim->offset = 0;
						sim->vidljivost = 'l';
						sim->rbr = TS.size();
						TS[op1.imeSimbola] = sim;
					}
				}

				tok = strtok(NULL, " ,	");
				if (tok) {
					cout << "Visak argumenata na liniji " << linenum << endl;
					greska = true;
				}
			} else if (kw == "xchg" || kw == "mov" || kw == "add" || kw == "sub"
					|| kw == "mul" || kw == "div" || kw == "cmp" || kw == "and"
					|| kw == "or" || kw == "xor" || kw == "test" || kw == "shl"
					|| kw == "shr") {

				tok = strtok(NULL, " ,	");
				if (tok == NULL) {
					cout << "Ne postoji argument na liniji " << linenum << endl;
					greska = true;
					continue;
				}
				Operand op1 = analizaOperanda(tok, suf);
				tok = strtok(NULL, " ,	");
				if (tok == NULL) {
					cout << "Ne postoji argument na liniji " << linenum << endl;
					greska = true;
					continue;
				}
				Operand op2 = analizaOperanda(tok, suf);
				LC += 3;
				if (op1.adrTip != 1 && op1.adrTip != 2) {
					LC++;
					if (suf == 'w') {
						LC++;
					}
					else if (op1.adrTip == 5) {
						cout << "Greska na liniji " << linenum << " Ne moze se koristiti memorijsko adresiranje ako je sufix operacije b !"
								<< endl;
						greska = true;
					}
				}
				if (op2.adrTip != 1 && op2.adrTip != 2) {
					LC++;
					if (suf == 'w') {
						LC++;
					}
					else if (op2.adrTip == 5) {
						cout << "Greska na liniji " << linenum << " . Ne moze se koristiti memorijsko adresiranje ako je sufix operacije b !"
								<< endl;
						greska = true;
					}
				}
				if (kw != "cmp" && kw != "test")
					if (op1.adrTip == 0) {
						cout << "Greska na liniji " << linenum << " . Ne moze se koristiti neposredno adresiranje za prvi operand !"
								<< endl;
						greska = true;
					}
				if (kw == "xchg")
					if (op2.adrTip == 0) {
						cout << "Greska na liniji " << linenum << " . Ne moze se koristiti neposredno adresiranje za drugi operand !"
								<< endl;
						greska = true;
					}

				if (!op1.imeSimbola.empty()) {
					Simbol* sim = pretraga(op1.imeSimbola);
					if (sim == NULL) {
						sim = new Simbol;
						sim->ime = op1.imeSimbola;
						sim->sekcija = "UND";
						sim->offset = 0;
						sim->vidljivost = 'l';
						sim->rbr = TS.size();
						TS[op1.imeSimbola] = sim;
					}
				}

				if (!op2.imeSimbola.empty()) {
					Simbol* sim = pretraga(op2.imeSimbola);
					if (sim == NULL) {
						sim = new Simbol;
						sim->ime = op2.imeSimbola;
						sim->sekcija = "UND";
						sim->offset = 0;
						sim->vidljivost = 'l';
						sim->rbr = TS.size();
						TS[op2.imeSimbola] = sim;
					}
				}

				tok = strtok(NULL, " ,	");
				if (tok) {
					cout << "Visak argumenata na liniji " << linenum << endl;
					greska = true;
				}
			} else {
				cout << "Neregularna kljucna rec na liniji " << linenum << endl;
				greska = true;
			}
		}
	}

	for (map<string, Simbol*>::iterator it = TS.begin(); it != TS.end(); it++) {
		if ((it->second->vidljivost == 'l' || it->second->global)
				&& it->second->sekcija == "UND") {
			cout << "Simbol " << it->second->ime
					<< " je lokalni, ili deklarisan kao globalni, pa mora biti definisan! "
					<< endl;
			greska = true;

		}
	}

	for (int i = 0; i < paroviPlus.size(); ++i) {
		Simbol* sim1 = pretraga(paroviPlus[i].first);
		Simbol* sim2 = pretraga(paroviPlus[i].second);
		if (sim1->sekcija != "CONST" && sim2->sekcija != "CONST") {
			cout << "Simboli " << sim1->ime << " i " << sim2->ime
					<< " ne mogu se sabirati! "
					<< endl;
			greska = true;
		}
	}

	for (int i = 0; i < paroviMinus.size(); ++i) {
		Simbol* sim1 = pretraga(paroviMinus[i].first);
		Simbol* sim2 = pretraga(paroviMinus[i].second);
		if (sim1->sekcija != "CONST" && sim2->sekcija != "CONST" && sim1->sekcija != sim2->sekcija) {
			cout << "Simboli " << sim1->ime << " i " << sim2->ime
					<< " ne mogu se oduzimati jer nisu u istoj sekciji! "
					<< endl;
			greska = true;
		}
	}

	for (int i = 0; i < byteArgs.size(); ++i) {
		Simbol* sim = pretraga(byteArgs[i]);
		if(sim->sekcija != "CONST") {
			cout << ".byte mora imati neposrednu vrednost kao argument."
				<< endl;
			greska = true;
		}
	}

	for (int i = 0; i < memDirSims.size(); ++i) {
		Simbol* sim = pretraga(memDirSims[i]);
		if(sim->sekcija == "CONST") {
			cout << "Za memorijsko direktno adresiranje ne sme se koristiti konstanta."
				<< endl;
			greska = true;
		}
	}



	if (greska) {
		return 0;
	}

	char* imeiz = argv[2];
	ofstream izlaz(imeiz);

	int rbr = 0;

	izlaz << setw(10) << "# Tabela simbola: "<< endl;

	izlaz << setw(10) << "# Ime" << setw(10) << "Sekcija" << setw(10) << "LC"
			<< setw(10) << "Vid" << setw(10) << "R B" << endl;
	for (map<string, Simbol*>::iterator it = TS.begin(); it != TS.end(); it++) {
		izlaz << setw(10) << setfill(' ') << it->second->ime;
		izlaz << setw(10) << setfill(' ') << it->second->sekcija;
		izlaz << setw(10) << setfill(' ') << it->second->offset;
		izlaz << setw(10) << setfill(' ') << it->second->vidljivost;
		izlaz << setw(10) << setfill(' ') << (it->second->rbr = rbr++);
		izlaz << endl;
	}

	// drugi prolaz

	ulaz.seekg(0);
	Sekcija* tekuca = NULL;
	while (getline(ulaz, line)) {
		char* tok = strtok(const_cast<char*>(line.c_str()), " ,	");
		if (tok == NULL)
			continue;
		string kw = tok;
		if (kw[kw.size() - 1] == ':') {

			tok = strtok(NULL, " ,	");
			if (tok == NULL)
				continue;
			kw = tok;
		}
		if (kw == ".end") {
			break;
		} else if (kw == ".equ" || kw == ".extern" || kw == ".global") {
		} else if (kw == ".skip") {
			tok = strtok(NULL, " ,	");
			int osnova = isNum(tok);
			int temp = parseInt(tok, osnova);
			for (int i = 0; i < temp; ++i) {
				tekuca->prevod.push_back(0);
			}

		} else if (kw == ".align") {
			int temp;
			tok = strtok(NULL, " ,	");
			int osnova = isNum(tok);
			temp = parseInt(tok, osnova);
			temp = pow(2, temp);
			int LC = tekuca->prevod.size();
			int dif = 0;
			if (LC % temp) {
				int temp1 = LC % temp;
				dif = temp - temp1;
			}
			for (int i = 0; i < dif; ++i) {
				tekuca->prevod.push_back(0);
			}
		} else if (kw == ".byte" || kw == ".word") {
			tok = strtok(NULL, " ,	");
			while (tok) {
				string arg = tok;

				size_t indeksZ = arg.find_first_of("+-");
				if(indeksZ != string::npos){
					char znak = arg[indeksZ];
					string firstopZ = arg.substr(0,indeksZ);
					int osnova1 = isNum(firstopZ);
					Simbol* sim1 = NULL;
					int value1;
					if (osnova1 == -1) {
						sim1 = pretraga(firstopZ);
						if (sim1->sekcija == "CONST")  {
							value1 = sim1->offset;
							sim1 = NULL;
						}
					} else {
						value1 = parseInt(firstopZ, osnova1);
					}
					string secondopZ = arg.substr(indeksZ+1);
					int osnova2 = isNum(secondopZ);
					Simbol* sim2 = NULL;
					int value2;
					if (osnova2 == -1) {
						sim2 = pretraga(secondopZ);
						if (sim2->sekcija == "CONST")  {
							value2 = sim2->offset;
							sim2 = NULL;
						}
					} else {
						value2 = parseInt(secondopZ, osnova2);
					}
					int prevedeno;
					if(sim1 != NULL && sim2 != NULL){
						prevedeno = sim1->offset - sim2->offset;
					}
					else if (sim2 != NULL) {
						Relokacija rel;
						rel.offset = tekuca->prevod.size();
						rel.tip = APS;
						if (sim2->vidljivost == 'g') {
							rel.vrednost = sim2->rbr;
							prevedeno = 0;
						} else {
							Simbol* simSek = pretraga(sim2->sekcija);
							rel.vrednost = simSek->rbr;
							prevedeno = sim2->offset;
						}
						tekuca->relokacije.push_back(rel);
						if(znak == '+'){
							prevedeno = value1 + prevedeno;
						}
						else {
							prevedeno = value1 - prevedeno;
						}
					}
					else if (sim1 != NULL) {

						Relokacija rel;
						rel.offset = tekuca->prevod.size();
						rel.tip = APS;
						if (sim1->vidljivost == 'g') {
							rel.vrednost = sim1->rbr;
							prevedeno = 0;
						} else {
							Simbol* simSek = pretraga(sim1->sekcija);
							rel.vrednost = simSek->rbr;
							prevedeno = sim1->offset;
						}
						tekuca->relokacije.push_back(rel);
						if(znak == '+'){
							prevedeno = prevedeno + value2;
						}
						else {
							prevedeno = prevedeno - value2;
						}
					}
					else if(znak == '+'){
						prevedeno = value1 + value2;
					}
					else {
						prevedeno = value1 - value2;
					}
					tekuca->prevod.push_back(prevedeno);
					if (kw == ".word")
						tekuca->prevod.push_back(prevedeno >> 8);
				}
				else {

					int osnova = isNum(arg);
					if (osnova == -1) {
						Simbol* sim = pretraga(arg);
						if (sim->sekcija != "CONST") {
							Relokacija rel;
							rel.offset = tekuca->prevod.size();
							rel.tip = APS;
							int prevedeno;
							if (sim->vidljivost == 'g') {
								rel.vrednost = sim->rbr;
								prevedeno = 0;
							} else {
								Simbol* simSek = pretraga(sim->sekcija);
								rel.vrednost = simSek->rbr;
								prevedeno = sim->offset;
							}
							tekuca->relokacije.push_back(rel);
							tekuca->prevod.push_back(prevedeno);
							tekuca->prevod.push_back(prevedeno >> 8);
						} else {
							int argument = sim->offset;
							tekuca->prevod.push_back(argument);
							if (kw == ".word")
								tekuca->prevod.push_back(argument >> 8);
						}
					} else {
						int argument = parseInt(arg, osnova);
						tekuca->prevod.push_back(argument);
						if (kw == ".word")
							tekuca->prevod.push_back(argument >> 8);
					}
				}
				tok = strtok(NULL, " ,	");
			}
		} else if (kw == ".text" || kw == ".data" || kw == ".bss"
				|| kw == ".section") {
			sekcija = kw;
			if (kw == ".section") {
				tok = strtok(NULL, " ,	");
				sekcija = tok;
			}
			tekuca = new Sekcija();
			tekuca->ime = sekcija;
			sekcije.push_back(tekuca);
		} else {
			char suf = kw[kw.size() - 1];
			kw = kw.substr(0, kw.size() - 1);

			int instrDescr = getOpCode(kw);
			instrDescr <<= 1;
			if (suf == 'w')
				instrDescr |= 1;
			instrDescr <<= 2;
			tekuca->prevod.push_back(instrDescr);

			if (kw == "push" || kw == "pop" || kw == "jmp" || kw == "jeq"
					|| kw == "jne" || kw == "jgt" || kw == "call" || kw == "not"
					|| kw == "int") {

				tok = strtok(NULL, " ,	");
				Operand op1 = analizaOperanda(tok, suf);

				int op1Descr = op1.adrTip;
				op1Descr <<= 4;
				op1Descr |= op1.regBr;
				op1Descr <<= 1;
				op1Descr |= op1.sufHL;
				tekuca->prevod.push_back(op1Descr);

				if (op1.adrTip != 1 && op1.adrTip != 2) {
					int imDiAd;
					if (!op1.imeSimbola.empty()) {
						Simbol* sim = pretraga(op1.imeSimbola);
						if (sim->sekcija == "CONST") {
							op1.imeSimbola.clear();
							op1.neposrednaVr = sim->offset;
						}
					}
					if (op1.imeSimbola.empty()) {
						imDiAd = op1.neposrednaVr;
					} else {
						Simbol* sim = pretraga(op1.imeSimbola);
						Relokacija rel;
						rel.offset = tekuca->prevod.size();
						bool needRel = true;
						if ((op1.adrTip == 3 || op1.adrTip == 4)
								&& op1.regBr == 7) {
							rel.tip = PCREL;
							int psDif = 1;
							if (suf == 'w')
								psDif++;
							if (sim->sekcija == tekuca->ime) {
								needRel = false;
								imDiAd = sim->offset - rel.offset - psDif;
							} else {
								imDiAd = -psDif;
							}
						} else {
							rel.tip = APS;
							imDiAd = 0;
						}
						if (sim->vidljivost == 'g') {
							rel.vrednost = sim->rbr;
						} else {
							Simbol* simSek = pretraga(sim->sekcija);
							rel.vrednost = simSek->rbr;
							if(needRel)
								imDiAd += sim->offset;
						}
						if(needRel)
							tekuca->relokacije.push_back(rel);
					}
					tekuca->prevod.push_back(imDiAd);
					if (suf == 'w')
						tekuca->prevod.push_back(imDiAd >> 8);
				}

			} else if (kw == "xchg" || kw == "mov" || kw == "add" || kw == "sub"
					|| kw == "mul" || kw == "div" || kw == "cmp" || kw == "and"
					|| kw == "or" || kw == "xor" || kw == "test" || kw == "shl"
					|| kw == "shr") {

				tok = strtok(NULL, " ,	");
				Operand op1 = analizaOperanda(tok, suf);
				tok = strtok(NULL, " ,	");
				Operand op2 = analizaOperanda(tok, suf);

				int op1Descr = op1.adrTip;
				op1Descr <<= 4;
				op1Descr |= op1.regBr;
				op1Descr <<= 1;
				op1Descr |= op1.sufHL;
				tekuca->prevod.push_back(op1Descr);

				if (op1.adrTip != 1 && op1.adrTip != 2) {
					int imDiAd;
					if (!op1.imeSimbola.empty()) {
						Simbol* sim = pretraga(op1.imeSimbola);
						if (sim->sekcija == "CONST") {
							op1.imeSimbola.clear();
							op1.neposrednaVr = sim->offset;
						}
					}
					if (op1.imeSimbola.empty()) {
						imDiAd = op1.neposrednaVr;
					} else {
						Simbol* sim = pretraga(op1.imeSimbola);
						Relokacija rel;
						rel.offset = tekuca->prevod.size();
						bool needRel = true;
						if ((op1.adrTip == 3 || op1.adrTip == 4)
								&& op1.regBr == 7) {
							rel.tip = PCREL;
							int psDif = 2;
							if (suf == 'w')
								psDif++;
							if (op2.adrTip != 1 && op2.adrTip != 2) {
								psDif++;
								if (suf == 'w')
									psDif++;
							}
							if (sim->sekcija == tekuca->ime) {
								needRel = false;
								imDiAd = sim->offset - rel.offset - psDif;
							} else {
								imDiAd = -psDif;
							}
						} else {
							rel.tip = APS;
							imDiAd = 0;
						}
						if (sim->vidljivost == 'g') {
							rel.vrednost = sim->rbr;
						} else {
							Simbol* simSek = pretraga(sim->sekcija);
							rel.vrednost = simSek->rbr;
							if(needRel)
								imDiAd += sim->offset;
						}
						if(needRel)
							tekuca->relokacije.push_back(rel);
					}
					tekuca->prevod.push_back(imDiAd);
					if (suf == 'w')
						tekuca->prevod.push_back(imDiAd >> 8);
				}

				int op2Descr = op2.adrTip;
				op2Descr <<= 4;
				op2Descr |= op2.regBr;
				op2Descr <<= 1;
				op2Descr |= op2.sufHL;
				tekuca->prevod.push_back(op2Descr);

				if (op2.adrTip != 1 && op2.adrTip != 2) {
					int imDiAd;
					if (!op2.imeSimbola.empty()) {
						Simbol* sim = pretraga(op2.imeSimbola);
						if (sim->sekcija == "CONST") {
							op2.imeSimbola.clear();
							op2.neposrednaVr = sim->offset;
						}
					}
					if (op2.imeSimbola.empty()) {
						imDiAd = op2.neposrednaVr;
					} else {
						Simbol* sim = pretraga(op2.imeSimbola);
						Relokacija rel;
						rel.offset = tekuca->prevod.size();
						bool needRel = true;
						if ((op2.adrTip == 3 || op2.adrTip == 4)
								&& op2.regBr == 7) {
							rel.tip = PCREL;
							int psDif = 1;
							if (suf == 'w')
								psDif++;
							if (sim->sekcija == tekuca->ime) {
								needRel = false;
								imDiAd = sim->offset - rel.offset - psDif;
							} else {
								imDiAd = -psDif;
							}
						} else {
							rel.tip = APS;
							imDiAd = 0;
						}
						if (sim->vidljivost == 'g') {
							rel.vrednost = sim->rbr;
						} else {
							Simbol* simSek = pretraga(sim->sekcija);
							rel.vrednost = simSek->rbr;
							if(needRel)
								imDiAd += sim->offset;
						}
						if(needRel)
							tekuca->relokacije.push_back(rel);
					}
					tekuca->prevod.push_back(imDiAd);
					if (suf == 'w')
						tekuca->prevod.push_back(imDiAd >> 8);
				}
			}
		}
	}

	for (int i = 0; i < sekcije.size(); ++i) {
		if (sekcije[i]->ime == ".bss")
			continue;

		izlaz << "#.ret"<< sekcije[i]->ime << endl;

		izlaz << setw(10) << "# Offset" << setw(10) << "Tip" << setw(10) << "Vrednost" << endl;
		for (int j = 0; j < sekcije[i]->relokacije.size(); ++j) {

			izlaz << setw(10) << setfill(' ') << sekcije[i]->relokacije[j].offset;
			izlaz << setw(10) << setfill(' ') << sekcije[i]->relokacije[j].tip;
			izlaz << setw(10) << setfill(' ') << sekcije[i]->relokacije[j].vrednost;
			izlaz << endl;

		}
		izlaz << endl;
	}
	for (int i = 0; i < sekcije.size(); ++i) {
		if (sekcije[i]->ime == ".bss")
			continue;

		izlaz << "#"<< sekcije[i]->ime << endl;

		for (int j = 0; j < sekcije[i]->prevod.size(); ++j) {

			izlaz << setw(2) << setfill('0') << hex << (((int)sekcije[i]->prevod[j]) & 0xff);
			if ((j+1)%16 == 0)
				izlaz << endl;
			else
				izlaz << " ";

		}
		izlaz << endl;
	}

	izlaz.close();

}
