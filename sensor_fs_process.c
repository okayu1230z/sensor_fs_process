#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>

typedef struct Information
{
    int x;
    int y;
    int z;
    int b;
    int cmd_num;
    int arg1;
    int arg2;
    char c_dir[256];
    int count;
} info_struct;
info_struct info = {0, 0, 0, 0, -1, -1, -1, "./", 0};

time_t time(time_t *t);
int process_fs();
long hexToDec(char *source);
int getIndexOfSigns(char ch);
void print_msg(char cmd[]);
void my_ls(char all_dir[8][256]);
int my_mkdir();
void my_touch();
int my_rmdir(char all_dir[8][256]);

/* char -> value */
int getIndexOfSigns(char ch)
{
    if (ch >= '0' && ch <= '9')
    {
        return ch - '0';
    }
    if (ch >= 'A' && ch <= 'F')
    {
        return ch - 'A' + 10;
    }
    if (ch >= 'a' && ch <= 'f')
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
        for (i = len - 1; i >= 0; i--)
        {
            sum += t * getIndexOfSigns(*(source + i));
            t *= 16;
        }

        return sum;
    }

    int main(int argc, const char *argv[])
    {
        FILE *fp;
        //info_struct info = {0, 0, 0, 0, -1, -1, -1, "./", 0};

        char fname[] = "teraterm.log";
        int flag = 1;
        double interval = 5.0;
        char a[20];

        while (flag)
        {
            //read file
            fp = fopen("teraterm.log", "r");
            if (fp == NULL)
            {
                printf("%s file not open!\n", fname);
                return -1;
            }
            else
            {
                info.count++;
            }

            //change position of file pointer
            fseek(fp, -100L, SEEK_END);

            char t[] = " ";
            char *result = NULL;
            int i; // count line
            int xyz_count = 0;
            int temp[4];

            while (fgets(a, 60, fp) != NULL)
            {
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
                            //temp[4] = result;
                            temp[4] = (int)hexToDec(result + 11);
                        }
                        i++;
                        if (i == 4 && xyz_count < 3)
                        {
                            //the fourth word is data
                            xyz_count++;
                            //printf("%d \n", temp[xyz_count]);
                            temp[xyz_count] = (int)hexToDec(result + 2);
                        }
                        result = strtok(NULL, t);
                    }
                }
            }

            info.x = temp[1];
            info.y = temp[2];
            info.z = temp[3];
            info.b = temp[4];

            process_fs(info);

            fclose(fp);

            sleep(interval);
        }
        return 0;
    }

    void print_msg(char cmd[])
    {
        printf("COUNT : %d \n", info.count);
        printf("X : %d \n", info.x);
        printf("Y : %d \n", info.y);
        printf("Z : %d \n", info.z);
        printf("B : %d \n", info.b);
        //printf("C : %s \n", cmd);
        printf("curent_dir : %s \n", info.c_dir);
        printf("\n");
    }

    void print_debug()
    {
        printf("\n------------------------------------------------ \n");
        printf(" info.cmd_num : %d \n", info.cmd_num);
        printf(" info.arg1 : %d \n", info.arg1);
        printf(" info.arg2 : %d \n", info.arg2);
    }

    int process_fs()
    {
        print_debug();

        char *path = info.c_dir;
        DIR *dir;
        struct dirent *dent;
        char all_dir[8][256];
        dir = opendir(path);

        int i = 0;
        while ((dent = readdir(dir)) != NULL)
        {
            if (strcmp(dent->d_name, "."))
            {
                if (i < 8)
                    strcpy(all_dir[i], dent->d_name);
                i++;
            }
        }

        closedir(dir);

        my_ls(all_dir);

        char cmd[256] = "";
        int thr = 30000; // 閾値

        int x_c = info.x;
        int y_c = info.y;
        int z_c = info.z;
        int b_c = info.b;

        if (b_c == 8)
        {
            info.cmd_num = -1;
            info.arg1 = -1;
            info.arg2 = -1;
            return 0;
        }

        if (info.cmd_num == 2)
        {
            if (info.arg1 == -1)
            {
                return 0;
            }
            else
            {
                if (strcmp(info.c_dir, ".") != 0)
                {
                    strcpy(info.c_dir, all_dir[info.arg1]);
                    info.cmd_num = -1;
                    return 0;
                }
                else
                {
                    strcpy(info.c_dir, "..");
                    info.cmd_num = -1;
                    return 0;
                }
            }
        }
        else if (info.cmd_num == 3)
        {
            if (info.arg1 == -1)
            {
                return 0;
            }
            else
            {
                my_rmdir(all_dir);
            }
        }

        if (info.cmd_num == -1)
        {
            if (x_c > thr && y_c < thr && z_c < thr)
            {
                // 1 - 0 - 0
                info.cmd_num = 4;
            }
            else if (x_c < thr && y_c > thr && z_c < thr)
            {
                // 0 - 1 - 0
                info.cmd_num = 2;
            }
            else if (x_c < thr && y_c < thr && z_c > thr)
            {
                // 0 - 0 - 1
                /*
            info.cmd_num = 1;
            my_mkdir(info);
            info.cmd_num = -1;
            */
                info.cmd_num = 3;
            }
            else if (x_c > thr && y_c > thr && z_c < thr)
            {
                // 1 - 1 - 0
                info.cmd_num = 6;
            }
            else if (x_c < thr && y_c > thr && z_c > thr)
            {
                // 0 - 1 - 1
                info.cmd_num = 3;
            }
            else if (x_c > thr && y_c < thr && z_c > thr)
            {
                // 1 - 0 - 1
                info.cmd_num = 5;
            }
            else if (x_c > thr && y_c > thr && z_c > thr)
            {
                // 1 - 1 - 1
                info.cmd_num = 7;
            }
            else
            {
                // 0 - 0 - 0
                info.cmd_num = 0;
            }
        }

        //print_msg(cmd);
        //my_ls(all_dir);

        return 0;
    }

    void my_ls(char all_dir[8][256])
    {
        for (int i = 0; i < 8; i++)
        {
            printf("\x1b[41m%d\x1b[m %s    ", (i + 1), all_dir[i]);
            if (i == 3 || i == 7)
                printf("\n");
        }
        return;
    }

    void my_touch()
    {
        char date[64];
        time_t t = time(NULL);
        strftime(date, sizeof(date), "%Y%m%d%H%M%S", localtime(&t));
        //printf("%s\n", date);
        FILE *fp_tmp;
        fp_tmp = fopen(date, "w");
        fclose(fp_tmp);
        return;
    }

    int my_mkdir()
    {
        char date[64];
        time_t t = time(NULL);
        strftime(date, sizeof(date), "%Y%m%d%H%M%S", localtime(&t));
        if (strcmp(info.c_dir, "./") == 0)
        {
            mkdir(date);
        }
        else
        {
            //char b[256] = strcat(info.c_dir, "/");
            char a[256];
            char b[2] = "/";
            sprintf(a, "%s%s%s", info.c_dir, b, date);
            mkdir(a); //strcat(info.c_dir, strcat("/", date));
        }
        //mkdir(date);
        return 0;
    }

    int my_rmdir(char all_dir[8][256])
    {
        if (strcmp(info.c_dir, "./") == 0)
        {
            rmdir(all_dir[info.arg1]);
        }
        else
        {
            char a[256];
            char b[2] = "/";
            sprintf(a, "%s%s%s", info.c_dir, b, all_dir[info.arg1]);
            rmdir(a);
        }
        return 0;
    }