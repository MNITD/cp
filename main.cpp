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
int get_file_status(string & path);

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

    for(i = 1; i < size; i++){
        string arg(argv[i]);
        for(j = 0; j < OPTIONS_SIZE; j++){
            if(arg.compare(OPTIONS[j]) == 0){
                opts->push_back(arg);
                break;
            }
        }
        if(j == OPTIONS_SIZE){
            args->push_back(arg);
        }
    }
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

/* Check status of file: whether it dir, not dir or does not exist
 * @param path Full path of the file
 * @return code of status 1 - directory, 0 - file, -1 - does not exist
 */

int get_file_status(string & path){
    struct stat info;

    if(stat( path.c_str(), &info ) != 0) {
        return -1;
    } else if(info.st_mode & S_IFDIR){
        return 1;
    }
    return 0;
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

    for(unsigned long i = 0; i < opts.size(); i++){
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
        string full_path1;
        string full_path2;

        path = get_path();
        full_path1 = path + args.at(0);
        full_path2 = path + args.at(1);
        in = fopen( full_path1.c_str(), "r" );
        out = fopen( full_path2.c_str(), "r" );

        if(in==NULL) {
            printf("cp: cannot stat '%s': No such file or directory\n",args.at(0).c_str());
            return EXIT_FAILURE;
        } else if ( get_file_status(full_path1) == 1 ){
            printf("cp: copying directories is not supported\n");
            return EXIT_FAILURE;
        }

        if(get_file_status(full_path2) == 1){
            full_path2 = full_path2+ "/" + args.at(0);
            out = fopen( full_path2.c_str(), "r" );
        }

        if(out != NULL && !f_option){
            printf("cp: File with name '%s' already exist\n", args.at(1).c_str());

            if(!confirm("Would you like to re-write file?")) {
                exit(EXIT_SUCCESS);
            }
        }

        out = fopen( full_path2.c_str(), "w" );

        while((c=getc(in))!=EOF)
            putc(c,out);

        fclose(in);
        fclose(out);
    }
    else{
        string full_path1;
        string full_path2;

        path = get_path();

        full_path2 = path + args.at(args.size() - 1);

        if(get_file_status(full_path2) != 1) {
            printf("cp: target '%s' is not a directory\n", args.at(args.size() - 1).c_str());
            return EXIT_FAILURE;
        }

        for(unsigned long i = 0; i <args.size() -1; i++){
            full_path1 = path + args.at(i);
            full_path2 = path + args.at(args.size() - 1) +"/"+ args.at(i);

            in = fopen( full_path1.c_str(), "r" );
            out = fopen( full_path2.c_str(), "r" );

            if( in == NULL ) {
                printf("cp: cannot stat '%s': No such file or directory\n",args.at(i).c_str());
                continue;
            } else if ( get_file_status(full_path1) == 1 ){
                printf("cp: copying directories is not supported\n");
                continue;
            }

            if(out != NULL && !f_option){
                printf("cp: File with name '%s' already exist\n", (args.at(args.size() - 1)+"/"+args.at(i)).c_str());

                if(!confirm("Would you like to re-write file?")) {
                    continue;
                }
            }

            out = fopen( full_path2.c_str(), "w" );

            while((c=getc(in))!=EOF)
                putc(c,out);

            fclose(in);
            fclose(out);
        }
    }

    return EXIT_SUCCESS;
}