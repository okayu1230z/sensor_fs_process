#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct Change {
  int x_change;
  int y_change;
  int z_change;
  int count;
} type_change;

int process_fs(type_change change);
long hexToDec(char *source);
int getIndexOfSigns(char ch);
void print_msg(type_change change, char cmd[]);

/* char -> value */
int getIndexOfSigns(char ch)
{
    if(ch >= '0' && ch <= '9')
    {
        return ch - '0';
    }
    if(ch >= 'A' && ch <='F')
    {
        return ch - 'A' + 10;
    }
    if(ch >= 'a' && ch <= 'f')
    {
        return ch - 'a' + 10;
    }
return -1;
}

/* 16->10 */
long hexToDec(char *source)
{
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


int main(void) {
    FILE *fp;
    type_change change = {0, 0, 0, 0};

    char fname[] = "teraterm_cordinator.log";
    int flag = 1;
    double interval = 1.0;
    char a[20];

    int x_old = 0;
    int y_old = 0;
    int z_old = 0;

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
          fseek(fp, -84L, SEEK_END);

          char t []=" ";
          char *result = NULL;
          int i;// count line
          int xyz_count = 0;
          int temp[3];

          while (fscanf(fp, "%[^\n]%*c", a) != EOF) {
            //read each line
            //puts(a);
            //split each word
            result=NULL;
            if(strchr(a,'0')!=NULL) {
              result = strtok(a, t);
              i=0;
              while(result!=NULL){
                  i++;
                  if (i==4){
                      //the fourth word is data
                      //printf("%s\n", result);
                      //printf("!!!!!%ld\n",hexToDec(result+2));
                      xyz_count ++;
                      temp[xyz_count] = (int)hexToDec(result+2);
                  }
                  result = strtok(NULL,t);
                  //printf("%d",i);
              }
          }
      }
      change.x_change = temp[1] - x_old; x_old = temp[1];
      change.y_change = temp[2] - y_old; y_old = temp[2];
      change.z_change = temp[3] - z_old; z_old = temp[3];

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
    char cmd[256];
    int thr = 30000;// 閾値

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
        strcpy(cmd, "なんも変化がない");
    }

    print_msg(change, cmd);

    return 0;
}
