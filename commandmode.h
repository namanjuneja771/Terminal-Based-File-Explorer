#include<bits/stdc++.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <pwd.h>
#include <grp.h>
using namespace std;

vector<string> processString(string str2){
	string str1;
	vector<string> v1;	
	for(int i=0;i<int(str2.size());i++){
		if(str2[i]==' '){
			v1.push_back(str1);
			str1="";
			continue;
		}
		str1+=str2[i];
	}
	if(str1[0]=='~'){
		v1.push_back(str1);
	}
	else if(str1[0]=='/'){
		v1.push_back("."+str1);
	}
	else 
	v1.push_back(str1);
	return v1;
}
void remove_dir(char* path){
	DIR *directory;
	struct dirent *cur;
	directory=opendir(path);
	while ((cur = readdir(directory))!=NULL){
		string name=string(cur->d_name);		
		if((name=="..") or (name=="."))
		continue;
			string fin1=string(path)+"/"+name;			
			char * npath;
				if(cur->d_type==DT_DIR){
					remove_dir(const_cast<char*>(fin1.c_str()));			
				}
				else{
					remove(const_cast<char*>(fin1.c_str()));		
				}	
	}
	closedir(directory); 
	rmdir(path);
}
bool search(string name,char * path){
	DIR *directory;
	struct dirent *cur;
	directory=opendir(path);
	bool chk=false;
	while ((cur = readdir(directory))!=NULL){
		if(string(cur->d_name)=="." or string(cur->d_name)=="..") continue;
		if(string(cur->d_name)==name)
			return true;
		string fin1=string(path)+"/"+string(cur->d_name);
		if(cur->d_type==DT_DIR)
			chk=search(name,const_cast<char*>(fin1.c_str()));
		if(chk==true) return true;			
	}
	closedir(directory);
	return false;
}
void copyfile(char* name,char* path){
	char block;
	int in, out;
	int nread;
	in = open(name, O_RDONLY);
	out = open(path, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
	while((nread = read(in,&block,1)) > 0)
		write(out,&block,1);
	struct stat sourcestat,deststat;
}
void copydirectory(char* name,char* path){
	mkdir(path,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	DIR *directory;
	struct dirent *cur;
	directory=opendir(name);
	bool chk=false;
	while ((cur = readdir(directory))!=NULL){
		if( (string(cur->d_name)!= "..") and (string(cur->d_name) != ".") ){
			string fin1=string(path)+"/"+string(cur->d_name);
			if(cur->d_type==DT_DIR)
			copydirectory(cur->d_name,const_cast<char*>(fin1.c_str()));
			else
			copyfile(name,const_cast<char*>(fin1.c_str()));
		}	
	}
}
string commandmode(){
	char ch='a';
	while(ch!=27){
		string str2;
		ch = getchar();
		struct winsize win;
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);
		int rowsize = win.ws_row;
		printf("%c[%d;%dH", 27, rowsize, 1);
		cout<<"Command Mode: ";	
		while(((ch = getchar())!= 10) && ch!=27)
		{		
			
			if(ch==127)
			{  	printf("%c[%d;%dH",27,rowsize,1);
				printf("%c[2K", 27);
				cout<<"Command Mode: ";	
				if(str2.length()<=1)
				{
					str2="";
				}
				else{
					str2 = str2.substr(0,str2.length()-1);
				}
				cout<<str2;	
			}
			else{
				str2+= ch;
				cout<<ch;	
			}
			
		}
		if(ch==27) return const_cast<char*>(".");
		vector<string> parameters=processString(str2);
		if(parameters[0]=="create_file" and parameters.size()==3){
			char* path;		
			string st=parameters[2]+"/"+parameters[1];
			int status=open(const_cast<char*>(st.c_str()),O_RDONLY | O_CREAT,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH ); 	
			if (status == -1)
	    		{
				 cout<<"Error in creating new file ";	       
	    		}
		}
		else if(parameters[0]=="create_dir" and parameters.size()==3){
			char* path;
			string st=parameters[2]+"/"+parameters[1];
			cout<<parameters[0]<<" "<<parameters[1]<<" "<<parameters[2]<<" "<<st;	
			int status=mkdir(const_cast<char*>(st.c_str()),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH ); 	
			if (status == -1)
	    		{
				 cout<<"Error in creating new directory ";	       
	    		}
		}
		else if(parameters[0]=="delete_file" and parameters.size()==2){
			char* path;		
			int status=remove(const_cast<char*>(parameters[1].c_str())); 	
			if (status == -1)
	    		{
				 cout<<"Error in deleting file ";	       
	    		}
		}
		else if(parameters[0]=="delete_dir" and parameters.size()==2){
			char* path;		
			
			remove_dir(const_cast<char*>(parameters[1].c_str()));
		}
		else if(parameters[0]=="goto" and parameters.size()==2){
			return parameters[1];
			 
		}
		else if(parameters[0]=="search" and parameters.size()==2){
			string name=parameters[1];
			string st=".";		
			bool chk=search(name,const_cast<char*>(st.c_str()));
			if(chk==true) cout<<" True";
			else cout<<" False";
			while(ch!=97) ch=getchar();
		}
		else if(parameters[0]=="copy" and parameters.size()>=3){
			
			for(int i=1;i<int(parameters.size())-1;i++){
			string name=parameters[i];				
			struct stat s;
			stat(const_cast<char*>(("./"+name).c_str()),&s);
			if(s.st_mode & S_IFDIR)	copydirectory(const_cast<char*>(name.c_str()),const_cast<char*>((parameters[parameters.size()-1]+"/"+parameters[i]).c_str()));
			else	
	copyfile(const_cast<char*>(name.c_str()),const_cast<char*>((parameters[parameters.size()-1]+"/"+parameters[i]).c_str()));
			}
		}
		else if(parameters[0]=="move" and parameters.size()>=3){
			
			for(int i=1;i<int(parameters.size())-1;i++){
			string name=parameters[i];				
			struct stat s;
			stat(const_cast<char*>(("./"+name).c_str()),&s);
			if(s.st_mode & S_IFDIR){	copydirectory(const_cast<char*>(name.c_str()),const_cast<char*>((parameters[parameters.size()-1]+"/"+parameters[i]).c_str()));
remove_dir(const_cast<char*>(("./"+name).c_str()));
}			
			else	{
	copyfile(const_cast<char*>(name.c_str()),const_cast<char*>((parameters[parameters.size()-1]+"/"+parameters[i]).c_str()));
	remove(const_cast<char*>(name.c_str()));
}
			}
		}
		else if(parameters[0]=="rename" and parameters.size()==3){
			rename(parameters[1].c_str(),parameters[2].c_str());
		}
		printf("%c[%d;%dH",27,rowsize,1);
		printf("%c[2K", 27);
		cout<<"Command Mode: ";	
				
	}
}
