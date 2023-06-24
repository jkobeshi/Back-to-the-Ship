// Project Identifier: 950181F63D0A883F183EC0A5CC67B19928FE896A

#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <stack>
#include <queue>
#include <getopt.h>
using namespace std;

class ShipData {
public:
	ShipData() {
		floors = 0; 
		sqDim = 0;
		input_format = '\0';
		output_format = 'M';
		dataStruc = '\0';
		numCalls = 0;
	}
	void read_data();
	void get_options(int argc, char** argv);
	void run();
	void path();
	void print();

private:
	uint32_t floors, sqDim;
	char input_format;
	char output_format;
	char dataStruc;
	uint32_t numCalls;
	vector<vector<vector<char>>> map;
	vector<vector<vector<char>>> disc;
	vector<uint32_t> start;
	vector<uint32_t> hang;
};

void ShipData::print() {
	if (output_format == 'M') {
		cout << "Start in level " << start[0]
			<< ", row " << start[1]
			<< ", column " << start[2] << "\n";
		for (uint32_t i = 0; i < floors; i++) {
			cout << "//level " << i << "\n";
			for (uint32_t j = 0; j < sqDim; j++) {
				for (uint32_t k = 0; k < sqDim; k++) {
					cout << map[i][j][k];
				}
				cout << "\n";
			}
		}
	}
	if (output_format == 'L') {
		cout << "//path taken\n";
		if (hang.size() == 3) {
			vector<uint32_t> loc(3);
			loc = { start[0], start[1], start[2] };
			while (map[loc[0]][loc[1]][loc[2]] != 'H') {
				cout << "(" << loc[0] << "," << loc[1] << "," << loc[2] << ","
					<< map[loc[0]][loc[1]][loc[2]] << ")\n";
				if (map[loc[0]][loc[1]][loc[2]] == 'n') {
					loc[1]--;
				}
				else if (map[loc[0]][loc[1]][loc[2]] == 'e') {
					loc[2]++;
				}
				else if (map[loc[0]][loc[1]][loc[2]] == 's') {
					loc[1]++;
				}
				else if (map[loc[0]][loc[1]][loc[2]] == 'w') {
					loc[2]--;
				}
				else if (int(map[loc[0]][loc[1]][loc[2]] - '0') >= 0 &&
					uint32_t(map[loc[0]][loc[1]][loc[2]] - '0') < floors) {
					loc[0] = (map[loc[0]][loc[1]][loc[2]] - '0');
				}
			}
		}
	}
}

void ShipData::get_options(int argc, char** argv) {
	int gotopt;
	int option_index = 0;
	option long_opts[] = {
		{ "stack", no_argument, nullptr, 's' },
		{ "queue", no_argument, nullptr, 'q' },
		{ "output", required_argument, nullptr, 'o'},
		{ "help", no_argument, nullptr, 'h'}};
	while ((gotopt = getopt_long(argc, argv, "o:sqh", long_opts, &option_index)) != -1) {
		switch (gotopt) {
		case 's':
			dataStruc = 's';
			numCalls++;
			break;

		case 'q':
			dataStruc = 'q';
			numCalls++;
			break;

		case 'o':
			output_format = *optarg;
			break;

		case 'h':
			cout << "help";
			exit(0);
			break;
		}
	}
	if (!(output_format == 'M' || output_format == 'L')) {
		cout << "Invalid output mode specified\n";
		exit(1);
	}
	if (dataStruc == '\0') {
		cout << "No routing mode specified\n";
		exit(1);
	}
	if (numCalls != 1) {
		cout << "Multiple routing modes specified\n";
		exit(1);
	}
}
void ShipData::read_data() {
	cin >> input_format >> floors >> sqDim >> ws;
	string input;
	map.resize(floors, vector<vector<char>>(sqDim, vector<char>(sqDim, '.')));
	if (input_format == 'M') {
		char ch; uint32_t k = 0;
		while (cin >> ch) {
			if (ch == '/') {
				getline(cin, input);
			}
			else {
				map[k][0][0] = ch;
				uint32_t j = 0, i = 0;
				while (j < sqDim && i < sqDim) {
					if (!(i == 0 && j == 0)) {
						cin >> map[k][j][i];
					}
					if (map[k][j][i] == 'S') {
						start = { k,j,i };
					}
					if (!(map[k][j][i] == 'S' || map[k][j][i] == '#' ||
						map[k][j][i] == 'E' || map[k][j][i] == '.' || map[k][j][i] == 'H')) {
						cout << "Invalid map character\n";
						exit(1);
					}
					i++;
					if (i >= sqDim) {
						j++; i = 0;
					}
				}
				k++;
			}
		}
	}
	if (input_format == 'L') {
		char ch;
		uint32_t lvl, row, col; char res;
		while (cin >> ch) {
			if (ch == '/') {
				getline(cin, input);
			}
			else {
				cin >> lvl >> ch >> row >> ch >> col >> ch >> res >> ch;
				if (!(res == 'S' || res == '#' || res == 'E' 
						|| res == '.' || res == 'H')) {
					cout << "Invalid map character\n";
					exit(1);
				}
				if (!(int(lvl) >= 0 && lvl < floors)) {
					cout << "Invalid map level\n";
					exit(1);
				}
				if (!(int(row) >= 0 && row < sqDim)) {
					cout << "Invalid map row\n";
					exit(1);
				}
				if (!(int(col) >= 0 && col < sqDim)) {
					cout << "Invalid map column\n";
					exit(1);
				}
				map[lvl][row][col] = res;
				if (res == 'S') {
					start = { lvl, row, col };
				}
			}
		}
	}
}

void ShipData::run() {
	disc.resize(floors, vector<vector<char>>(sqDim, vector<char>(sqDim, 'd')));
	deque<vector<uint32_t>> searchCon;
	vector<uint32_t> curLoc(3);
	searchCon.push_back({ start[0], start[1], start[2] });
	disc[start[0]][start[1]][start[2]] = 'S';
	while (searchCon.size() > 0) {	
		if (dataStruc == 'q') {
			curLoc = { searchCon.front()[0], searchCon.front()[1], searchCon.front()[2] };
			searchCon.pop_front();
		}
		if (dataStruc == 's') {
			curLoc = { searchCon.back()[0], searchCon.back()[1], searchCon.back()[2] };
			searchCon.pop_back();
		}
		if ((int(curLoc[1]) - 1 >= 0) && (map[curLoc[0]][curLoc[1] - 1][curLoc[2]] != '#' 
				    && disc[curLoc[0]][curLoc[1] - 1][curLoc[2]] == 'd')) {
			searchCon.push_back({ curLoc[0], curLoc[1] - 1, curLoc[2] });
			disc[curLoc[0]][curLoc[1] - 1][curLoc[2]] = 'n';
			if (map[searchCon.back()[0]][searchCon.back()[1]][searchCon.back()[2]] == 'H') {
				hang = { searchCon.back()[0], searchCon.back()[1], searchCon.back()[2] };
				break;
			}
		}
		if ((curLoc[2] + 1 < sqDim) && (map[curLoc[0]][curLoc[1]][curLoc[2] + 1] != '#' 
					&& disc[curLoc[0]][curLoc[1]][curLoc[2] + 1] == 'd')) {
			searchCon.push_back({ curLoc[0], curLoc[1], curLoc[2] + 1 });
			disc[curLoc[0]][curLoc[1]][curLoc[2] + 1] = 'e';
			if (map[searchCon.back()[0]][searchCon.back()[1]][searchCon.back()[2]] == 'H') {
				hang = { searchCon.back()[0], searchCon.back()[1], searchCon.back()[2] };
				break;
			}
		}
		if ((curLoc[1] + 1 < sqDim) && (map[curLoc[0]][curLoc[1] + 1][curLoc[2]] != '#' 
					&& disc[curLoc[0]][curLoc[1] + 1][curLoc[2]] == 'd')) {
			searchCon.push_back({ curLoc[0], curLoc[1] + 1, curLoc[2] });
			disc[curLoc[0]][curLoc[1] + 1][curLoc[2]] = 's';
			if (map[searchCon.back()[0]][searchCon.back()[1]][searchCon.back()[2]] == 'H') {
				hang = { searchCon.back()[0], searchCon.back()[1], searchCon.back()[2] };
				break;
			}
		}
		if ((int(curLoc[2]) - 1 >= 0) && (map[curLoc[0]][curLoc[1]][curLoc[2] - 1] != '#' 
					&& disc[curLoc[0]][curLoc[1]][curLoc[2] - 1] == 'd')) {
			searchCon.push_back({ curLoc[0], curLoc[1], curLoc[2] - 1 });
			disc[curLoc[0]][curLoc[1]][curLoc[2] - 1] = 'w';
			if (map[searchCon.back()[0]][searchCon.back()[1]][searchCon.back()[2]] == 'H') {
				hang = { searchCon.back()[0], searchCon.back()[1], searchCon.back()[2] };
				break;
			}
		}
		if (map[curLoc[0]][curLoc[1]][curLoc[2]] == 'E') {
			for (uint32_t i = 0; i < floors; i++) {
				if (map[i][curLoc[1]][curLoc[2]] == 'E' && disc[i][curLoc[1]][curLoc[2]] == 'd') {
					searchCon.push_back({ i, curLoc[1], curLoc[2] });
					disc[i][curLoc[1]][curLoc[2]] = char(curLoc[0] + '0');
				}
			}
		}
	}
}

void ShipData::path() {
	if (hang.size() == 3) {
		while (!(hang[0] == start[0] && hang[1] == start[1] && hang[2] == start[2])) {
			if (disc[hang[0]][hang[1]][hang[2]] == 'n') {
				map[hang[0]][hang[1] + 1][hang[2]] = 'n';
				hang[1]++;
			}
			else if (disc[hang[0]][hang[1]][hang[2]] == 'e') {
				map[hang[0]][hang[1]][hang[2] - 1] = 'e';
				hang[2]--;
			}
			else if (disc[hang[0]][hang[1]][hang[2]] == 's') {
				map[hang[0]][hang[1] - 1][hang[2]] = 's';
				hang[1]--;
			}
			else if (disc[hang[0]][hang[1]][hang[2]] == 'w') {
				map[hang[0]][hang[1]][hang[2] + 1] = 'w';
				hang[2]++;
			}
			else if (int(disc[hang[0]][hang[1]][hang[2]] - '0') >= 0 &&
				uint32_t(disc[hang[0]][hang[1]][hang[2]] - '0') < floors) {
				uint32_t g = uint32_t(disc[hang[0]][hang[1]][hang[2]] - '0');
				map[g][hang[1]][hang[2]] = char(hang[0] + '0');
				hang[0] = g;
			}
		}
	}
}

int main(int argc, char *argv[]) {
	ios_base::sync_with_stdio(false);
	ShipData exe;
	exe.get_options(argc, argv);
	exe.read_data();
	exe.run();
	exe.path();
	exe.print();
	return 0;
}