#include "map_reduce.h"

//Space to store the results for analysis map
struct Analysis analysis_space[NFILES];
//Space to store the results for stats map
Stats stats_space[NFILES];

//Sample Map function action: Print file contents to stdout and returns the number bytes in the file.
int cat(FILE* f, void* res, char* filename) {
    char c;
    int n = 0;
    printf("%s\n", filename);
    while((c = fgetc(f)) != EOF) {
        printf("%c", c);
        n++;
    }
    printf("\n");
    return n;
}

int main(int argc, char** argv) {
    printf("Welcome to CSE 320!\n");

    //validateargs(argc, argv);  //seg fault

    //map("./rsrc/stats_light", analysis_space, sizeof(analysis_space), cat);//segmentation fault core dumped error

	int nInt = nfiles("./rsrc/ana_heavy");
    printf("%d\n", nInt);s
    return EXIT_SUCCESS;
}

