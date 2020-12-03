#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <string>
#include <termios.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <time.h>
#include <vector>
#include <stack>
#include <cstddef>
#include <pwd.h>
#include <grp.h>
#include <termios.h>
#include <limits.h>
#include "commandmode.h"
#define pos() printf("%c[%d;%dH", 27, xcor, ycor)
#define posx(x,y) printf("%c[%d;%dH", 27, x, y)
using namespace std;
int rowsize,colsize;
vector<string> dirvec(char* path){
DIR *mydir;
  struct dirent *myfile;
  struct stat fileStat;
  mydir=opendir(path);
  stat(path,&fileStat);
  vector<string> v1; 
  while((myfile=readdir(mydir))!=NULL)
      v1.push_back(string(myfile->d_name));
  closedir(mydir);
  return v1;
}
int count(char* path){
  DIR *mydir;
  struct dirent *myfile;
  struct stat fileStat;
  mydir=opendir(path);
  stat(path,&fileStat);
  int total=0; 
  while((myfile=readdir(mydir))!=NULL)
      total++;
  closedir(mydir);
  return total;
}
void display(char* path,int n,int xcor){
  int i=0;
  struct passwd *pw;
  struct group *gp;
  DIR *mydir;
  char *c;
  struct dirent *myfile;
  struct stat fileStat;
  mydir=opendir(path);
  stat(path,&fileStat);
  int total=0; 
  while(i<xcor-1 and (myfile=readdir(mydir))!=NULL){
	i++;
  }
  i=0;
  while((myfile=readdir(mydir))!=NULL and i<n)
  {
    i++;
    stat(myfile->d_name,&fileStat);  
    printf( (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
    printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
    printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
    printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-"); 
    printf(" ");
    pw=getpwuid(fileStat.st_uid);
    printf("%s ",pw->pw_name);
    gp=getgrgid(fileStat.st_gid);
    printf("%s ",gp->gr_name);
    printf("%4d ",int(fileStat.st_size));
    c=ctime(&fileStat.st_mtime);
    for(int j=4;j<=15;j++)
      printf("%c",c[j]);
    cout<<" ";
    printf("%s ",myfile->d_name);
    cout<<endl;
  }
  closedir(mydir);
}
int totalfileprint(int totalfiles){
	int len;	
	struct winsize win;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);
	rowsize = win.ws_row;
	colsize = win.ws_col;
	if (totalfiles>rowsize-1)
	{
		len=rowsize-1;
	}
	else
	{
		len=totalfiles;
	}	
	return len;
}
int main(){
  	string cur=".";
	stack<string> forward;
	stack<string> backward;
	backward.push(".");
	char ch;
	int xcor,ycor=1,wintrack,prev=1;	
	printf("%c[2J", 27);
	printf("%c[%d;%dH", 27, 1, 1);	
	int totalfiles=count(const_cast<char*>(cur.c_str()));
	int total=totalfileprint(totalfiles);					
	xcor=1;	
	wintrack=1;
	vector<string> v1=dirvec(const_cast<char*>(cur.c_str()));
	display(const_cast<char*>(cur.c_str()),total,1);
	
	printf("%c[%d;%dH", 27, 1, 1);
	fflush(stdout);
	struct termios initialrsettings, newrsettings;
	tcgetattr(fileno(stdin), &initialrsettings);
	newrsettings = initialrsettings;
	newrsettings.c_lflag &= ~ICANON;
	newrsettings.c_lflag &= ~ECHO;
	tcsetattr(0, TCSANOW, &newrsettings);		
while(1){
			ch = cin.get();
			if (ch == 27)
			{
				ch = cin.get();
				ch = cin.get();
				if (ch == 'A')
				{
					if (xcor + wintrack > 2)
					{
						xcor--;
						if (xcor>=wintrack)
						{
							posx(--prev,1);
						}
						else if (xcor<wintrack)
						{
							wintrack--;
							printf("%c[2J", 27);	
							fflush(stdout);						
							display(const_cast<char*>(cur.c_str()),total,wintrack);
							posx(1,1);
							prev=1;
							fflush(stdout);
						}
					}
				}
				else if (ch == 'B')
				{
					if (xcor<(totalfiles))
					{
						xcor++;
						if (xcor<=wintrack+total-1)
						{
							posx(++prev,1);
						}
						else
						{
							printf("%c[2J", 27);
							fflush(stdout);
							wintrack++;
							//cout<<"wintrack : "<<wintrack<<"***********";
							display(const_cast<char*>(cur.c_str()),total,wintrack);
							posx(rowsize-1,1);
							prev=rowsize-1;
							fflush(stdout);						
						}
					}
				}
				else if(ch=='C'){
					if(forward.size()>0){
						cur=const_cast<char*>((forward.top()).c_str());
						forward.pop();
						ycor=1;
						prev=1;	
						printf("%c[2J", 27);
						printf("%c[%d;%dH", 27, 1, 1);	
						totalfiles=count(const_cast<char*>(cur.c_str()));
						total=totalfileprint(totalfiles);					
						xcor=1;	
						wintrack=1;
						v1=dirvec(const_cast<char*>(cur.c_str()));
						display(const_cast<char*>(cur.c_str()),total,1);
						printf("%c[%d;%dH", 27,1, 1);
						fflush(stdout);						
					}

				}
				else if(ch=='D'){
					if(backward.size()>1){
						forward.push(backward.top());
						backward.pop();
						cur=const_cast<char*>((backward.top()).c_str());
						ycor=1;
						prev=1;	
						printf("%c[2J", 27);
						printf("%c[%d;%dH", 27, 1, 1);	
						totalfiles=count(const_cast<char*>(cur.c_str()));
						total=totalfileprint(totalfiles);					
						xcor=1;	
						wintrack=1;
						v1=dirvec(const_cast<char*>(cur.c_str()));
						display(const_cast<char*>(cur.c_str()),total,1);
						printf("%c[%d;%dH", 27,1, 1);
						fflush(stdout);						
					}

				}						
			}
			else if(ch=='k'){
				if(wintrack>1){
				int last=wintrack;
				int i=max(wintrack-total-1,0);
				wintrack=i+1;
				xcor=wintrack+prev-1;
				int j=i+total;
				for(int k=0;k<int(v1.size());k++) string a=v1[k];
				printf("%c[2J", 27);				
				for(i;i<j;i++){
					    
					    struct passwd *pw;
  					    struct group *gp;
					    char *c;
					    struct stat fileStat;
					    stat(const_cast<char*>(v1[i].c_str()),&fileStat);  
					    printf( (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
					    printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
					    printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
					    printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
					    printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
					    printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
					    printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
					    printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
					    printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
					    printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-"); 
					    printf(" ");
					    pw=getpwuid(fileStat.st_uid);
					    printf("%s ",pw->pw_name);
					    gp=getgrgid(fileStat.st_gid);
					    printf("%s ",gp->gr_name);
					    printf("%4d ",int(fileStat.st_size));
					    c=ctime(&fileStat.st_mtime);
					    for(int k=4;k<=15;k++)
					      printf("%c",c[k]);
					    cout<<" ";
					    cout<<v1[i]<<" "<<endl;
				}
				posx(prev,1);
			}
			}
			else if(ch=='l'){
				if(wintrack+total<totalfiles){
				int last=wintrack;
				int i=min(wintrack+total-1,totalfiles-total);
				wintrack=i+1;
				xcor=wintrack+prev-1;
				int j=i+total;
				printf("%c[2J", 27);				
				for(i;i<j;i++){
					    
					    struct passwd *pw;
  					    struct group *gp;
					    char *c;
					    struct stat fileStat;
					    stat(const_cast<char*>(v1[i].c_str()),&fileStat);  
					    printf( (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
					    printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
					    printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
					    printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
					    printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
					    printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
					    printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
					    printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
					    printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
					    printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-"); 
					    printf(" ");
					    pw=getpwuid(fileStat.st_uid);
					    printf("%s ",pw->pw_name);
					    gp=getgrgid(fileStat.st_gid);
					    printf("%s ",gp->gr_name);
					    printf("%4d ",int(fileStat.st_size));
					    c=ctime(&fileStat.st_mtime);
					    for(int k=4;k<=15;k++)
					      printf("%c",c[k]);
					    cout<<" ";
					    cout<<v1[i]<<" "<<endl;
				}
				posx(prev,1);
			}
			}
			else if(ch==10){
				struct stat fileStat;
				string cur1=cur+"/"+v1[xcor-1];
				stat(const_cast<char*>(cur1.c_str()),&fileStat);
				if(!S_ISDIR(fileStat.st_mode)){
					pid_t pid=fork();
					if(pid==0){
					execl("/usr/bin/xdg-open","xdg-open",const_cast<char*>(cur1.c_str()),NULL);
					}
				}
				else{
					
					backward.push(cur1);
					cur=cur1;
					ycor=1;
					prev=1;	
					printf("%c[2J", 27);
					printf("%c[%d;%dH", 27, 1, 1);	
					totalfiles=count(const_cast<char*>(cur.c_str()));
					total=totalfileprint(totalfiles);					
					xcor=1;	
					wintrack=1;
					v1=dirvec(const_cast<char*>(cur.c_str()));
					display(const_cast<char*>(cur.c_str()),total,1);
					printf("%c[%d;%dH", 27,1, 1);
					fflush(stdout);				
				}
			}
			else if(ch=='h'){
				if(backward.top()!="."){
					backward.push(".");
					cur=const_cast<char*>((backward.top()).c_str());
					ycor=1;
					prev=1;	
					printf("%c[2J", 27);
					printf("%c[%d;%dH", 27, 1, 1);	
					totalfiles=count(const_cast<char*>(cur.c_str()));
					total=totalfileprint(totalfiles);					
					xcor=1;	
					wintrack=1;
					v1=dirvec(const_cast<char*>(cur.c_str()));
					display(const_cast<char*>(cur.c_str()),total,1);
					printf("%c[%d;%dH", 27,1, 1);
					fflush(stdout);					
				}	
			}
			else if(ch==127){
				if(backward.top()!="."){
					string s="",s1="";
					string a=backward.top();
					for(int i=0;i<int(a.size());i++){
						if(a[i]=='/') {
							s+=s1;
							s1="/";
							continue;			
						}
						s1+=a[i];
					}
					backward.push(s);
					cur=const_cast<char*>((backward.top()).c_str());
					ycor=1;
					prev=1;	
					printf("%c[2J", 27);
					printf("%c[%d;%dH", 27, 1, 1);	
					totalfiles=count(const_cast<char*>(cur.c_str()));
					total=totalfileprint(totalfiles);					
					xcor=1;	
					wintrack=1;
					v1=dirvec(const_cast<char*>(cur.c_str()));
					display(const_cast<char*>(cur.c_str()),total,1);
					printf("%c[%d;%dH", 27,1, 1);
					fflush(stdout);
				}
			}
			else if(ch==58){
				cur=commandmode();
				if(cur!="."){
				backward.push(cur);
				}
				cur=backward.top();
				ycor=1;
				prev=1;	
				printf("%c[2J", 27);
				printf("%c[%d;%dH", 27, 1, 1);	
				totalfiles=count(const_cast<char*>(cur.c_str()));
				total=totalfileprint(totalfiles);					
				xcor=1;	
				wintrack=1;
				v1=dirvec(const_cast<char*>(cur.c_str()));
				display(const_cast<char*>(cur.c_str()),total,1);
				printf("%c[%d;%dH", 27,1, 1);
				fflush(stdout);
			}
				
				
	}	
return 0;
}
