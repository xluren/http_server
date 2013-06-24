#include "getopt_long.h"
void deal_request(char *http_request,int connect_fd)
{
	printf("fork id is %d\n",getpid());
	char tmp_buf[MAXLEN];
	int html_fd;
	int file_len;
	struct stat file_stat;

	printf("the ID is %d\n",connect_fd);
	if(strlen(http_request)==0)
		http_request="index.html";
	if((html_fd=open(http_request,O_RDONLY))==-1)
	{
		send(connect_fd, not_found, strlen(not_found),0);
		printf("index open failure!!the source is %s and the line is %d\n",__FILE__,__LINE__);
	}
	else
	{
		stat(http_request, &file_stat);
		file_len=file_stat.st_size;

		printf( "filelen = %d\n",file_len);
		FILE *connect_fp=fdopen(connect_fd,"w+");
		fprintf(connect_fp, response_head, file_len);
		fflush(connect_fp);
		int len;
		while((len = read(html_fd, tmp_buf, MAXLEN)) > 0)
		{
			send(connect_fd, tmp_buf, len,0);
			memset(tmp_buf, 0, MAXLEN);
		}
		close(html_fd);
	}

}
void get_request(int connect_fd)
{

	char http_buf[1024];
	char http_request[64]={'\0'};
	while(1)
	{
		int leng=recv(connect_fd,http_buf,1024,0);
		printf("------------------------%d----------------------------------------\n",leng);
		printf("%s\n",http_buf);
		printf("----------------------------------------------------------------\n");
		if(leng<=0)
			close(connect_fd);
		sscanf(http_buf, "%*[^/]/%[^ ]",http_request);
		printf("the request---%s %d\n",http_request,strlen(http_request));
		deal_request(http_request,connect_fd);
		bzero(http_request,64);
	}
	exit(0);
}
int main(int argc,char *argv)
{
	struct sockaddr_in client_addr;
	int connect_fd,server_sockfd;
	int addr_len=sizeof(client_addr);
	char http_request[64]={'\0'};
	pid_t work_pid;
	
	read_conf("conf");	
	printf("current path is %s\n",getcwd(NULL,0));
	chdir(root_path);

	server_sockfd=set_socket(6550);
	if( listen(server_sockfd, 10) == -1)
        {    
                printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);    
                exit(0);    
        }      
        printf("======waiting for client's request======\n");    
        while(1)
        {    
                if( (connect_fd = accept(server_sockfd, (struct sockaddr*)&client_addr,&addr_len)) == -1)
                {        
                        printf("accept socket error: %s(errno: %d)",strerror(errno),errno);        
                        continue;    
                }
	printf("the ID is %d\n",connect_fd);
		/*char http_buf[1024];
		recv(connect_fd,http_buf,1024,0);
		printf("----------------------------------------------------------------\n");
		printf("%s\n",http_buf);
		printf("----------------------------------------------------------------\n");*/
		if((work_pid=fork())>0)
		{
			//close(connect_fd);

			//continue;
		}
		else if(work_pid==0)
		{
			
			/*char http_buf[1024];
			while(1)
			{
				int leng=recv(connect_fd,http_buf,1024,0);
				printf("------------------------%d----------------------------------------\n",leng);
				printf("%s\n",http_buf);
				printf("----------------------------------------------------------------\n");
				if(leng<=0)
					close(connect_fd);
				close(server_sockfd);
				sscanf(http_buf, "%*[^/]/%[^ ]",http_request);
				printf("the request---%s %d\n",http_request,strlen(http_request));
				deal_request(http_request,connect_fd);
				bzero(http_request,64);
			}
			exit(0);*/
			close(server_sockfd);
			get_request(connect_fd);
		}
		else
			printf("error");
		
        } 
	close(server_sockfd);
	return 0;
}	
