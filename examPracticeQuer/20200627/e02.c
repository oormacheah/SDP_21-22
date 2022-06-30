struct dirent {
inot_t d_no;
char d_name[NAM_MAX+1];
. . .
};

DIR *opendir (const char *filename);
struct dirent *readdir (DIR *dp);
int closedir (DIR *dp);

void visit(char *pathname, char *prefix, int level) {
    visitR(pathname, prefix, 0);
}

void visitR(char *pathname, char *prefix, int level) {
    struct stat statbuf;
    DIR *dp;
    struct dirent *dir_Sp; 
    char nameR[N];

    if (lstat(pathname, &statbuf) < 0) {
        fprintf(stderr, "error on lstat\n");
        exit(1);
    }

    if (S_ISDIR(statbuf.st_mode) == 0) {
        return(1);
    }
    // if it's a directory
    if ((dp = opendir(pathname)) == NULL) {
        fprintf(stderr, "Error reading directory\n");
        exit(1);
    }
    while ((dir_Sp = readdir(dp) != NULL) {
        // concatenate path name
        sprintf(nameR, "%s/%s", pathname, dir_Sp->d_name);

        if (lstat(nameR, &statbuf < 0) { // read
            fprintf(stderr, "error on lstat\n");
            exit(1);
        }
        if (S_ISDIR(statbuf->st_mode) == 0) { // files only will be displayed
            if (level == 2 && strstr(dir_Sp->d_name, prefix)) {
                fprintf(stdout, "At level = %d, found file: %s", level+1, dir_Sp->d_name);
            }
        }
        else {
            // if it's a directory, it won't be considered as a "content"
            if (strcmp(dir_Sp->d_name, "..") || strcmp(dir_Sp->d_name, ".")) {
                continue;
            }
            visitR(nameR, prefix, level + 1);
        }
    }
}
