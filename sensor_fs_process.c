#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

typedef struct Change {
  int x_change;
  int y_change;
  int z_change;
  int s_change;
  int count;
} type_change;

int process_fs(type_change change);
long hexToDec(char *source);
int getIndexOfSigns(char ch);
void print_msg(type_change change, char cmd[]);
// void my_ls(char *all_dir[]);

/* char -> value */
int getIndexOfSigns(char ch) {
    if(ch >= '0' && ch <= '9') {
        return ch - '0';
    }
    if(ch >= 'A' && ch <='F') {
        return ch - 'A' + 10;
    }
    if(ch >= 'a' && ch <= 'f') {
        return ch - 'a' + 10;
    }
    return -1;
}

/* 16->10 */
long hexToDec(char *source) {
    long sum = 0;
    long t = 1;
    int i, len;

    len = strlen(source);
    for(i=len-1; i>=0; i--)
    {
        sum += t * getIndexOfSigns(*(source + i));
        t *= 16;
    }

    return sum;
}


int main(int argc, const char *argv[]) {
    FILE *fp;
    type_change change = {0, 0, 0, 0};

    char fname[] = "teraterm_cordinator.log";
    int flag = 1;
    double interval = 1.0;
    char a[20];

    int x_old = 0;
    int y_old = 0;
    int z_old = 0;
    int s_old = 0;
    while(flag) {
        //read file
        fp = fopen(fname, "r");
        if(fp == NULL) {
            printf("%s file not open!\n", fname);
            return -1;
          } else {
            change.count++;
          }

          //change position of file pointer
          fseek(fp, -100L, SEEK_END);

          char t []=" ";
          char *result = NULL;
          int i;// count line
          int xyz_count = 0;
          int temp[4];

          while (fgets(a, 60, fp) != NULL)
          {
              //read each line
              //puts(a);
              //split each word
              result = NULL;
              if (strchr(a, '0') != NULL)
              {
                  result = strtok(a, t);
                  i = 0;
                  while (result != NULL)
                  {
                      if (strchr(a, 's') != NULL)
                      {
                          //printf("", );
                          temp[4] = (int)hexToDec(result + 11);
                      }
                      i++;
                      if (i == 4 && xyz_count < 3)
                      {
                          //the fourth word is data
                          //printf("%s\n", result); printf("!!!!!%ld\n",hexToDec(result+2));
                          xyz_count++;
                          //printf("%d \n", temp[xyz_count]);
                          temp[xyz_count] = (int)hexToDec(result + 2);
                          printf("%d \n", temp[xyz_count]);
                      }
                      result = strtok(NULL, t);
                      //printf("%d",i);
                  }
              }
      }
      change.x_change = temp[1] - x_old; x_old = temp[1];
      change.y_change = temp[2] - y_old; y_old = temp[2];
      change.z_change = temp[3] - z_old; z_old = temp[3];
      change.s_change = temp[4] - s_old; s_old = temp[4];
      process_fs(change);

      fclose(fp);
      sleep(interval);
   }

   return 0;
}

void print_msg (type_change change, char cmd[]) {
    printf("COUNT : %d \n", change.count);
    printf("X_CHANGE : %d \n", change.x_change);
    printf("Y_CHANGE : %d \n", change.y_change);
    printf("Z_CHANGE : %d \n", change.z_change);
    printf("COMMAND : %s \n", cmd);
    printf("\n");
}

int process_fs(type_change change) {
    /* ここから、current directry の情報を拾ってくるのに必要 */
    char *path = "./";
    DIR *dir;
    struct dirent *dent;
    char all_dir[8][256];
    dir = opendir(path);

    int i = 0;
    while ((dent = readdir(dir)) != NULL) {
        if(i < 8) strcpy(all_dir[i], dent->d_name);
        i++;
    }

    closedir(dir);
    /* ここまで */

    char cmd[256];
    int thr = 30000; // 閾値

    int x_c = change.x_change;
    int y_c = change.y_change;
    int z_c = change.z_change;

    if(x_c > thr && y_c < thr && z_c < thr) {
        // xだけが閾値を超えている
        strcpy(cmd, "x");
    } else if(x_c < thr && y_c > thr && z_c < thr) {
        // yだけ
        strcpy(cmd, "y");
    } else if(x_c < thr && y_c < thr && z_c > thr) {
        // zだけ
        strcpy(cmd, "z");
    } else if(x_c > thr && y_c > thr && z_c < thr) {
        // xとyが閾値を超えている
        strcpy(cmd, "x, y");
    } else if(x_c < thr && y_c > thr && z_c > thr) {
        // yとz
        strcpy(cmd, "y, z");
    } else if(x_c > thr && y_c < thr && z_c > thr) {
        // xとz
        strcpy(cmd, "x, z");
    } else if(x_c > thr && y_c > thr && z_c > thr) {
        // x,y,zが閾値を超えている
        strcpy(cmd, "x, y, z");
    } else {
        // system("ls");
        // my_ls(&all_dir);
        for(int j = 0; j < 8 ; j++) {
            printf("\x1b[41m%d\x1b[m %s    ", j, all_dir[j]);
            if(j == 3 || j == 7) printf("\n");
        }
        strcpy(cmd, "なんも変化がない");
    }

    print_msg(change, cmd);

    return 0;
}

/* lsの関数化はまだしていない
void my_ls(char *all_dir[]) {
  for(int i = 0; i < 8 ; i++) {
      printf("\x1b[41m%d\x1b[m %s    ", i, all_dir[i]);
      if(i == 3 || i == 7) printf("\n");
  }
  return;
}
*/