#include "shogi.hpp"
#include <cstdlib>
#include <ctime> 

void saveLoadTest(Shogi s){
	s.EasyBoardPrint();
	cout << s.round << "\n";
	s.LoadGame(s.SaveGame());
	s.EasyBoardPrint();
	cout << s.round << "\n";
}

string vectorHexdump(vector<char> k){
	string s;
	char charset[17] = "0123456789ABCDEF";
	for(char a : k){
		unsigned char c = a;
		s.append(1, charset[c/16]);
		s.append(1, charset[c%16]);
	}
	return s;
}

vector<char> vectorHexload(string s){
	vector<char> c;
	c.reserve(s.length()/2);
	for(int i=0;i<s.length();i+=2){
		char v = 0;
		v += (s[i] - (s[i] >= 'A' ? ('A'-10) : ('0')))*16;
		v += (s[i+1] - (s[i+1] >= 'A' ? ('A'-10) : ('0')));
		c.push_back(v);
	}
	return c;
}


void random_playing_test_visible(int n){
	Shogi s;
	s.Init();

	for(int i=0;i<n;i++){
		
		vector<int> moveList = s.FetchMove(3);
		if(moveList.size() == 0){
			vector<int> moveList = s.FetchMove(1);
			s.EasyBoardPrint(); cout << "\n";
			s.PrintAttackBoard(); cout << "\n";
			for(int move: moveList){
				printMove(move);
			}
			system("pause");
			break;
		}
		int move = moveList[rand() % moveList.size()];
		printMove(move);

		cout << "\n------------------------------- " << s.round+1 << "\n";
		s.MakeMove(move);
//		system("cls");
	}
}

void handtest(){
	Shogi s;
	s.Init();

	while(true){
		vector<int> moveList = s.FetchMove(3);
		bool finish = false;
		int move = 0;

		s.EasyBoardPrint(); cout << "\n";

		while(!finish){
			int presuji, predan, newsuji, newdan, upgrade, playing;
			cin >> presuji >> predan >> newsuji >> newdan >> upgrade >> playing;
			if(playing){
				move = genMove(presuji, genPos(newsuji, newdan), upgrade, playing);
			}else{
				move = genMove(genPos(presuji, predan), genPos(newsuji, newdan), upgrade, playing);
			}
			for(int m : moveList){
				if(move == m){
					finish = true;
					break;
				}
			}
		}

		
		cout << "\n";
		cout << "\n------------------------------- " << s.round+1 << "\n";
		s.MakeMove(move);
	}
}

void speedtest(int testtimes){
	int roundCount = 0;
	for(int i=0;i<testtimes;i++){
		Shogi s;
		s.Init();
		for(int i=0;i<500000;i++){
			vector<int> movelist = s.FetchMove(3);
			if(movelist.size() == 0){
				roundCount += s.round;
				break;
			}
			int move = movelist[rand() % movelist.size()];
			s.MakeMove(move);
		}
	}
	cout << "finish.\n";
	cout << "Average round = " << (double)roundCount / (double)testtimes << "\n";
	cout << "Total Compute round = " << roundCount << "\n";
}

void fuzztest(int testtimes){
	for(int i=0;i<testtimes;i++){
		random_playing_test_visible(500);
		cout << "==============================\n";
		cout << "==============================\n\n";
	}
}

vector<char> randomlyChange(vector<char> k, int c){
	vector<char> r;
	r.reserve(k.size());
	for(char d : k){
		char f = d + (char)(rand() % (2*c+1)) + (char)c;
		r.push_back(f);
	}
	return r;
}

int fight(ShogiBot sente, ShogiBot gote, int maxRound, int depth, double randomP){
	Shogi s;
	s.Init();
	while(s.round < maxRound){
//		cout << s.round << "\n";
		s.EasyBoardPrint();
		if(s.round % 2 == 0){
			int move = sente.decideMove(s, depth, randomP);
			if(move == -1){
				return -1;
			}
			s.MakeMove(move);
		}else{
			int move = gote.decideMove(s, depth, randomP);
			if(move == -1){
				return 1;
			}
			s.MakeMove(move);
		}
	}
	return 0;
}

vector<char> crossover(vector<char> a, vector<char> b){
	vector<char> c;
	c.reserve(a.size());

	for(int i=0;i<a.size();i++){
		int dice = rand() % 20;
		if(dice < 9) c.push_back(a[i]);
		else if(dice < 18) c.push_back(b[i]);
		else c.push_back((char)(rand() % 256));
	}
	return c;
}

void findbestvector(int N, int T, double R){
	cout << "Start, generate vector\n";
	cout << "N = " << N << ", T = " << T << ", R = " << R << "\n";
	ShogiBot* players = new ShogiBot[N];
	ShogiBot* tmp = new ShogiBot[N];
	string specialVector = "054B322864647850505050507D7D0103FFFF0101FEFF010101010303FD03FDFD640AF6FBFBFBFBFBFBFB0A0A03030A05FF0A0F0F0F0F0F5A0505050505050505050505050505011E";
	for(int i=0;i<N;i++){
		players[i].SpecialInit(randomlyChange(vectorHexload(specialVector), 5));
	}
	vector<char> bestVector;
	for(int i=0;i<T;i++){
		int winRecord[1000] = {0};
		cout << "- Tier " << i+1 << " start -\n";
		for(int i=0;i<N;i++){
			cout << i << ": " << vectorHexdump(players[i].judgingVector) << "\n";
		}
		for(int i=0;i<2*N;i++){
			int sente = rand() % N;
			int gote = (sente + rand() % (N-1) + 1) % N;
			int result = fight(players[sente], players[gote], 500, 2, R);
			winRecord[sente] += result;
			winRecord[gote] -= result;
			if(result == 1)cout << sente << " win " << gote << "\n";
			else if(result == -1) cout << gote << " win " << sente << "\n";
			else cout << sente << " tie " << gote << "\n";
		}
		cout << "- Tier " << i+1 << " result -\n";
		
		int mostWin = 0;
		for(int i=0;i<N;i++){
			cout << vectorHexdump(players[i].judgingVector) << ": " << winRecord[i] << "\n";
			if(winRecord[i] > mostWin){
				mostWin = winRecord[i];
				bestVector = players[i].judgingVector;
			}
		}
		double lastGate = 0.6;
		int lastCount = 0;
		int threhold = mostWin;

		while((double)(lastCount)/(double)(N) < lastGate){
			for(int i=0;i<N;i++){
				int dice = rand() % 6;
				if(winRecord[i] >= threhold and dice > 2){
					tmp[lastCount++] = players[i];
					winRecord[i] = -600;
				}
			}
			threhold--;
		}

		while(lastCount < N){
			int a = rand() % N;
			int b = (a + rand() % (N-1) + 1) % N;
			tmp[lastCount++].SpecialInit(
				crossover(players[a].judgingVector, players[b].judgingVector)
			);
		}
		for(int i=0;i<N;i++){
			players[i] = tmp[i];
			tmp[i].judgingVector.clear();
		}
	}

	cout << "best vector = " << vectorHexdump(bestVector) << "\n";

	delete players;
	delete tmp;
}

void botTest(){
	ShogiBot sente;
	ShogiBot gote;
	sente.RandomInit();
	gote.RandomInit();

	vector<char> testV1 = vectorHexload("0A503E526E3E8140025B7F5E1D89090809D9EA8E0EC074070C070D0EB330BEF86CFC557D2B88D14174557C45C5104169041F1C481A3DBB8CC3C849E18E0E1211122D3A3486F67F29");
	vector<char> testV2 = vectorHexload("054B322864647850505050507D7D0103FFFF0101FEFF010101010303FD03FDFD640AF6FBFBFBFBFBFBFB0A0A03030A05FF0A0F0F0F0F0F5A0505050505050505050505050505011E");

	sente.SpecialInit(testV1);
	gote.SpecialInit(testV2);
	vectorHexdump(testV1);
	vectorHexdump(testV2);
	Shogi s;
	s.Init();

	int result = fight(sente, gote, 1000, 3, 1);
	cout << result << "\n";
	
}




int main(){
	srand(time(0));
//	findbestvector(100, 50, 0.35);
	botTest();
}