#include "getopt_long.h"
void deal_request(char *http_request,int connect_fd)
{
	char tmp_buf[MAXLEN];
	int html_fd;
	int file_len;
	struct stat file_stat;

	//printf("httprequest%s\n",strlen(http_request));
	char *response_head ="HTTP/1.1 200 OK\r\n"                \
                        "Content-Type: text/html\r\n"        \
                        "Content-Length:%ld\r\n"            \
                        "\r\n";
	char *not_found =    "HTTP/1.1 404 Not Found\r\n"        \
                        "Content-Type: text/html\r\n"        \
                        "Content-Length: 40\r\n"            \
                        "\r\n"                                \
                        "<HTML><BODY>Page Fault</BODY></HTML>";
	char *bad_request = "HTTP/1.1 400 Bad Request\r\n"        \
                        "Content-Type: text/html\r\n"        \
                        "Content-Length: 39\r\n"            \
                        "\r\n"                                \
                        "<h1>Bad Request </h1>";
	if(strlen(http_request)==0)
	{
		if((html_fd=open("html/index.html",O_RDONLY))==-1)
		{
			send(connect_fd, not_found, strlen(not_found),0);
			printf("index open failure!!\n");
		}
		else
		{
			stat("html/index.html", &file_stat);
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
	else
	{
		if((html_fd=open(http_request,O_RDONLY))==-1)
		{
			send(connect_fd, not_found, strlen(not_found),0);
			printf("index open failure!!\n");
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
}
int main(int argc,char *argv)
{
	struct sockaddr_in client_addr;
	int connect_fd,server_sockfd;
	int addr_len=sizeof(client_addr);
	char http_request[64]={'\0'};
	pid_t work_pid;

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
		char http_buf[1024];
		recv(connect_fd,http_buf,1024,0);
		printf("%s\n",http_buf);
		if((work_pid=fork())>0)
		{
			close(connect_fd);
			continue;
		}
		else if(work_pid==0)
		{
			close(server_sockfd);
			sscanf(http_buf, "%*[^/]/%[^ ]",http_request);
			printf("the request---%s %d\n",http_request,strlen(http_request));
			deal_request(http_request,connect_fd);
			bzero(http_request,64);
			exit(0);
		}
		else
			printf("error");
		
        } 
	close(server_sockfd);
	return 0;
}	
