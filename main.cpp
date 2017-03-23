#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

using namespace std;

/*
 *  Function Declaration
 */

void print_help();
void parse_argv(int size, char ** argv, vector<string>*args, vector<string>* opts);
string get_path();
bool confirm(string message);

/*
 * Constants
 */
const int OPTIONS_SIZE = 3;

const string OPTIONS[OPTIONS_SIZE] = {
        "-f",
        "-h",
        "--help"
};

void parse_argv(int size, char ** argv, vector<string>*args, vector<string>*opts){
    int i;
    int j;

    printf("inside of parse_argv\n");
    for(i = 1; i < size; i++){
        string arg(argv[i]);
        printf("arg: %s \n", arg.c_str());
        for(j = 0; j < OPTIONS_SIZE; j++){
            printf("compare: %s and %s \n", arg.c_str(), OPTIONS[j].c_str() );
            if(arg.compare(OPTIONS[j]) == 0){
                printf("opts push_back\n");
                opts->push_back(arg);
                break;
            }
        }
        printf("compare: %d and %d \n", j, OPTIONS_SIZE );
        if(j == OPTIONS_SIZE){
            printf("args push_back\n");
            args->push_back(arg);
        }
    }
    printf("end of parse_argv\n");
}

void print_help(){
    printf("This is a command for coping files \n");
    printf("To copy some file write: сp <filename> <newfilename> \n");
    printf("To copy several files to some directory write: сp <filename1> <filename2> <directory> \n");
    printf("Additional options: \n");
    printf("-f                silently re-write file with existing name\n");
}

string get_path(){
    const  int SIZE = 1024;
    char buffer[SIZE];
    char *path = getcwd(buffer, sizeof(buffer));
    string s_cwd;


    if (path)
    {
        s_cwd = path;
        s_cwd += "/";
        printf("current path: %s \n", s_cwd.c_str());
        return s_cwd;
    }
    else {
        printf("cp: cannot determine current directory\n");
        exit(EXIT_FAILURE);
    }
}

bool confirm(string message){
    printf("%s[y/n]:\n", message.c_str());
    string answer;
    cin >> answer;
    return answer.compare("y") == 0;
}


int main(int argc, char **argv) {
    int c;
    FILE *in,*out;
    struct stat info;
    string path;
    vector<string> args;
    vector<string> opts;

    parse_argv(argc, argv, &args, &opts);

    bool f_option = false;

    printf("opts.size(): %lu \n", opts.size());
    printf("args.size(): %lu \n", args.size());

    for(unsigned long i = 0; i < opts.size(); i++){
        printf("%s \n",opts.at(i).c_str() );

        if(opts.at(i).compare("--help") == 0 || opts.at(i).compare("-h") == 0){
            print_help();
            exit(EXIT_SUCCESS);
        }
        if(opts.at(i).compare("-f") == 0){
            f_option = true;
        }
    }

    if (args.size() < 2){
        printf("missing file operand\n");
        printf("Try 'cp --help' for more information.\n");
        exit(EXIT_FAILURE);
    }
    else if(args.size() == 2){
        string string1;
        string string2;

        path = get_path();
        string1 = path + args.at(0);
        string2 = path + args.at(1);
        in = fopen( string1.c_str(), "r" );
        out = fopen( string2.c_str(), "r" );

        if(in==NULL)
        {
            printf("cp: cannot stat '%s': No such file or directory\n",args.at(0).c_str());
            return EXIT_FAILURE;
        }

        if(out != NULL && !f_option){
            printf("cp: File with name '%s' already exist\n", args.at(1).c_str());

            if(!confirm("Would you like to re-write file?")) {
                exit(EXIT_SUCCESS);
            }

        }

        out = fopen( string2.c_str(), "w" );

        while((c=getc(in))!=EOF)
            putc(c,out);

        fclose(in);
        fclose(out);
    }
    else{
        string string1;
        string string2;

        path = get_path();
        printf("more then 2 args\n");




        for(unsigned long i = 0; i <args.size() -1; i++){
            string1 = path + args.at(i);
            string2 = path + args.at(args.size() - 1) +"/"+ args.at(i);

            printf("string1: %s\n", string1.c_str());
            printf("string2: %s\n", string2.c_str());
            in = fopen( string1.c_str(), "r" );
            out = fopen( string2.c_str(), "r" );

            if(in==NULL)
            {
                printf("cp: cannot stat '%s': No such file or directory\n",args.at(i).c_str());
                return EXIT_FAILURE;
            }

            if(out != NULL && !f_option){
                printf("cp: File with name '%s' already exist\n", (args.at(args.size() - 1)+"/"+args.at(i)).c_str());

                if(!confirm("Would you like to re-write file?")) {
                    continue;
                }

            }

            out = fopen( string2.c_str(), "w" );

            if(out == NULL){
                printf("cp: target '%s' is not a directory\n", args.at(args.size() - 1).c_str());
                return EXIT_FAILURE;
            }

            while((c=getc(in))!=EOF)
                putc(c,out);

            fclose(in);
            fclose(out);
        }
    }

    return EXIT_SUCCESS;
}