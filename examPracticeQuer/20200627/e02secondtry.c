struct dirent {
inot_t d_no;
char d_name[NAM_MAX+1];
. . .
};

DIR *opendir (const char *filename);
struct dirent *readdir (DIR *dp);
int closedir (DIR *dp);

int main() {
    visit("mydir", "foo", 0);
}

void visit(char *pathname, char *prefix, int level) {
    struct stat stat_buf;
    DIR *dp;
    struct dirent *dirent_buf;
    char new_pathname[N];

    if ((lstat(pathname, &stat_buf) < 0) {
        fprintf(stderr, "error reading pathname");
        exit(1);
    }
    if (!I_ISDIR(stat_buf.mode)) {
        fprintf(stderr, "given pathname is not a directory");
        exit(1);
    }
    if ((dp = opendir(pathname)) == NULL) {
        fprintf(stderr, "pathname couldn't be opened");
        exit(1);
    }
    while ((dirent_buf = readdir(dp)) != NULL) {
        if (strcmp(dirent_buf->d_name, ".") || strcmp(dirent_buf->d_name, "..")) {
            continue;
        }
        
        sprintf(new_pathname, "%s/%s", pathname, dirent_buf->d_name);

        if (lstat(new_pathname, &stat_buf) < 0) {
            fprintf(stderr, "error reading link of directory");
            exit(1);
        }
        
        if (!I_ISDIR(stat_buf.mode)) {
            if (level == 2 && strstr(dirent_buf->d_name, prefix)) {
                printf("at level %d there is file: %s", level+1, dirent_buf->d_name);
            }
        }
        else {
            // it is a directory
            visit(new_pathname, prefix, level + 1);
        }   
    }
}