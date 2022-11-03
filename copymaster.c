#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdbool.h>
#include <dirent.h>
#include <time.h>
#include <inttypes.h>
#include <pwd.h>
//#include <bsd/string.h>

#include "options.h"


void FatalError(char c, const char* msg, int exit_status);
void PrintCopymasterOptions(struct CopymasterOptions* cpm_options);

 /*void print_mods(mode_t mods)
    {
        printf( (mods & S_IRUSR) ? "r" : "-");
        printf( (mods & S_IWUSR) ? "w" : "-");
        printf( (mods & S_IXUSR) ? "x" : "-");
        printf( (mods & S_IRGRP) ? "r" : "-");
        printf( (mods & S_IWGRP) ? "w" : "-");
        printf( (mods & S_IXGRP) ? "x" : "-");
        printf( (mods & S_IROTH) ? "r" : "-");
        printf( (mods & S_IWOTH) ? "w" : "-");
        printf( (mods & S_IXOTH) ? "x" : "-");
    }*/

void
strmode(int mode, char *p)
{
	 /* print type */
	switch (mode & S_IFMT) {
	case S_IFDIR:			/* directory */
		*p++ = 'd';
		break;
	case S_IFCHR:			/* character special */
		*p++ = 'c';
		break;
	case S_IFBLK:			/* block special */
		*p++ = 'b';
		break;
	case S_IFREG:			/* regular */
		*p++ = '-';
		break;
	case S_IFLNK:			/* symbolic link */
		*p++ = 'l';
		break;
	case S_IFSOCK:			/* socket */
		*p++ = 's';
		break;
#ifdef S_IFIFO
	case S_IFIFO:			/* fifo */
		*p++ = 'p';
		break;
#endif
	default:			/* unknown */
		*p++ = '?';
		break;
	}
	/* usr */
	if (mode & S_IRUSR)
		*p++ = 'r';
	else
		*p++ = '-';
	if (mode & S_IWUSR)
		*p++ = 'w';
	else
		*p++ = '-';
	switch (mode & (S_IXUSR | S_ISUID)) {
	case 0:
		*p++ = '-';
		break;
	case S_IXUSR:
		*p++ = 'x';
		break;
	case S_ISUID:
		*p++ = 'S';
		break;
	case S_IXUSR | S_ISUID:
		*p++ = 's';
		break;
	}
	/* group */
	if (mode & S_IRGRP)
		*p++ = 'r';
	else
		*p++ = '-';
	if (mode & S_IWGRP)
		*p++ = 'w';
	else
		*p++ = '-';
	switch (mode & (S_IXGRP | S_ISGID)) {
	case 0:
		*p++ = '-';
		break;
	case S_IXGRP:
		*p++ = 'x';
		break;
	case S_ISGID:
		*p++ = 'S';
		break;
	case S_IXGRP | S_ISGID:
		*p++ = 's';
		break;
	}
	/* other */
	if (mode & S_IROTH)
		*p++ = 'r';
	else
		*p++ = '-';
	if (mode & S_IWOTH)
		*p++ = 'w';
	else
		*p++ = '-';
	switch (mode & (S_IXOTH | S_ISVTX)) {
	case 0:
		*p++ = '-';
		break;
	case S_IXOTH:
		*p++ = 'x';
		break;
	case S_ISVTX:
		*p++ = 'T';
		break;
	case S_IXOTH | S_ISVTX:
		*p++ = 't';
		break;
	}
	*p++ = ' ';		/* will be a '+' if ACL's implemented */
	*p = '\0';
}
//DEF_WEAK(strmode);

int main(int argc, char* argv[])
{
    struct CopymasterOptions cpm_options = ParseCopymasterOptions(argc, argv);

    //-------------------------------------------------------------------
    // Kontrola hodnot prepinacov
    //-------------------------------------------------------------------

    // Vypis hodnot prepinacov odstrante z finalnej verzie
    
    PrintCopymasterOptions(&cpm_options);
    
    //-------------------------------------------------------------------
    // Osetrenie prepinacov pred kopirovanim
    //-------------------------------------------------------------------
    
    if ((cpm_options.fast && cpm_options.slow) 
    || (cpm_options.create && cpm_options.overwrite)
    || (cpm_options.append && cpm_options.overwrite)
    || (cpm_options.create && cpm_options.append)
    || (cpm_options.delete_opt && cpm_options.truncate)) {
        fprintf(stderr, "CHYBA PREPINACOV\n"); 
        exit(EXIT_FAILURE);
    }
    if((cpm_options.sparse && cpm_options.fast)
    || (cpm_options.sparse && cpm_options.slow)
    || (cpm_options.sparse && cpm_options.create)
    || (cpm_options.sparse && cpm_options.overwrite)
    || (cpm_options.sparse && cpm_options.append)
    || (cpm_options.sparse && cpm_options.lseek)
    || (cpm_options.sparse && cpm_options.directory)
    || (cpm_options.sparse && cpm_options.delete_opt)
    || (cpm_options.sparse && cpm_options.chmod)
    || (cpm_options.sparse && cpm_options.inode)
    || (cpm_options.sparse && cpm_options.umask)
    || (cpm_options.sparse && cpm_options.link)
    || (cpm_options.sparse && cpm_options.truncate)){
        fprintf(stderr, "CHYBA PREPINACOV\n"); 
        exit(EXIT_FAILURE);
    }

    if((cpm_options.directory && cpm_options.fast)
    || (cpm_options.directory && cpm_options.slow)
    || (cpm_options.directory && cpm_options.create)
    || (cpm_options.directory && cpm_options.overwrite)
    || (cpm_options.directory && cpm_options.append)
    || (cpm_options.directory && cpm_options.lseek)
    || (cpm_options.directory && cpm_options.sparse)
    || (cpm_options.directory && cpm_options.delete_opt)
    || (cpm_options.directory && cpm_options.chmod)
    || (cpm_options.directory && cpm_options.inode)
    || (cpm_options.directory && cpm_options.umask)
    || (cpm_options.directory && cpm_options.link)
    || (cpm_options.directory && cpm_options.truncate)){
        fprintf(stderr, "CHYBA PREPINACOV\n"); 
        exit(EXIT_FAILURE);
    }

    // TODO Nezabudnut dalsie kontroly kombinacii prepinacov ...
    
    //-------------------------------------------------------------------
    // Kopirovanie suborov
    //-------------------------------------------------------------------
    
    // TODO Implementovat kopirovanie suborov
    
    // cpm_options.infile
    // cpm_options.outfile
    if(!cpm_options.fast && !cpm_options.slow && !cpm_options.create &&
        !cpm_options.overwrite && !cpm_options.append && !cpm_options.lseek
        && !cpm_options.directory && !cpm_options.delete_opt && !cpm_options.chmod
        && !cpm_options.inode && !cpm_options.umask && !cpm_options.link
        && !cpm_options.truncate && !cpm_options.sparse){
        int c;
        char buf[10000];
        int ifd, ofd;
        if((ifd = open(cpm_options.infile, O_RDONLY)) == (-1))
            FatalError('B', "SUBOR NEEXISTUJE", 21);
        if((ofd = open(cpm_options.outfile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == (-1))
            FatalError('B', "INA CHYBA", 21);
        while((c = read(ifd, &buf, 10000)) > 0)
            write(ofd, &buf, c);
        close(ifd);
        close(ofd);
    }
    //-------------------------------------------------------------------
    // Vypis adresara
    //-------------------------------------------------------------------
    
    if (cpm_options.directory) {
        // TODO Implementovat vypis adresara
        struct stat st;
        stat(cpm_options.infile, &st);
    
        if (!S_ISDIR(st.st_mode))
           FatalError('D', "VSTUPNY SUBOR NIE JE ADRESAR", 28);

        FILE* outfile = fopen(cpm_options.outfile, "w");

        if (outfile == NULL)
            FatalError('D', "VYSTUPNY SUBOR - CHYBA", 28);
        DIR *mydir;
        struct dirent *mydirent;
        struct stat stt;
        char outstr[100];
        struct tm *tmp;
        char buf[512];
        mydir = opendir(cpm_options.infile);
        if(mydir != NULL){
            while((mydirent = readdir(mydir)) != NULL){
                if((strcmp(mydirent->d_name, ".") != 0) 
                && strcmp(mydirent->d_name, "..") != 0){
                    sprintf(buf, "%s/%s", cpm_options.infile, mydirent->d_name);
                    stat(buf, &stt);
                    tmp = localtime(&stt.st_mtime);
                    if(tmp == NULL){
                        return -1;
                    } else {
                        char mode[100];
                        int i = 0;
                        if(S_ISDIR(stt.st_mode)){
                            mode[i] = 'd';
                            i++;
                        } else {
                            mode[i] = '-';
                            i++;
                        }
                        if(stt.st_mode & S_IRUSR){
                            mode[i] = 'r';
                            i++;
                        } else {
                            mode[i] = '-';
                            i++;
                        }
                        if(stt.st_mode & S_IWUSR){
                            mode[i] = 'w';
                            i++;
                        } else {
                            mode[i] = '-';
                            i++;
                        }
                        if(stt.st_mode & S_IXUSR){
                            mode[i] = 'x';
                            i++;
                        } else {
                            mode[i] = '-';
                            i++;
                        }
                        if(stt.st_mode & S_IRGRP){
                            mode[i] = 'r';
                            i++;
                        } else {
                            mode[i] = '-';
                            i++;
                        }
                        if(stt.st_mode & S_IWGRP){
                            mode[i] = 'w';
                            i++;
                        } else {
                            mode[i] = '-';
                            i++;
                        }
                        if(stt.st_mode & S_IXGRP){
                            mode[i] = 'x';
                            i++;
                        } else {
                            mode[i] = '-';
                            i++;
                        }
                        if(stt.st_mode & S_IROTH){
                            mode[i] = 'r';
                            i++;
                        } else {
                            mode[i] = '-';
                            i++;
                        }
                        if(stt.st_mode & S_IWOTH){
                            mode[i] = 'w';
                            i++;
                        } else {
                            mode[i] = '-';
                            i++;
                        }
                        if(stt.st_mode & S_IXOTH){
                            mode[i] = 'x';
                            i++;
                        } else {
                            mode[i] = '-';
                            i++;
                        }
                        mode[i] = '\0';
                        strftime(outstr, sizeof(outstr), "%d-%m-%Y", tmp);                        
                        fprintf(outfile ,"\n%s\t%ld\t%d\t%d\t%zu\t%s\t%s", 
                            mode, stt.st_nlink, stt.st_uid, 
                            stt.st_gid, stt.st_size, outstr, mydirent->d_name);
                    }
                }
            }
        }
            closedir(mydir);

    //-------------------------------------------------------------------
    // Osetrenie prepinacov po kopirovani
    //-------------------------------------------------------------------
    }
    // TODO Implementovat osetrenie prepinacov po kopirovani
    if(cpm_options.create){
        mode_t create = cpm_options.create_mode;
        //printf("%04o\n", create);
        if(create < 1 || create > 777)
            FatalError('c', "ZLE PRAVA", 23);
        int c;
        char buf[1000000];
        int ifd, ofd;
        struct stat st;
        stat(cpm_options.outfile, &st);
        int size = st.st_size;
        if(size > 0){
            FatalError('c', "SUBOR EXISTUJE", 23);
        }
        if((ifd = open(cpm_options.infile, O_RDONLY)) == (-1))
            FatalError('c', "INA CHYBA", 23);
        if((ofd = open(cpm_options.outfile, O_WRONLY | O_CREAT, create)) == (-1))
            FatalError('c', "INA CHYBA", 23);
        while((c = read(ifd, buf, 1000000)) > 0)
            write(ofd, buf, c);
        close(ifd);
        close(ofd);




    } else if(cpm_options.overwrite){
        int c;
        char buf[1000000];
        int ifd, ofd;
        //struct stat st;
        //stat(cpm_options.outfile, &st);
        //int size = st.st_size;
        if((ifd = open(cpm_options.infile, O_RDONLY)) == (-1))
            FatalError('o', "INA CHYBA", 24);
        if((ofd = open(cpm_options.outfile, O_TRUNC | O_WRONLY)) == (-1)){
            if(errno == 2){
                FatalError('o', "SUBOR NEEXISTUJE", 24);
            }
            FatalError('o', "INA CHYBA", 24);
        }
        while((c = read(ifd, buf, 1000000)) > 0)
            write(ofd, buf, c);
        close(ifd);
        close(ofd);



    }
    if(cpm_options.umask){
        //char u_r[] = "u-r";
        //umask(072);
        
        //mode_t n;
        //umask(n);
        int ifd, ofd, c;
        char buf[1000000];
        //char mode[4];
        //mode[0] = '0';
        //printf("OR: %3o\nNEW: %3o\n", nul, n);
        struct stat st;
        stat(cpm_options.outfile, &st);
        mode_t im = st.st_mode;
        char mod[10];
        strmode(im, mod);
        //print_mods(im);
        //printf("\ndsfdsds %s\n", mod);

        
        for(int i = 0; i < 10; i++){
            if(strcmp(cpm_options.umask_options[i], "u-r") == 0){
                mod[1] = '-';
                //printf("\n%s\n", mod);
            }
            if(strcmp(cpm_options.umask_options[i], "u-w") == 0){
                mod[2] = '-';
                //printf("\na\n");
            }
            //printf("\na\n");
            if(strcmp(cpm_options.umask_options[i], "u-x") == 0){
                mod[3] = '-';
            }
            if(strcmp(cpm_options.umask_options[i], "g-r") == 0){
                mod[4] = '-';
            }
            if(strcmp(cpm_options.umask_options[i], "g-w") == 0){
                mod[5] = '-';
            }
            if(strcmp(cpm_options.umask_options[i], "g-x") == 0){
                mod[6] = '-';
            }
            if(strcmp(cpm_options.umask_options[i], "o-r") == 0){
                mod[7] = '-';
            }
            if(strcmp(cpm_options.umask_options[i], "o-w") == 0){
                mod[8] = '-';
            }
            if(strcmp(cpm_options.umask_options[i], "o-x") == 0){
                mod[9] = '-';
                //printf("\na\n");
            }
            if(strcmp(cpm_options.umask_options[i], "u+r") == 0){
                mod[1] = 'r';
                //printf("+++\n%s\n", mod);
            }
            if(strcmp(cpm_options.umask_options[i], "u+w") == 0){
                mod[2] = 'w';
            }
            if(strcmp(cpm_options.umask_options[i], "u+x") == 0){
                mod[3] = 'x';
            }
            if(strcmp(cpm_options.umask_options[i], "g+r") == 0){
                mod[4] = 'r';
            }
            if(strcmp(cpm_options.umask_options[i], "g+w") == 0){
                mod[5] = 'w';
            }
            if(strcmp(cpm_options.umask_options[i], "g+x") == 0){
                mod[6] = 'x';
            }
            if(strcmp(cpm_options.umask_options[i], "o+r") == 0){
                mod[7] = 'r';
            }
            if(strcmp(cpm_options.umask_options[i], "o+w") == 0){
                mod[8] = 'w';
            }
            if(strcmp(cpm_options.umask_options[i], "o+x") == 0){
                mod[9] = 'x';
                //printf("\na\n");
            }
            
        }
        //printf("\n%s\n", mod);
        mode_t mode = 0;

if (mod[1] == 'r')
  mode |= 0400;
if (mod[2] == 'w')
  mode |= 0200;
if (mod[3] == 'x')
  mode |= 0100;
if (mod[4] == 'r')
  mode |= 0040;
if (mod[5] == 'w')
  mode |= 0020;
if (mod[6] == 'x')
  mode |= 0010;
if (mod[7] == 'r')
  mode |= 0004;
if (mod[8] == 'w')
  mode |= 0002;
if (mod[9] == 'x')
  mode |= 0001;
        //mode_t cur = umask(0);
        //printf("SDFDSFJFDKS%4o\n", mode);
        //mode_t n = 
        umask(0);
        umask(mode);
        ifd = open(cpm_options.infile, O_RDONLY);
            //FatalError('u', "INA CHYBA1", 31);
        if((ofd = open(cpm_options.outfile, O_WRONLY | O_CREAT, mode)) == (-1))
            FatalError('u', "INA CHYBA", 32);
        while((c = read(ifd, buf, 1000000)) > 0)
            write(ofd, buf, c);
        chmod(cpm_options.outfile, mode);
        /*for(int i = 0; i < 3; i++){
            printf("%c\n", cpm_options.umask_options[0][i]);
        }*/
        close(ifd);
        close(ofd);
        return 0;



    } else if(cpm_options.link){
        int ifd;
        if((ifd = open(cpm_options.infile, O_RDONLY)) == (-1)){
            if(errno == 2){
                FatalError('a', "SUBOR NEEXISTUJE", 30);
            }
            FatalError('a', "INA CHYBA", 30);
            close(ifd);
        }
        if(link(cpm_options.infile, cpm_options.outfile) != 0){
            FatalError('K', "VYSTUPNY SUBOR NEVITVORENY", 30);
        }



    } else if(cpm_options.truncate){
        int ifd, ofd, c;
        struct stat st;
        char buf[1000000];
        stat(cpm_options.infile, &st);
        if(cpm_options.truncate_size < 0)
            FatalError('t', "ZAPORNA VELKOST", 31);
        if(!S_ISREG(st.st_mode))
            FatalError('t', "VSTUPNY SUBOR NEZMENENY", 31);
        if((ifd = open(cpm_options.infile, O_RDONLY)) == (-1))
            FatalError('l', "INA CHYBA", 31);
        if((ofd = open(cpm_options.outfile, O_WRONLY)) == (-1))
            FatalError('l', "INA CHYBA", 31);
        while((c = read(ifd, buf, 1000000)) > 0)
            write(ofd, buf, c);
        truncate(cpm_options.infile, cpm_options.truncate_size);
        close(ifd);
        close(ofd);



    } else if(cpm_options.sparse){
        int ifd, ofd, c;
        //int i;
        //int h = 0;
        char buf[1];
        int byte = 0;
        //int check = 0;
        //lseek(ifd, 0L, SEEK_SET);
        ifd = open(cpm_options.infile, O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH);
        if((ofd = open(cpm_options.outfile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH)) == (-1))
            FatalError('S', "RIEDKY SUBOR NEVYTVORENY", 41);
        while((c = read(ifd, &buf, 1)) > 0){
            if(buf[0] == '\0'){
                lseek(ofd, 1, SEEK_CUR);
            } else {
                write(ofd, &buf, 1);
            }
        }
        //struct stat st_o;
        
        struct stat st;
        stat(cpm_options.infile, &st);
        int byte1 = st.st_size;
        int res = byte1-byte;
        ftruncate(ofd, res);
        //int inodee = st.st_ino - byte;
        
        //truncate(cpm_options.infile, cpm_options.outfile);
        close(ifd);
        close(ofd);
        return 0;



    } else if(cpm_options.lseek){
        int ifd, ofd, c;
        int check = 0;
        char buf[1000000];
        if((ifd = open(cpm_options.infile, O_RDONLY)) == (-1))
            FatalError('l', "INA CHYBA", 33);
        if((ofd = open(cpm_options.outfile, O_WRONLY)) == (-1))
            FatalError('l', "INA CHYBA", 33);
        lseek(ifd, cpm_options.lseek_options.pos1, SEEK_SET);
        if(cpm_options.lseek_options.x == 1)
            lseek(ofd, cpm_options.lseek_options.pos2, SEEK_CUR);
        if(cpm_options.lseek_options.x == 0)
            lseek(ofd, cpm_options.lseek_options.pos2, SEEK_SET);
        if(cpm_options.lseek_options.x == 2)
            lseek(ofd, cpm_options.lseek_options.pos2, SEEK_END);
        while((c = read(ifd, &buf, cpm_options.lseek_options.num)) > 0){
            check = check + c;
            if(check <= (int)cpm_options.lseek_options.num)
                write(ofd, &buf, c);
        }
        close(ifd);
        close(ofd);




    } else if(cpm_options.append){
        int c;
        char buf[1000000];
        int ifd, ofd;
        //struct stat st;
        //stat(cpm_options.outfile, &st);
        //int size = st.st_size;
        if((ifd = open(cpm_options.infile, O_RDONLY)) == (-1))
            FatalError('o', "INA CHYBA", 24);
        if((ofd = open(cpm_options.outfile, O_WRONLY | O_APPEND, 0644)) == (-1)){
            if(errno == 2){
                FatalError('a', "SUBOR NEEXISTUJE", 22);
            }
            FatalError('a', "INA CHYBA", 22);
        }
        while((c = read(ifd, buf, 1000000)) > 0)
            write(ofd, buf, c);
        close(ifd);
        close(ofd);



    }
    if(cpm_options.chmod){
        int ifd, ofd, c;
        char buf[1000000];
        int d = atoi(argv[2]);
        mode_t chmod_mode = cpm_options.chmod_mode;
        if(d < 1 || d > 777){
            FatalError('m', "ZLE PRAVA", 34);
        }
        if((ifd = open(cpm_options.infile, O_RDONLY)) == (-1))
            FatalError('f', "INA CHYBA", -1);
        if((ofd = open(cpm_options.outfile, O_WRONLY | O_CREAT)) == (-1))
            FatalError('f', "INA CHYBA", -1);
        while((c = read(ifd, buf, 1000000)) > 0)
            write(ofd, buf, c);
        close(ofd);
        close(ifd);
        chmod(cpm_options.outfile, chmod_mode);




    } else if(cpm_options.inode){
        struct stat in;
        stat(cpm_options.infile, &in);
        if(cpm_options.inode_number != in.st_ino)
            FatalError('i', "ZLY INODE", 27);
        if(!S_ISREG(in.st_mode))
            FatalError('i', "ZLY TYP VSTUPNOHO SUBORU", 27);
        int ifd, ofd, c;
        char buf[1000000];
        if((ifd = open(cpm_options.infile, O_RDONLY)) == (-1))
            FatalError('i', "INA CHYBA", 27);
        if((ofd = open(cpm_options.outfile, O_WRONLY | O_CREAT)) == (-1))
            FatalError('i', "INA CHYBA", 27);
        while((c = read(ifd, buf, 1000000)) > 0)
            write(ofd, buf, c);
        close(ifd);
        close(ofd);



    }
    if(cpm_options.delete_opt){
        int ifd, ofd, c;
        char buf[1000000];
        struct stat inf;
        if((ifd = open(cpm_options.infile, O_RDONLY)) == (-1))
            FatalError('f', "INA CHYBA", 26);
        if((ofd = open(cpm_options.outfile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == (-1))
            FatalError('f', "INA CHYBA", 26);
        while((c = read(ifd, &buf, 1000000)) > 0)
            write(ofd, &buf, c);
        stat(cpm_options.infile, &inf);
        close(ifd);
        close(ofd);
        if(S_ISREG(inf.st_mode)){
            unlink(cpm_options.infile);
        } else {
            FatalError('d', "SUBOR NEBOL ZMAZANY", 26);
        }




    } else if(cpm_options.fast){
        int c;
        char buf[1000000];
        int ifd, ofd;
        if((ifd = open(cpm_options.infile, O_RDONLY)) == (-1))
            FatalError('f', "INA CHYBA", -1);
        if((ofd = open(cpm_options.outfile, O_WRONLY)) == (-1))
            FatalError('f', "INA CHYBA", -1);
        while((c = read(ifd, buf, 1000000)) > 0)
            write(ofd, buf, c);
        close(ifd);
        close(ofd);




    } else if(cpm_options.slow){
        //int c;
        char buf;
        int ifd, ofd;
        if((ifd = open(cpm_options.infile, O_RDONLY)) == (-1))
            FatalError('s', "INA CHYBA", -1);
        if((ofd = open(cpm_options.outfile, O_WRONLY | O_TRUNC)) == (-1))
            FatalError('s', "INA CHYBA", -1);
        while(read(ifd, &buf, 1) > 0)
            write(ofd, &buf, 1);
        close(ifd);
        close(ofd);
    }
    return 0;
}


void FatalError(char c, const char* msg, int exit_status)
{
    fprintf(stderr, "%c:%d\n", c, errno); 
    fprintf(stderr, "%c:%s\n", c, strerror(errno));
    fprintf(stderr, "%c:%s\n", c, msg);
    exit(exit_status);
}

void PrintCopymasterOptions(struct CopymasterOptions* cpm_options)
{
    if (cpm_options == 0)
        return;
    
    printf("infile:        %s\n", cpm_options->infile);
    printf("outfile:       %s\n", cpm_options->outfile);
    
    printf("fast:          %d\n", cpm_options->fast);
    printf("slow:          %d\n", cpm_options->slow);
    printf("create:        %d\n", cpm_options->create);
    printf("create_mode:   %o\n", (unsigned int)cpm_options->create_mode);
    printf("overwrite:     %d\n", cpm_options->overwrite);
    printf("append:        %d\n", cpm_options->append);
    printf("lseek:         %d\n", cpm_options->lseek);
    
    printf("lseek_options.x:    %d\n", cpm_options->lseek_options.x);
    printf("lseek_options.pos1: %ld\n", cpm_options->lseek_options.pos1);
    printf("lseek_options.pos2: %ld\n", cpm_options->lseek_options.pos2);
    printf("lseek_options.num:  %lu\n", cpm_options->lseek_options.num);
    
    printf("directory:     %d\n", cpm_options->directory);
    printf("delete_opt:    %d\n", cpm_options->delete_opt);
    printf("chmod:         %d\n", cpm_options->chmod);
    printf("chmod_mode:    %o\n", (unsigned int)cpm_options->chmod_mode);
    printf("inode:         %d\n", cpm_options->inode);
    printf("inode_number:  %lu\n", cpm_options->inode_number);
    
    printf("umask:\t%d\n", cpm_options->umask);
    for(unsigned int i=0; i<kUMASK_OPTIONS_MAX_SZ; ++i) {
        if (cpm_options->umask_options[i][0] == 0) {
            // dosli sme na koniec zoznamu nastaveni umask
            break;
        }
        printf("umask_options[%u]: %s\n", i, cpm_options->umask_options[i]);
    }
    
    printf("link:          %d\n", cpm_options->link);
    printf("truncate:      %d\n", cpm_options->truncate);
    printf("truncate_size: %ld\n", cpm_options->truncate_size);
    printf("sparse:        %d\n", cpm_options->sparse);
}

