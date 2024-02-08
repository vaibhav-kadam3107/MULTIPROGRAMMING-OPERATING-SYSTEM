#include <bits/stdc++.h>
using namespace std;

struct PCB{
    int job_id; 
    int TTL;  //total time limit
    int TLL;  //total line limit
    int TTC;  //total time counter
    int TLC;  //total line counter
};

struct PCB P;

int ptr;  //page table register
int visited[30]; //pages
char M[300][4];  //main memory
char IR[4];  //Instruction Register
char R[4];//General Purpose Register
int IC;//Instruction Count
bool C;//Toggle Register
int SI;//System Interrupt
int VA;  //virtual address
int RA;  //real address
int PI;  //program interrupt
int TI;  //time interrupt
int EM;  //error message

fstream inFile;
fstream outFile;

unordered_map<int, string> errors = {{0, "No Error"},
        {1, "Out of Data"}, 
        {2, "Line Limit Exceeded"},
        {3, "Time Limit Exceeded"},
        {4, "Operation Code Error"},
        {5, "Operand Error"}, 
        {6, "Invalid Page Fault"}};

void init();
void LOAD();
void EXECUTE();
void MOS();
int ALLOCATE();
int ADDRESSMAP(int);
int TERMINATE(int);

void init(){
    for (int i = 0; i < 300; i++){
        for (int j = 0; j < 4; j++){
            M[i][j] = ' ';
        }
    }
    for (int i = 0; i < 30; i++){
        visited[i] = 0;
    }

    IR[4] = {'-'};
    R[4] = {'-'};
    IC = 0;
    C = false;
    ptr = 0;
    VA = 0;
    PI = 0;
    TI = 0;
    EM = 0;
}

int ALLOCATE(){
    return (rand() % 30);
}

int ADDRESSMAP(int va){
    int pte = ptr * 10 + va / 10;
    cout<<"PTE: "<<pte<<endl;
    string temp = "";
    if (M[pte][0] == '*'){
        cout << "Page Fault" << endl;
        return -1;
    }else{
        for (int i = 0; i < 4; i++){
            if (M[pte][i] != ' '){
                temp += M[pte][i];
            }
        }
        int d=(stoi(temp) * 10) + (va % 10);
        cout<<"d="<<d<<endl;
        return ((stoi(temp) * 10) + (va % 10));
    }
}

int TERMINATE(int Code){
    cout<<"\n"<<errors[Code]<<endl;
    outFile<<"\nProgram Terminated abnormally"<<"\t\t";
    outFile<<errors[Code]<<endl;
}

void MOS(){
    if (SI == 1){
        string line;
        getline(inFile, line);

        if (line[0] == '$' && line[1] == 'E' && line[2] == 'N' && line[3] == 'D'){
            EM = 1;
            TERMINATE(1);
            return;
        }

        int frame = ALLOCATE();
        cout<<"frame = "<<frame<<endl;
        while (visited[frame] != 0){
            frame = ALLOCATE();
        }

        visited[frame] = 1;

        int i = ptr;
        i = i * 10;
        cout << "\n\nPTR = " << ptr << endl; // PTR = page table register
        while (M[i][0] != '*'){
            i++;
        }

        int temp = frame / 10;

        M[i][0] = ' ';
        M[i][1] = ' ';
        M[i][2] = temp + 48;
        M[i][3] = frame % 10 + 48;

        int l = 0;
        frame = frame * 10;
        for (int i = 0; i < line.length() && line.length() < 40; i++){
            M[frame][l++] = line[i];
            if (l == 4){
                l = 0;
                frame += 1;
            }
        }
    }else if (SI == 2){
        P.TLC += 1;
        if (P.TLC > P.TLL){
            EM = 2;
            TERMINATE(2);
            return;
        }

        int add = IR[2] - 48;
        add = (add * 10);

        int ra = ADDRESSMAP(add);

        if (ra != -1){
            string out;

            for (int i = 0; i < 10; i++){
                for (int j = 0; j < 4; j++){
                    out += M[ra][j];
                }
                ra += 1;
            }
            outFile << out << "\n";
        }else{
            EM = 6;
            TERMINATE(6);
            PI = 3;
        }
    }else if (SI == 3){
        outFile<<"\nProgram Terminated Successfully"<<"\n";
        outFile<< "JOBID: "<<P.job_id<<"\tIC: "<<IC<<"\tToggle : "<<C<<"\tTLC: "<<P.TLC<<"\tTTC: "<<P.TTC<<"\tTTL: "<<P.TTL<<"\tTLL: " <<P.TLL<<"\tIR: ";

        for (int i = 0; i < 4; i++){
            outFile << IR[i];
        }
        outFile<<"\n\n";
    }
}

void EXECUTE(){
    while(1){
        if (PI != 0 || TI != 0 || EM != 0){
            outFile<< "JOBID: "<<P.job_id<<"\tIC: "<<IC<<"\tToggle : "<<C<<"\tTLC: "<<P.TLC<<"\tTTC: "<<P.TTC<<"\tTTL: "<<P.TTL<<"\tTLL: " <<P.TLL<<"\tIR: ";
            for (int i = 0; i < 4; i++){
                outFile << IR[i];
            }
            outFile<<"\n\n";
            break;
        }
        cout<<"ic="<<IC<<endl;
        RA = ADDRESSMAP(IC);
        cout<<"RA "<<RA<<endl;

        if (M[RA][0] != 'H' && (!isdigit(M[RA][2]) || !isdigit(M[RA][3]))){
            EM = 5;
            TERMINATE(EM);
            outFile<< "JOBID: "<<P.job_id<<"\tIC: "<<IC<<"\tToggle : "<<C<<"\tTLC: "<<P.TLC<<"\tTTC: "<<P.TTC<<"\tTTL: "<<P.TTL<<"\tTLL: " <<P.TLL<<"\tIR: ";
            for (int i = 0; i < 4; i++){
                outFile << IR[i];
            }
            outFile<<"\n\n";
        }
        for (int i = 0; i < 4; i++){
            IR[i] = M[RA][i];
        }
        IC++;

        int add = IR[2] - 48;
        add = (add * 10) + (IR[3] - 48);
        cout<<"add "<<add<<endl;

        if ((IR[0] == 'G' && IR[1] == 'D') || (IR[0] == 'S' && IR[1] == 'R')){
            P.TTC += 2;
        }else{
            P.TTC += 1;
        }
        if (P.TTC > P.TTL){
            EM = 3;
            TI = 2;
            TERMINATE(EM);
            outFile<< "JOBID: "<<P.job_id<<"\tIC: "<<IC<<"\tToggle : "<<C<<"\tTLC: "<<P.TLC<<"\tTTC: "<<P.TTC<<"\tTTL: "<<P.TTL<<"\tTLL: " <<P.TLL<<"\tIR: ";
            for (int i = 0; i < 4; i++){
                outFile << IR[i];
            }
            outFile<<"\n\n";
            break;
        }
        if (IR[0] == 'L' && IR[1] == 'R'){
            int ra = ADDRESSMAP(add);
            cout<<"ra "<<ra<<endl;
            if (ra == -1){
                EM = 6; 
                TERMINATE(6);
            }else{
                for (int i = 0; i < 4; i++){
                    R[i] = M[ra][i];
                }
            }
        }else if (IR[0] == 'S' && IR[1] == 'R'){
            int ra = ADDRESSMAP(add);
            if (ra != -1){
                for (int i = 0; i < 4; i++){
                    M[ra][i] = R[i];
                }
            }else{
                int frame = ALLOCATE();
                while (visited[frame] != 0){
                    frame = ALLOCATE();
                }

                visited[frame] = 1;

                int i = ptr;
                i = i * 10;
                while (M[i][0] != '*'){
                    i++;
                }

                int temp = frame / 10;

                M[i][0] = ' ';
                M[i][1] = ' ';
                M[i][2] = temp + 48;
                M[i][3] = frame % 10 + 48;

                frame = frame * 10;
                for (int i = 0; i < 4; i++){
                    M[frame][i] = R[i];
                }
            }
        }else if (IR[0] == 'C' && IR[1] == 'R'){
            int flag = 0;
            int ra = ADDRESSMAP(add);
            if (ra == -1){
                EM = 6;
                TERMINATE(6);
            }else{
                for (int i = 0; i < 4; i++){
                    if (R[i] != M[ra][i]){
                        flag = 1;
                    }
                }

                if (flag == 1){
                    C = false;
                }else{
                    C = true;
                }
            }
        }else if (IR[0] == 'B' && IR[1] == 'T'){
            if (C == true){
                IC = add;
            }
        }else if (IR[0] == 'G' && IR[1] == 'D'){
            SI = 1;
            MOS();
        }else if (IR[0] == 'P' && IR[1] == 'D'){
            SI = 2;
            MOS();
        }else if (IR[0] == 'H'){
            SI = 3;
            MOS();
            break;
        }else{
            EM = 4;
            TERMINATE(EM);
            outFile<< "JOBID: "<<P.job_id<<"\tIC: "<<IC<<"\tToggle : "<<C<<"\tTLC: "<<P.TLC<<"\tTTC: "<<P.TTC<<"\tTTL: "<<P.TTL<<"\tTLL: " <<P.TLL<<"\tIR: ";
            for (int i = 0; i < 4; i++){
                outFile << IR[i];
            }
            outFile<<"\n\n";
            break;
        }
    }
}

void LOAD(){
    cout << "\nReading Data..." << endl;
    int m = 0;
    string line;
    while (getline(inFile, line)){
        string str = "";
        for (int i = 0; i < 4; i++){
            str += line[i];
        }
        if (str == "$AMJ"){
            init();
            ptr = ALLOCATE();
            cout<<"PTR : "<<ptr<<endl;
            for (int i = ptr * 10; i < ptr * 10 + 10; i++){
                for (int j = 0; j < 4; j++){
                    M[i][j] = '*';
                }
            }
            visited[ptr] = 1;
            // Initialize PCB
            string jobid_str = "";
            string TTL_str = "";
            string TLL_str = "";
            for (int i = 0; i < 4; i++){
                jobid_str += line[i + 4];
                TTL_str += line[i + 8];
                TLL_str += line[i + 12];
            }
            P.job_id = stoi(jobid_str);
            cout<<"job-id: "<<P.job_id<<endl;
            P.TTL = stoi(TTL_str);
            P.TLL = stoi(TLL_str);
            P.TLC = 0;
            P.TTC = 0;
        }else if (str == "$DTA"){
            EXECUTE();
        }else if (str == "$END"){
            for (int i = 0; i < 300; i++){
                cout << "M[" << i << "] - ";
                for (int j = 0; j < 4; j++){
                    cout << M[i][j]<<" ";
                }
                cout << endl;
            }
            cout << "\n******   END/HALT   ******\n\n"<< endl;
        }else{
            int frameNo = ALLOCATE();
            cout<<"Frame no: "<<frameNo<<endl;
            while (visited[frameNo] != 0){
                frameNo = ALLOCATE(); 
                cout<<"Frame no: "<<endl;
            }
            visited[frameNo] = 1;
            int i = ptr;
            while (M[i][0] != '*'){
                i++;
            }
            int temp = frameNo / 10; // 17/10 = 1

            M[i][0] = ' ';
            M[i][1] = ' ';
            M[i][2] = temp + 48;  //48 here means 0
            M[i][3] = frameNo % 10 + 48;

            int len = 0;
            for (int i = frameNo * 10; i < frameNo * 10 + 10 && len < line.length(); i++){
                for (int j = 0; j < 4 && len < line.length(); j++){
                    if (line[len] == 'H'){
                        M[i][j] = line[len++];
                        break;
                    }else{
                        M[i][j] = line[len++];
                    }
                }
            }
            line.clear();
        }
    }
}

int main(){
    inFile.open("input2.txt", ios::in);
    outFile.open("output2.txt", ios::out);
    LOAD();
    return 0;
}