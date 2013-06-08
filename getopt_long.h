#include<stdio.h>
#include<stdlib.h>  
#include<string.h>  
#include<errno.h>  
#include<sys/types.h>  
#include<sys/socket.h>  
#include<netinet/in.h> 
#include<getopt.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<unistd.h>


#define DEFAULTPORT "80"
#define DEFAULTFILE "10"
#define MAXLEN 1024
#define MAXCLIENT 5
#define BACKLOG 5

const char* root_path="/home/neusoft/getopt_long/html";
char 	*port=0;
char	*file=0;
char *response_head ="HTTP/1.1 200 OK\r\n"                \
			"Content-Type: text/html\r\n"        \
			"Content-Length:%ld\r\n"            \
			"\r\n";
char *not_found     ="HTTP/1.1 404 Not Found\r\n"        \
			"Content-Type: text/html\r\n"        \
			"Content-Length: 40\r\n"            \
			"\r\n"                                \
			"<HTML><BODY>Page Fault</BODY></HTML>";
char *bad_request   ="HTTP/1.1 400 Bad Request\r\n"        \
			"Content-Type: text/html\r\n"        \
			"Content-Length: 39\r\n"            \
			"\r\n"                                \
			"<h1>Bad Request </h1>";
int set_socket(int port)
{
	struct sockaddr_in     server_addr;
	int sock_fd;
        if( (sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
        {    
                printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);    
                exit(0);    
        }    
        memset(&server_addr, 0, sizeof(server_addr));    
        server_addr.sin_family = AF_INET;    
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);    
        server_addr.sin_port = htons(port);    
        if( bind(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
        {    
                //printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);    
                //exit(0);    
		;
        }
	if( listen(sock_fd, BACKLOG) == -1)
 	{
		printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);
		exit(0);
	}
	return sock_fd;
}

void allocate_mem(char **s, int l, char *d)
{
        *s = malloc(l + 1);
        bzero(*s, l + 1);
        memcpy(*s, d, l);
}
 
void get_option(int argc,char *argv[]) 
{
	char *l_opt_arg,*ptr=0;
	char *const short_options="p:f:h";
	struct option long_options[]={
		{ "port",	1,   NULL,    'p'     },  
		{ "file",	1,   NULL,    'f'     },  
		{ "help",	0,   NULL,    'h'     },  
		{      0,     0,     0,     0},  
	};
	int c;  
	int option_index=0;
	while((c = getopt_long (argc, argv, short_options, long_options, &option_index)) != -1)	
	{
		if(optarg)
			printf("length===%d,%d\n",strlen(optarg),*optarg);
		switch (c){
		case 'p':
			ptr=port=malloc(strlen(optarg)+1);
			break;
		case 'f':
			ptr=file=malloc(strlen(optarg)+1);
			break;
		case 'h':
			printf("./getopt_long [option] ... file ...\n");
			printf("-p, --port set the port \n");
			printf("-f, --file set the file will be sent\n");
			printf("-h, --help for more help\n");
			break;
		default:
			break;
		}
		bzero(ptr,strlen(optarg)+1);
		memcpy(ptr,optarg,strlen(optarg)+1);
		printf("%s\n",port);	
	}  
}
void default_arg()
{
	int addr_len;
        if (!port) 
        {
                addr_len = strlen(DEFAULTPORT);
                allocate_mem(&port, addr_len, DEFAULTPORT);
        }
        if (!file) 
        {
                addr_len = strlen(DEFAULTFILE);
                allocate_mem(&file, addr_len, DEFAULTFILE);
        }
}

void read_conf(char *file_name)
{
	FILE *fp=fopen(file_name,"r");
	char a[100];
	char *ptr;
	char *ptr2;
	char *name;
	char *value;
	while(fgets(a,sizeof(a),fp))
	{
		if((ptr=strchr(a,'#'))!=NULL)
			*ptr='\0';	
		ptr=a;
		ptr=ptr+strspn(a," \t\r\n");
		if(*ptr!='\0')
		{
			ptr2=ptr+strcspn(ptr," \t\r\n");
			*ptr2='\0';
			name=ptr;
			value=strchr(ptr,'=');
			*value++='\0';
			if(strcasecmp(name,"hello")==0)
			{
				printf("%s=%s",name,value);
				//do something
			}
		}
	}
	fclose(fp);
}
