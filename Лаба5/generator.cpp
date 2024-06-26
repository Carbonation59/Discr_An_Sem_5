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
    int numb_of_rows;
    int numb_of_subst;
    int number;
    int MAX, MIN;
    char alpha;
    string line = "";
    for(int i = 1 ; i <= TEST_COUNT ; i++){
        f_name[f_name.size()-3] = i + '0';
        ofstream create_tests (f_name);
        numb_of_rows = rand() % 51;
        numb_of_subst = rand() % 51;
        MAX = max(numb_of_rows, numb_of_subst);
        MIN = min(numb_of_rows, numb_of_subst);
        numb_of_rows = MAX;
        numb_of_subst = MIN;
        if(numb_of_rows > 9){
            alpha = numb_of_rows/10 + '0';
            line = line + alpha;
        }
        alpha = numb_of_rows%10 + '0';
        line = line + alpha;
        line = line + ' ';
        if(numb_of_subst > 9){
            alpha = numb_of_subst/10 + '0';
            line = line + alpha;
        }
        alpha = numb_of_subst%10 + '0';
        line = line + alpha;
        line = line + '\n';
        create_tests << line;
        line = "";
        for(int j = 0 ; j < numb_of_rows ; j++){
            for(int k = 0; k <= numb_of_subst; k++){
                number = rand() % 51;
                if(number > 9){
                    alpha = number/10 + '0';
                    line = line + alpha;
                }
                alpha = number%10 + '0';
                line = line + alpha;
                line = line + ' ';
            }
            line = line + '\n';
            create_tests << line;
            line = "";
        }
        create_tests.close();
    }
}
