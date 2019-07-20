
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
int my_rm(char all_dir[8][256]);
int my_open(char all_dir[8][256]);
int fcopy(const char *fnamer, const char *fnamew);
int my_cp(char all_dir[8][256]);

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
	  //printf("\n %s file not open!\n", fname);
	  return -1;
	}
      else
	{
	  //printf("\n %s file opened!\n", fname);
	  info.count++;
	}

      //change position of file pointer
      fseek(fp, -100L, SEEK_END);

      char t[] = " ";
      char *result = NULL;
      int i; // count line
      int xyz_count = 0;
      int temp[4];
      int button_flag = 1;

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
		      if(button_flag == 1) {
			temp[4] = atoi(result + 11);//(int)hexToDec(result + 11);
			button_flag = 0;
		      }
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

      printf("temp[1] %d\n", temp[1]);
      printf("temp[2] %d\n", temp[2]);
      printf("temp[3] %d\n", temp[3]);
      printf("temp[4] %d\n", temp[4]);

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

  int i;
  for(i = 0; i < 8; i++) {
    strcpy(all_dir[i],"");
  }

  i = 0;
  while ((dent = readdir(dir)) != NULL)
    {
      if (strcmp(dent->d_name, "."))
	{
	  if (i < 8) {
	    strcpy(all_dir[i], dent->d_name);
	    i++;
	  }
	}
    }

  closedir(dir);

  my_ls(all_dir);

  char cmd[256] = "";
  int thr = 30000; // 閾値

  int x_c = info.x;
  int y_c = info.y;
  int z_c = info.z;

  if (info.cmd_num == -1 && info.arg1 != -1) {
    info.arg1 = -1;
    info.arg2 = -1;
    printf(" [debug reset] status reset!");
    return 0;
  }

  if(info.cmd_num != -1 && info.b == 8){
    printf(" [debug waitting button] you should push button for arg1");
    return 0;
  } else if(info.cmd_num != -1 && info.b != 8) {
    info.arg1 = info.b;
    switch(info.cmd_num) {
    case 0:
      printf(" [debug do] info.cmd = 0 : cp \n");
      my_cp(all_dir);
      break;
    case 1:
      printf(" [debug do] info.cmd = 1 : nyancat \n");
      system("nyancat");
      break;
    case 2:
      printf(" [debug do] info.cmd = 2 : nyancar \n");
      system("nyancat");
      break;
    case 3:
      printf(" [debug do] info.cmd = 3 : open \n");
      my_open(all_dir);
      break;
    case 4:
      printf(" [debug do] info.cmd = 4 : rmdir \n");
      my_rmdir(all_dir);
      break;
    case 5:
      printf(" [debug do] info.cmd = 5 : cd \n");
      if (strcmp(info.c_dir, ".") != 0)
	{
	  strcpy(info.c_dir, all_dir[info.arg1]);
	}
      else
	{
	  strcpy(info.c_dir, "..");
	}
      break;
    case 6:
      printf(" [debug do] info.cmd = 6 : mkdir [date] \n");
      break;
    case 7:
      printf(" [debug do] info.cmd = 7 : rm \n");
      my_rm(all_dir);
      break;
    default:
      printf(" [debug do] !? 何が起きた \n");
    }
    info.cmd_num = -1;
    return 0;
  } else if(info.cmd_num == -1 && info.b == 8) {
    if (info.x > thr && info.y < thr && info.z < thr)
      {
	// 1 - 0 - 0
	info.cmd_num = 4;
	printf(" [debug dicision] info.cmd = 4 : rmdir \n");
      }
    else if (info.x < thr && info.y > thr && info.z < thr)
      {
	// 0 - 1 - 0
	info.cmd_num = 2;
	printf(" [command dicision] info.cmd = 2 : nyancat \n");
      }
    else if (info.x < thr && info.y < thr && info.z > thr)
      {
	// 0 - 0 - 1
	info.cmd_num = 1;
	printf(" [command dicision] info.cmd = 1 : nyancat \n");
      }
    else if (info.x > thr && info.y > thr && info.z < thr)
      {
	// 1 - 1 - 0
	info.cmd_num = 6;
	printf(" [command dicision] info.cmd = 6 : mkdir \n");
	my_mkdir(info);
	printf(" [command execution] info.cmd = 6 : mkdir [date] \n");
	info.cmd_num = -1;
      }
    else if (info.x < thr && info.y > thr && info.z > thr)
      {
	// 0 - 1 - 1
	info.cmd_num = 3;
	printf(" [command dicision] info.cmd = 3 : open \n");
      }
    else if (info.x > thr && info.y < thr && info.z > thr)
      {
	// 1 - 0 - 1
	info.cmd_num = 5;
	printf(" [command dicision] info.cmd = 5 : cd \n");
      }
    else if (info.x > thr && info.y > thr &&info.z > thr)
      {
	// 1 - 1 - 1
	info.cmd_num = 7;
	printf(" [command dicision] info.cmd = 7 : rm \n");
      }
    else
      {
	// 0 - 0 - 0
	info.cmd_num = 0;
	printf(" [command dicision] info.cmd = 0 : copy \n");
      }
  }

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
      rmdir(all_dir[info.arg1-1]);
    }
  else
    {
      char a[256];
      char b[2] = "/";
      sprintf(a, "%s%s%s", info.c_dir, b, all_dir[info.arg1-1]);
      rmdir(a);
    }
  return 0;
}

int my_open(char all_dir[8][256])
{
  if (strcmp(info.c_dir, "./") == 0)
    {
      system(strcat("open ", all_dir[info.arg1-1]));
    }
  else
    {
      char a[516];
      char b[2] = "/";
      sprintf(a, "%s%s%s%s", "open " ,info.c_dir, b, all_dir[info.arg1-1]);
      remove(a);
    }
}

int my_rm(char all_dir[8][256])
{
  if (strcmp(info.c_dir, "./") == 0)
    {
      remove(all_dir[info.arg1-1]);
    }
  else
    {
      char a[256];
      char b[2] = "/";
      sprintf(a, "%s%s%s", info.c_dir, b, all_dir[info.arg1-1]);
      remove(a);
    }
  return 0;
}

int my_cp(char all_dir[8][256])
{
  if (strcmp(info.c_dir, "./") == 0)
    {
      const char *fnamer = all_dir[info.arg1];
      const char *fnamew = strcat(all_dir[info.arg1], "_copy");
      fcopy(fnamer, fnamew);
    }
  else
    {
      const char *fnamer = strcat(info.c_dir, all_dir[info.arg1]);
      const char *fnamew = strcat(strcat(info.c_dir, all_dir[info.arg1]), "_copy");
      fcopy(fnamer, fnamew);
    }
  return 0;
}

int fcopy(const char *fnamer, const char *fnamew)
{
  int c;
  FILE *fpr = fopen(fnamer, "rb");
  FILE *fpw = fopen(fnamew, "wb");
  if (fpr == NULL || fpw == NULL)
    {
      printf("file open error!\n");
      return -1;
    }

  while ((c = fgetc(fpr)) != EOF)
    {
      fputc(c, fpw);
    }
  fclose(fpw);
  fclose(fpr);

  return 0;
}
