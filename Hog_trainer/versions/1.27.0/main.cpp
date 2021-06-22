#include <iostream>
#include <windows.h>
#include <conio.h>

#include <fstream>
#include <cmath>
#include <io.h>
#include <cstdlib>
#include <filesystem>

#include "header.h"
#include "random.h"
#include "util.h"

const int GOAL_SCORE = 100;
const unsigned long long PI[] = {3141592653, 5897932384, 6264338327, 9502884197, 1693993751,
                           582097494, 4592307816, 4062862089, 9862803482, 5342117067, 9000000000};

int piDigitAt(int i) {
    long long divider = pow(10, (i / 10 + 1) * 10 - i - 1);
    return PI[i / 10] / divider % 10;
}

unsigned long long Q[100][100][11] = {};
unsigned long long N[100][100][11] = {};
unsigned long long old_Q[100][100][11] = {};
unsigned long long old_N[100][100][11] = {};
int DELTA[200][3] = {};
int delta_index = 0;

int turn_num_sum = 0;
long long effective_time_consumed = 0;
unsigned long long total_time_consumed = 0;
unsigned long long episode_counter = 0;
unsigned long long total_episode_counter = 0;

string policy_name = "untitled";
unsigned long long train_time = 0;
double win_rate = 0;

unsigned long long EPISODE_PER_SEC = 200000;
double DEFAULT_EPSILON = 0.01;
double EPSILON = 0.01;
int DYNAMIC_EPSILON = 0;
unsigned long long EPISODE_PER_EPOCH = 1000000;
double CONSERVATIVITY = 0.01;
int CONSERVATIVE_LEARNING = 0;
int REVERSE_MODE = 0;

void clean() {
    system("cls");
    header("Hog Trainer","1.27.0");
    cout << "\n";
}

// Mutating
int roll_a_die(int side) {
    return 1 + randomNumber(side);
}

// Mutating
int roll_dice(int num_rolls=1, int dice_side=6) {
    int result = 0;
    int current = 0;
    for (int i = 0; i < num_rolls; i++) {
        current = roll_a_die(dice_side);
        if (current == 1) {
            return 1; //pig_out
        }
        result += current;
    }
    return result;
}

// Mutating
int free_bacon(int score1) {
    if (score1 == 0) {
        return 3;
    }
    int square = score1 * score1;
    int result = 9;
    int digit;
    while (square != 0) {
        digit = square % 10;
        square /= 10;
        if (digit < result) {
            result = digit;
        }
    }
    return result + 3;
}

// Mutating
int take_turn(int num_rolls, int score1, int dice_side) {
    if (num_rolls == 0) {
        return free_bacon(score1);
    } else {
        return roll_dice(num_rolls, dice_side);
    }
}

// Mutating

bool is_swap(int score0, int score1) {
    int p0left = score0;
    int p0right = score0 % 10;
    int p1left = score1;
    int p1right = score1 % 10;
    while (p0left >= 10) {
        p0left /= 10;
    }
    while (p1left >= 10) {
        p1left /= 10;
    }
    return p0left * p0right == p1left * p1right;
}


// Mutating
//Return whether the player gets an extra turn.
//player_score:   The total score of the current player.
//opponent_score: The total score of the other player.
bool more_boar(int score0, int score1) {
    int score0d2 = score0;
    int score1d2 = score1;
    while (score0d2 > 99) {
        score0d2 /= 10;
    }
    while (score1d2 > 99) {
        score1d2 /= 10;
    }
    return score0d2 / 10 < score1d2 / 10 and score0d2 % 10 < score1d2 % 10;
}

// Mutating

bool pig_pass(int score0, int score1) {
    return (score1 - score0) == 1 or (score1 - score0) == 2;
}

// Mutating
bool time_trot(int dice, int round) {
    return round % 8 == dice;
}

// Mutating
//bool is_extra(int score0, int score1, ) {
//	return pig_pass(score0, score1) || more_boar(score0, score1);
//}

int other(int player) {
    return 1 - player;
}

// Mutating
/*
int feral_hog(int curr_roll, int last_roll) {
	if (abs(curr_roll - last_roll) == 2) {
		return 3;
	}
	return 0;
}
*/

// Mutating
int play(int (*strategy0) (int, int), int (*strategy1) (int, int), int first) {

    int score0 = 0;
    int score1 = 0;

    int turn = first;
    int round = 0;
    int curr_roll = 0;
    int dice_side = 6;
    bool extra = false;


    int turn_num = 0;


    while (score0 < GOAL_SCORE && score1 < GOAL_SCORE) {
        round++;
        if (turn == 0) {
            curr_roll = strategy0(score0, score1);
            score0 += take_turn(curr_roll, score1, dice_side);
            if (more_boar(score0, score1) || (!extra && time_trot(curr_roll, round))) {
                turn = other(turn);
                extra = true;
                dice_side = 8;
            } else {
                extra = false;
                dice_side = 6;
            }
        } else {
            curr_roll = strategy1(score1, score0);
            score1 += take_turn(curr_roll, score0, dice_side);
            if (more_boar(score1, score0) || (!extra && time_trot(curr_roll, round))) {
                turn = other(turn);
                extra = true;
                dice_side = 8;
            } else {
                extra = false;
                dice_side = 6;
            }
        }
        if (DYNAMIC_EPSILON) {
            turn_num++;
        }
        turn = other(turn);
    }
    if (DYNAMIC_EPSILON) {
        turn_num_sum += turn_num;
    }
    return score1 > score0;
}

int always_roll_4(int score0, int score1) {
    return 4;
}

int always_roll_6(int score0, int score1) {
    return 6;
}

double average_win_rate(int (*strategy) (int, int), int (*baseline) (int, int)) {
    int win_count = 0;
    for (int i = 0; i < 100000; i++) {
        win_count += other(play(strategy, baseline, 0));
    }
    for (int i = 0; i < 100000; i++) {
        win_count += other(play(strategy, baseline, 1));
    }
    return (double)win_count / 200000;
}

/*
int bacon_strategy(int score0, int score1) {
	int margin = 8;
	int num_rolls = 4;
	if (free_bacon(score1) >= margin) {
		return 0;
	}
	return num_rolls;
}

int swap_strategy(int score0, int score1) {
	int margin = 8;
	int num_rolls = 4;
	if (free_bacon(score1) + score0 < score1) {
		if (is_swap(free_bacon(score1) + score0, score1) || free_bacon(score1) >= margin) {
			return 0;
		} else {
			return num_rolls;
		}
	} else if (free_bacon(score1) + score0 > score1) {
		if (is_swap(free_bacon(score1) + score0, score1)) {
			return num_rolls;
		} else if (free_bacon(score1) >= margin) {
			return 0;
		} else {
			return num_rolls;
		}
	} else {
		if (free_bacon(score1) >= margin) {
			return 0;
		} else {
			return num_rolls;
		}
	}
}
*/

//-----------IO----------//

void saveN(string path) {
    ofstream outfile;
    outfile.open(path, ios::out);
    for (int score0 = 0; score0 < 100; score0++) {
        for (int score1 = 0; score1 < 100; score1++) {
            for (int choice = 0; choice < 11; choice++) {
                outfile << N[score0][score1][choice] << ' ';
            }
            outfile << ' ';
        }
        outfile << '\n';
    }
    outfile.close();
}

void saveQ(string path) {
    ofstream outfile;
    outfile.open(path, ios::out);
    for (int score0 = 0; score0 < 100; score0++) {
        for (int score1 = 0; score1 < 100; score1++) {
            for (int choice = 0; choice < 11; choice++) {
                outfile << Q[score0][score1][choice] << ' ';
            }
            outfile << ' ';
        }
        outfile << '\n';
    }
    outfile.close();
}

void loadN() {
    mkdir("./strategy");
    ifstream infile;
    infile.open("./strategy/N");
    if (!infile) {
        saveN("./strategy/N");
    } else {
        for (int score0 = 0; score0 < 100; score0++) {
            for (int score1 = 0; score1 < 100; score1++) {
                for (int choice = 0; choice < 11; choice++) {
                    infile >> N[score0][score1][choice];
                }
            }
        }
    }
    infile.close();
}

void loadQ() {
    mkdir("./strategy");
    ifstream infile;
    infile.open("./strategy/Q");
    if (!infile) {
        cout << "q not exist";
        saveQ("./strategy/Q");
    } else {
        for (int score0 = 0; score0 < 100; score0++) {
            for (int score1 = 0; score1 < 100; score1++) {
                for (int choice = 0; choice < 11; choice++) {
                    infile >> Q[score0][score1][choice];
                }
            }
        }
    }
    infile.close();
}

void loadInfo() {
    mkdir("./strategy");
    ifstream infile;
    infile.open("./strategy/Info");
    string parser;
    if (!infile) {
        ofstream outfile;
        outfile.open("./strategy/Info", ios::out);
        outfile << "policy_name = " << policy_name << endl;
        outfile << "train_time = " << train_time << endl;
        outfile << "win_rate = " << win_rate << endl;
        outfile.close();
    } else {
        while (!infile.eof()) {
            infile >> parser;
            if (parser == "policy_name") {
                infile >> parser;
                if (parser == "=") {
                    infile >> policy_name;
                }
            } else if (parser == "train_time") {
                infile >> parser;
                if (parser == "=") {
                    infile >> train_time;
                }
            } else if (parser == "win_rate") {
                infile >> parser;
                if (parser == "=") {
                    infile >> win_rate;
                }
            }
        }
    }
    infile.close();
}

void loadConfig() {
    mkdir("./config");
    ifstream infile;
    infile.open("./config/machine");
    string parser;
    if (!infile) {
        ofstream outfile;
        outfile.open("./config/machine", ios::out);
        outfile << "EPISODE_PER_SEC = " << EPISODE_PER_SEC << endl;
        outfile << "DEFAULT_EPSILON = " << DEFAULT_EPSILON << endl;
        outfile << "EPSILON = " << EPSILON << endl;
        outfile << "DYNAMIC_EPSILON = " << DYNAMIC_EPSILON << endl;
        outfile << "EPISODE_PER_EPOCH = " << EPISODE_PER_EPOCH << endl;
        outfile << "CONSERVATIVITY = " << CONSERVATIVITY << endl;
        outfile << "CONSERVATIVE_LEARNING = " << CONSERVATIVE_LEARNING << endl;
        outfile << "REVERSE_MODE = " << REVERSE_MODE << endl;
        outfile.close();
    } else {
        while (!infile.eof()) {
            infile >> parser;
            if (parser == "EPISODE_PER_SEC") {
                infile >> parser;
                if (parser == "=") {
                    infile >> EPISODE_PER_SEC;
                }
            } else if (parser == "DEFAULT_EPSILON") {
                infile >> parser;
                if (parser == "=") {
                    infile >> DEFAULT_EPSILON;
                }
            } else if (parser == "EPSILON") {
                infile >> parser;
                if (parser == "=") {
                    infile >> EPSILON;
                }
            } else if (parser == "DYNAMIC_EPSILON") {
                infile >> parser;
                if (parser == "=") {
                    infile >> DYNAMIC_EPSILON;
                }
            } else if (parser == "EPISODE_PER_EPOCH") {
                infile >> parser;
                if (parser == "=") {
                    infile >> EPISODE_PER_EPOCH;
                }
            } else if (parser == "CONSERVATIVITY") {
                infile >> parser;
                if (parser == "=") {
                    infile >> CONSERVATIVITY;
                }
            } else if (parser == "CONSERVATIVE_LEARNING") {
                infile >> parser;
                if (parser == "=") {
                    infile >> CONSERVATIVE_LEARNING;
                }
            } else if (parser == "REVERSE_MODE") {
                infile >> parser;
                if (parser == "=") {
                    infile >> REVERSE_MODE;
                }
            }
        }
    }
    infile.close();
}

void saveInfo() {
    ofstream outfile;
    mkdir("./strategy");
    outfile.open("./strategy/Info", ios::out);
    outfile << "policy_name = " << policy_name << endl;
    outfile << "train_time = " << train_time << endl;
    outfile << "win_rate = " << win_rate << endl;
    outfile.close();
}

void saveConfig() {
    ofstream outfile;
    mkdir("./config");
    outfile.open("./config/machine", ios::out);
    outfile << "EPISODE_PER_SEC = " << EPISODE_PER_SEC << endl;
    outfile << "DEFAULT_EPSILON = " << DEFAULT_EPSILON << endl;
    outfile << "EPSILON = " << EPSILON << endl;
    outfile << "DYNAMIC_EPSILON = " << DYNAMIC_EPSILON << endl;
    outfile << "EPISODE_PER_EPOCH = " << EPISODE_PER_EPOCH << endl;
    outfile << "CONSERVATIVITY = " << CONSERVATIVITY << endl;
    outfile << "CONSERVATIVE_LEARNING = " << CONSERVATIVE_LEARNING << endl;
    outfile << "REVERSE_MODE = " << REVERSE_MODE << endl;
    outfile.close();
}

//------------------------

void clearDelta() {
    delta_index = 0;
    for (int history = 0; history < 100; history++) {
        for (int loc_index = 0; loc_index < 3; loc_index++) {
            DELTA[history][loc_index] = -1;
        }
    }
}

void clearQ() {
    for (int score0 = 0; score0 < 100; score0++) {
        for (int score1 = 0; score1 < 100; score1++) {
            for (int choice = 0; choice < 11; choice++) {
                Q[score0][score1][choice] = 1;
            }
        }
    }
}

void clearN() {
    for (int score0 = 0; score0 < 100; score0++) {
        for (int score1 = 0; score1 < 100; score1++) {
            for (int choice = 0; choice < 11; choice++) {
                N[score0][score1][choice] = 1;
            }
        }
    }
}

void normalizeQN(int scale) {
    for (int score0 = 0; score0 < 100; score0++) {
        for (int score1 = 0; score1 < 100; score1++) {
            for (int choice = 0; choice < 11; choice++) {
                if (N[score0][score1][choice] >= (long long)scale) {
                    Q[score0][score1][choice] = (int)((double)Q[score0][score1][choice] / (double)N[score0][score1][choice] * scale);
                    N[score0][score1][choice] = scale;
                }
            }
        }
    }
    mkdir("./strategy");
    saveQ("./strategy/Q");
    saveN("./strategy/N");
}

void conserve() {
    for (int score0 = 0; score0 < 100; score0++) {
        for (int score1 = 0; score1 < 100; score1++) {
            for (int choice = 0; choice < 11; choice++) {
                old_Q[score0][score1][choice] = Q[score0][score1][choice];
                old_N[score0][score1][choice] = N[score0][score1][choice];
            }
        }
    }
}

void recover() {
    for (int score0 = 0; score0 < 100; score0++) {
        for (int score1 = 0; score1 < 100; score1++) {
            for (int choice = 0; choice < 11; choice++) {
                Q[score0][score1][choice] = old_Q[score0][score1][choice];
                N[score0][score1][choice] = old_N[score0][score1][choice];
            }
        }
    }
}

double Qvalue(int score0, int score1, int choice) {
    return (double)Q[score0][score1][choice] / (double)N[score0][score1][choice];
}

string dumpCurrentStrategy() {
    double highestQ;
    int bestChoice;
    double q;
    ofstream outfile;
    mkdir("./strategy");
    outfile.open("./strategy/Q_Matrix", ios::out);
    outfile << "[";
    for (int score0 = 0; score0 < 100; score0++) {
        outfile << "[";
        for (int score1 = 0; score1 < 100; score1++) {
            highestQ = 0;
            bestChoice = 0;
            for (int choice = 0; choice < 11; choice++) {
                q = Qvalue(score0, score1, choice);
                if (q > highestQ) {
                    highestQ = q;
                    bestChoice = choice;
                }
            }
            outfile << bestChoice;
            if (score1 != 99) {
                outfile << ", ";
            }
        }
        outfile << "]";
        if (score0 != 99) {
            outfile << ", \n";
        }
    }
    outfile << "]\n";
    outfile.close();
    mkdir("./outputs/");
    string path_str = "./outputs/" + localTime();
    const char* path = toPath(path_str);
    mkdir(path);
    path_str[1] = '\\';
    path_str[9] = '\\';
    system(toPath("copy .\\strategy " + path_str + "\\"));
    Sleep(500);
    path_str[1] = '/';
    path_str[9] = '/';
    return path_str;
}

/*
 * Load everything from local
 */
void setup() {
    clearQ();
    clearN();
    clearDelta();
    loadQ();
    loadN();
    loadInfo();
    loadConfig();
}

//---------Dynamic Programming---------//
/*
struct gameState{
	int score0;
	int score1;
	bool extra;
	double winrate;
};

gameState
*/


//-------------Q-Learning--------------//

// This one is intentionally buggy.
int playerStrategy(int score0, int score1) {
    int decision = 0;
    cin >> decision;
    if (decision > 10) {
        decision = 10;
    }
    return decision;
}

/*
 * static strategy, based on loaded Q-N table only.
 */
int QStrategy(int score0, int score1) {
    int decision = 0;
    double highestQ = Qvalue(score0, score1, 0);
    double q;
    for (int i = 1; i <= 10; i++) {
        q = Qvalue(score0, score1, i);
        if (q > highestQ) {
            decision = i;
            highestQ = q;
        }
    }
    return decision;
}

int old_QStrategy(int score0, int score1) {
    int decision = 0;
    double highestQ = old_Q[score0][score1][0] / old_N[score0][score1][0];
    double q;
    for (int i = 1; i <= 10; i++) {
        q = old_Q[score0][score1][i] / old_N[score0][score1][i];
        if (q > highestQ) {
            decision = i;
            highestQ = q;
        }
    }
    return decision;
}

/*
 * self-improving strategy, based on loaded Q-N table only.
 */
int Q0Strategy(int score0, int score1) {
    int decision = 0;
    double highestQ = Qvalue(score0, score1, 0);
    double q;
    for (int i = 1; i <= 10; i++) {
        q = Qvalue(score0, score1, i);
        if (q > highestQ) {
            decision = i;
            highestQ = q;
        }
    }
    DELTA[delta_index][0] = score0;
    DELTA[delta_index][1] = score1;
    DELTA[delta_index++][2] = decision;
    return decision;
}

/*
 * self-improving strategy, based on loaded Q-N table and epsilon.
 */
int QLStrategy(int score0, int score1) {
    int decision;
    if(trueOrFalse((int)((DYNAMIC_EPSILON ? EPSILON : DEFAULT_EPSILON) * 10000), 10000)) {
        int original_decision = QStrategy(score0, score1);
        do {
            decision = randomNumber(11);
        } while (decision == original_decision);
    } else {
        decision = QStrategy(score0, score1);
    }
    DELTA[delta_index][0] = score0;
    DELTA[delta_index][1] = score1;
    DELTA[delta_index++][2] = decision;
    return decision;
}

void updateQN(int reward) {
    int score0, score1, decision;
    for (int i = 0; i < delta_index; i++) {
        score0 = DELTA[i][0];
        score1 = DELTA[i][1];
        decision = DELTA[i][2];
        ++N[score0][score1][decision];
        if (reward == 1) {
            ++Q[score0][score1][decision];
        }
    }
}

double evolveToNextEpoch(long long episode) {
    long long kilo_episode = episode / 1000;
    effective_time_consumed = 0;
    total_time_consumed = GetCurrentTime();
    int reward;
    loadQ();
    loadN();
    turn_num_sum = 0;
    total_episode_counter = 0;
    episode_counter = 0;
    if (CONSERVATIVE_LEARNING) {
        conserve();
    }
    for (long long i = 1; i <= episode; i++) {
        //Run i (==EPISODE) episodes.
        effective_time_consumed -= GetCurrentTime();
        //Timer begins.
        reward = play(QLStrategy, QStrategy, i % 2) == 0 ? 1 : -1;
        REVERSE_MODE ? reward *= -1 : NULL ;
        //play() a game and get a REWARD.
        effective_time_consumed += GetCurrentTime();
        //Timer stops.
        ++(CONSERVATIVE_LEARNING ? episode_counter : total_episode_counter);
        //Update EPISODE_COUNTER if in conservative mode, TOTAL_EPISODE_COUNTER otherwise.
        updateQN(reward);
        //Update Q & N tables.
        delta_index = 0;
        //Reset DELTA_INTEX to position 0 so that it is ready for next episode.
        if (i % kilo_episode == 0) {
            //Every MILLI (==EPISODE/1000) episodes, refresh UI and progress percentage.
            cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b               ";
            cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b";
            cout << "学习进度：" << ((double)i / episode) * 100 << "%...";
        }
        if (DYNAMIC_EPSILON && i % 100000 == 0) {
            //If in dynamic mode, every 100000 episodes, update EPSILON, and reset TURN_NUM_SUM to 0.
            EPSILON = 1 / ((double)turn_num_sum / 2 / 100000);
            turn_num_sum = 0;
        }
        if (CONSERVATIVE_LEARNING) {
            //If in conservative mode...
            if (episode_counter == EPISODE_PER_EPOCH || i == episode) {
                //If reach a checkpoint or training is over...
                if (average_win_rate(QStrategy, old_QStrategy) < 0.5 + CONSERVATIVITY) {
                    //If win rate doesn't pass the threshold, recover() both Q & N tables to the lastest version.
                    recover();
                } else {
                    //Else, conserve() both Q & N tables, update total_episode_counter, and reset EPISODE_COUNTER to 0.
                    conserve();
                    total_episode_counter += episode_counter;
                    episode_counter = 0;
                }
            }
        }
    }
    train_time += total_episode_counter;
    saveQ("./strategy/Q");
    saveN("./strategy/N");
    saveInfo();
    total_time_consumed = GetCurrentTime() - total_time_consumed;
    return (double) (100 * effective_time_consumed) / total_time_consumed;
}

// Mutating
void pve(int (*player) (int, int), int (*ai) (int, int)) {

    int turn = 0;
    int score0 = 0;
    int score1 = 0;
    int curr_roll = 0;
    int dice_side = 6;
    int round = 0;
    bool extra = false;

    int score_this_turn = 0;

    while (score0 < GOAL_SCORE && score1 < GOAL_SCORE) {
        round++;
        cout << "第" << round << "回合，当前分数：" << score0 << " vs. " << score1 << " ，";
        cout << "A.I.有 " << ((turn == 1) ? (Qvalue(score1, score0, QStrategy(score1, score0)))
                                         : (1 - Qvalue(score0, score1, QStrategy(score0, score1)))) * 100 << "% 的把握拿下这一局" <<endl;

        if (turn == 0) {
            cout << "你的回合\n";
            curr_roll = player(score0, score1);
            if (curr_roll <= -1) {
                cout << "你掷了 " << curr_roll << " 个骰子，A.I.将你踢出了房间……\n";
                return;
            }
            cout << "你掷了 " << curr_roll << " 个骰子，A.I.觉得这一步的胜率为 " << 100 * Qvalue(score0, score1, curr_roll) << "%\n";
            score_this_turn = take_turn(curr_roll, score1, dice_side);
            cout << "你获得 " << score_this_turn << " 分\n";
            score0 += score_this_turn;
            if (more_boar(score0, score1)) {
                cout << "连击(MB)！你额外获得一个回合！\n";
                turn = other(turn);
                extra = true;
                dice_side = 8;
            } else if (!extra && time_trot(curr_roll, round)) {
                cout << "连击(TT)！你额外获得一个回合！\n";
                turn = other(turn);
                extra = true;
                dice_side = 8;
            } else {
                extra = false;
                dice_side = 6;
            }
        } else {
            cout << "A.I.回合\n";
            curr_roll = ai(score1, score0);
            cout << "A.I.掷了 " << curr_roll << " 个骰子，A.I.觉得这一步的胜率为 " << 100 * Qvalue(score1, score0, curr_roll) << "%\n";
            score_this_turn = take_turn(curr_roll, score0, dice_side);
            cout << "A.I.获得 " << score_this_turn << " 分\n";
            score1 += score_this_turn;
            if (more_boar(score1, score0)) {
                cout << "连击(MB)！A.I.额外获得一个回合！\n";
                turn = other(turn);
                extra = true;
                dice_side = 8;
            } else if (!extra && time_trot(curr_roll, round)) {
                cout << "连击(TT)！A.I.额外获得一个回合！\n";
                turn = other(turn);
                extra = true;
                dice_side = 8;
            } else {
                extra = false;
                dice_side = 6;
            }
        }
        turn = other(turn);
    }
    cout << score0 << " vs. " << score1 << " ";
    if (score0 < score1) {
        cout << "你被A.I.的完美运营理所应当地碾压了 (っ*′Д`)っ\n";
    } else {
        cout << "你一口欧气，A.I.的运营变得一文不值 (っ*′Д`)っ\n";
    }
}

//-----------------UI-------------------//

void showMenu(string currentStage) {
    char input;
    string message;
    long long parameter = 0;
    refreshStage:;
    message = "";
    if (currentStage == "mainMenu") {
        clean();
        cout << "这里是萌萌哒的QLearner~ ╰ (￣▽￣)╭ \n";
        cout << "欢迎回来学习~ (￣_,￣ )\n";
        cout << "游戏规则：Spring 21 Contest\n";
        cout << "当前时间：";
        displayLocalTime();
        cout << endl;
        cout << "菜单:[0]更新日志\n";
        cout << "     [1]学习器设置\n";
        cout << "     [2]当前策略的信息\n";
        cout << "     [3]粗略计算当前策略胜率\n";
        cout << "     [4]挑战模式(pve)\n";
        cout << "     [5]对决模式(eve)\n";
        cout << "     [6]学习！！\n";
        input = requireSelection("0123456");
        if (input == '0') {
            currentStage = "log";
        } else if (input == '1') {
            currentStage = "config";
        } else if (input == '2') {
            currentStage = "strategyInfo";
        } else if (input == '3') {
            currentStage = "getWinRate";
        } else if (input == '4') {
            currentStage = "pve";
        } else if (input == '5') {
            currentStage = "eve";
        } else if (input == '6') {
            currentStage = "qLearn";
        }
        goto refreshStage;
    } else if (currentStage == "log") {
        clean();
        cout << "更新日志:\n";
        cout << "  1.26.0: 1.新增功能——标准化，以提高训练的极限强度。\n";
        cout << "  1.26.1: 1.现在标准化只对N>=1000的QN-pair生效。\n";
        cout << "  1.26.2: 1.现在标准化参数N = 100000。\n";
        cout << "  1.26.3: 1.更新至2021 Spring规则。\n";
        cout << "          2.优化了AI在挑战模式的评价算法。\n";
        cout << "  1.26.4: 1.修复了一个挑战模式AI评价算法的bug。\n";
        cout << "          2.新增了always_rolls(6)胜率测试。\n";
        cout << "  1.26.5: 1.新增了反转训练模式——练出一个只输不赢的人工智障，能输给它算你狠(狗头)。\n";
        cout << "  1.27.0: 1.源代码重构。\n";
        cout << "          2.修复了学习算法中的一个bug，现在Q值收敛得更快。\n";
        cout << "  1.27.1: (开发中)1.AI对决模式\n";
        cout << "          (开发中)2.更新至2021 Summer规则。\n";
        cout << "          (开发中)3.DP算法。\n";
        cout << endl;
        cout << "菜单:[1]爪巴回主面板\n";
        input = requireSelection("1");
        if (input == '1') {
            currentStage = "mainMenu";
            goto refreshStage;
        }
    } else if (currentStage == "config") {
        clean();
        cout << "当前机器算力：" << EPISODE_PER_SEC << "EPS\n";
        cout << "Epsilon：" << ((DYNAMIC_EPSILON == 1) ? EPSILON : DEFAULT_EPSILON) << endl;
        cout << "动态Epsilon：" << ((DYNAMIC_EPSILON == 1) ? "开" : "关") << endl;
        cout << "Epoch长度：" << EPISODE_PER_EPOCH << endl;
        cout << "保守系数：" << CONSERVATIVITY << endl;
        cout << "保守模式：" << ((CONSERVATIVE_LEARNING == 1) ? "开" : "关") << endl;
        if (REVERSE_MODE) cout << "警告：反向学习已启动！" << endl;
        cout << endl;
        cout << "菜单:[1]爪巴回主面板\n";
        cout << "     [2]" << ((DYNAMIC_EPSILON == 1) ? "关闭" : "开启") << "动态Epsilon\n";
        cout << "     [3]切换学习模式为 " << ((CONSERVATIVE_LEARNING == 1) ? "自由" : "保守") << "\n";
        cout << "     [4]切换学习模式为 " << ((REVERSE_MODE == 1) ? "正向" : "反向") << "\n";
        cout << "     [5]学习！！\n";
        input = requireSelection("12345");
        if (input == '1') {
            currentStage = "mainMenu";
            goto refreshStage;
        } else if (input == '2') {
            DYNAMIC_EPSILON = 1 - DYNAMIC_EPSILON;
            saveConfig();
            goto refreshStage;
        } else if (input == '3') {
            CONSERVATIVE_LEARNING = 1 - CONSERVATIVE_LEARNING;
            saveConfig();
            goto refreshStage;
        } else if (input == '4') {
            REVERSE_MODE = 1 - REVERSE_MODE;
            saveConfig();
            goto refreshStage;
        } else if (input == '5') {
            currentStage = "qLearn";
            goto refreshStage;
        }
    } else if (currentStage == "strategyInfo") {
        strategyInfo:;
        clean();
        cout << "当前策略名：" << policy_name << endl;
        cout << "训练时长：两年半(X) " << train_time / 1000000 << " MEpisodes\n";
        cout << "标准胜率(vs. always_roll(4))：" << win_rate << "，有待更新\n";
        if (input == '2' && !message.empty()) {
            cout << "策略导出成功，路径：" << message << "\n";
        } else if (input == '3' && !message.empty()) {
            cout << "标准化完成。旧策略备份路径：" << message << "\n";
        }
        cout << endl;
        cout << "菜单:[1]爪巴回主面板\n";
        cout << "     [2]导出策略\n";
        cout << "     [3]标准化(N=100000)\n";
        cout << "     [4]学习！！\n";
        input = requireSelection("1234");
        if (input == '1') {
            currentStage = "mainMenu";
            goto refreshStage;
        } else if (input == '2') {
            message = dumpCurrentStrategy();
            goto strategyInfo;
        } else if (input == '3') {
            message = dumpCurrentStrategy();
            normalizeQN(100000);
            goto strategyInfo;
        } else if (input == '4') {
            currentStage = "qLearn";
            goto refreshStage;
        }
    } else if (currentStage == "getWinRate") {
        clean();
        cout << "当前胜率只有 ...";
        cout << "\b\b\b" << average_win_rate(QStrategy, always_roll_4) * 100 << "% (vs. always_roll(4)), ";
        cout << average_win_rate(QStrategy, always_roll_6) * 100 << "% (vs. always_roll(6)) 哦~\n";
        cout << "真是有够蔡的呢~ (っ*′Д`)っ\n";
        cout << "请开始你的表演 (￣_,￣ )\n";
        cout << endl;
        cout << "菜单:[1]爪巴回主面板\n";
        cout << "     [2]再算一次叭>_<\n";
        cout << "     [3]学习！！\n";
        input = requireSelection("123");
        if (input == '1') {
            currentStage = "mainMenu";
            goto refreshStage;
        } else if (input == '2') {
            currentStage = "getWinRate";
            goto refreshStage;
        } else if (input == '3') {
            currentStage = "qLearn";
            goto refreshStage;
        }
    } else if (currentStage == "pve") {
        clean();
        cout << "听说你想打一架？放*马*过来吧骚年~\n";
        cout << "请开始你的表演 (￣_,￣ )\n\n";
        pve(playerStrategy, QStrategy);
        cout << endl;
        cout << "菜单:[1]爪巴回主面板\n";
        cout << "     [2]再打一架叭>_<\n";
        cout << "     [3]学习！！\n";
        input = requireSelection("123");
        if (input == '1') {
            currentStage = "mainMenu";
            goto refreshStage;
        } else if (input == '2') {
            currentStage = "pve";
            goto refreshStage;
        } else if (input == '3') {
            currentStage = "qLearn";
            goto refreshStage;
        }
    } else if (currentStage == "eve") { //todo
        clean();
        cout << "听说你想打一架？放*马*过来吧骚年~\n";
        cout << "请开始你的表演 (￣_,￣ )\n\n";
        pve(playerStrategy, QStrategy);
        cout << endl;
        cout << "菜单:[1]爪巴回主面板\n";
        cout << "     [2]再打一架叭>_<\n";
        cout << "     [3]学习！！\n";
        input = requireSelection("123");
        if (input == '1') {
            currentStage = "mainMenu";
            goto refreshStage;
        } else if (input == '2') {
            currentStage = "pve";
            goto refreshStage;
        } else if (input == '3') {
            currentStage = "qLearn";
            goto refreshStage;
        }
    } else if (currentStage == "qLearn") {
        clean();
        string task_name;
        long long speed_test = 0; // debug only
        cout << "Q-Learning模块准备就绪\n请指定训练时长 (＠_＠;)\n";
        cout << "若是第一次使用本机学习，请选[2]以测试机器算力（手动滑稽）\n";
        cout << endl;
        cout << "菜单:[1]3秒真男人（" << 2 * EPISODE_PER_SEC / 1000 / (double)1000 << "MEpisodes）\n";
        cout << "     [2]死线前的最后一分钟（测速专用）（" << 60 * EPISODE_PER_SEC / 1000 / (double)1000 << "MEpisodes）\n";
        cout << "     [3]Berkeley Time（" << 600 * EPISODE_PER_SEC / 1000 / (double)1000 << "MEpisodes）\n";
        cout << "     [4]水课上一半翘了（" << 1800 * EPISODE_PER_SEC / 1000 / (double)1000 << "MEpisodes）\n";
        cout << "     [5]一课时Lecture（" << 3600 * EPISODE_PER_SEC / 1000 / (double)1000 << "MEpisodes）\n";
        cout << "     [6]两课时Discussion（" << 7200 * EPISODE_PER_SEC / 1000 / (double)1000 << "MEpisodes）\n";
        cout << "     [7]四课时Lab（" << 14400 * EPISODE_PER_SEC / 1000 / (double)1000 << "MEpisodes）\n";
        cout << "     [8]六课时Back-to-back finals（" << 21600 * EPISODE_PER_SEC / 1000 / (double)1000 << "MEpisodes）\n";
        cout << "     [9]九课时，做梦都在学习（" << 32400 * EPISODE_PER_SEC / 1000 / (double)1000 << "MEpisodes）\n";
        cout << "     [0]提前适应996（" << 43200 * EPISODE_PER_SEC / 1000 / (double)1000 << "MEpisodes）\n";
        cout << "     [其他数字]叉路自助餐（必须是1000的倍数）\n";
        cin >> parameter;
        switch (parameter) {
            case 1:
                parameter = 2 * EPISODE_PER_SEC / 1000 * 1000;
                task_name = "3秒真男人";
                break;
            case 2:
                parameter = 60 * EPISODE_PER_SEC / 1000 * 1000;
                speed_test = 1;
                task_name = "DDL前的最后一分钟";
                break;
            case 3:
                parameter = 600 * EPISODE_PER_SEC / 1000 * 1000;
                task_name = "Berkeley Time";
                break;
            case 4:
                parameter = 1800 * EPISODE_PER_SEC / 1000 * 1000;
                task_name = "水课上一半翘了";
                break;
            case 5:
                parameter = 3600 * EPISODE_PER_SEC / 1000 * 1000;
                task_name = "一课时Lecture";
                break;
            case 6:
                parameter = 7200 * EPISODE_PER_SEC / 1000 * 1000;
                task_name = "两课时Discussion";
                break;
            case 7:
                parameter = 14400 * EPISODE_PER_SEC / 1000 * 1000;
                task_name = "四课时Lab";
                break;
            case 8:
                parameter = 21600 * EPISODE_PER_SEC / 1000 * 1000;
                task_name = "六课时Back-to-back finals";
                break;
            case 9:
                parameter = 32400 * EPISODE_PER_SEC / 1000 * 1000;
                task_name = "九课时，做梦都在学习";
                break;
            case 0:
                parameter = 43200 * EPISODE_PER_SEC / 1000 * 1000;
                task_name = "提前适应996";
                break;
            default:
                if (parameter < 1000) {
                    parameter = 1000;
                }
                parameter = parameter / 1000 * 1000;
                task_name = "叉路自助餐";
                break;
        }
        clean();
        cout << "Q-Learning运行中……o(≧口≦)o\n";
        cout << "正在学习：" << task_name << endl;
        cout << "Episode计数：" << parameter / (double)1000000 << "MEpisodes\n";
        cout << "开始时间：";
        displayLocalTime();
        if (speed_test) {
            speed_test = GetCurrentTime();
        }
        double efficiency = evolveToNextEpoch(parameter);
        if (speed_test) {
            speed_test = (GetCurrentTime() - speed_test) / 1000;
            EPISODE_PER_SEC = (parameter / speed_test);
            saveConfig();
        }
        cout << "︿(￣︶￣)︿\n";
        cout << "学习完成！";
        displayLocalTime();
        cout << "学习效率：" << efficiency << "% (强化学习算法运行占全部训练时间的比例)\n";
        cout << "收获率：" << (double)total_episode_counter / parameter * 100 << "% (计入当前策略的episode比例)\n";
        cout << "有效Episode计数：" << total_episode_counter / (double)1000000 << "MEpisodes\n";
        cout << endl;
        cout << "菜单:[1]返回主面板\n";
        cout << "     [2]再来一发！\n";
        input = requireSelection("12");
        if (input == '1') {
            currentStage = "mainMenu";
            goto refreshStage;
        } else if (input == '2') {
            currentStage = "qLearn";
            goto refreshStage;
        }
    }

}

int main(int argc, char** argv) {
    cout << "诶诶我被启动了？等等我~~Σ(っ°Д°;)っ\n";
    setup();
    showMenu("mainMenu");
    return 0;
}

