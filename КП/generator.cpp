#include <iostream>
#include <math.h>
#include <string>
#include <fstream>
#include <dirent.h>

using namespace std;

const int TEST_COUNT = 10;

int main(int argc, char * argv[]) {
    if(argc!=2){
            fprintf(stderr, "Usage: {sys.argv[0]} <test directory>\n");
            return 1;
    }
    DIR *test_dir = opendir(argv[1]);
    string d_name = argv[1];
    string f_name = d_name + "/0.t";
    string line = "";
    for(int i = 1 ; i <= 1 ; i++){
        f_name[f_name.size()-3] = i + '0';
        ofstream create_tests (f_name);
        line = "1000 1000\n";
        create_tests << line;
        line = "";
        for(int j = 0 ; j < 1000 ; j++){
            for(int k = 0; k < 1000; k++){
                if(j == 500 && k != 0 && k != 999){
                    line = line + '#';
                }
                else {
                    line = line + '.';
                }
            }
            line = line + '\n';
            create_tests << line;
            line = "";
        }
        line = "5\n";
        create_tests << line;
        line = "";
        for(int j = 0 ; j < 200 ; j++){
            line = "1 1 800 800\n";
            create_tests << line;
            line = "";
        }
        create_tests.close();
    }
}
