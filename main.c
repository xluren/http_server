#include "getopt_long.h"
void deal_request(char *http_request,int connect_fd)
{
	char tmp_buf[MAXLEN];
	int html_fd;
	int file_len;
	struct stat file_stat;

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
int main(int argc,char *argv)
{
	struct sockaddr_in client_addr;
	int connect_fd,server_sockfd;
	int select_ret;
	int recv_ret;
	int i=0;
	int addr_len=sizeof(client_addr);
	char http_request[64]={'\0'};
	char recv_buf[1024];
	pid_t work_pid;
	
	read_conf("conf");	//test read configure module
	printf("current path is %s\n",getcwd(NULL,0)); //test chdir to root
	chdir(root_path);

	server_sockfd=set_socket(6550);
	
        printf("======waiting for client's request======\n");    

	fd_set read_set;
	struct timeval outtime_tv;
	int	connected_count=0;
	int	connected_client[MAXCLIENT]={0};
	
	//memset(connected_client,'0',MAXCLIENT);
	FD_ZERO(&read_set);
	
        while(1)
        {
		FD_ZERO(&read_set);
		FD_SET(server_sockfd, &read_set);
		outtime_tv.tv_sec=3;
		outtime_tv.tv_usec=0;

		//add connected client  to read_set
		for(i=0;i<MAXCLIENT;i++)
		{
			printf("fd---%d\n",connected_client[i]);  
			if(connected_client[i]!=0)
			{
				FD_SET(connected_client[i],&read_set);
			}
		}
		
		select_ret=select(MAXCLIENT+1,&read_set,NULL,NULL,/*&outtime_tv*/NULL);
		
		if(select_ret<0)
		{
			printf("select error\n");
			exit(0);
		}
		else if(select_ret==0)
		{
			printf("time_out\n");
			continue;
		}
		else
		{
			//read set can access
			for(i=0;i<MAXCLIENT;i++)
			{
				if(FD_ISSET(connected_client[i],&read_set))
				{
					recv_ret=recv(connected_client[i],recv_buf,sizeof(recv_buf),0);
					printf("the recv_buf is %s\n",recv_buf);
					if(recv_ret<=0)
					{
						printf("client %d closed\n",i);
						close(connected_client[i]);
						FD_CLR(connected_client[i],&read_set);
						connected_client[i]=0;
						connected_count--;
					}
					else
					{
						recv_buf[recv_ret]='\0';
						sscanf(recv_buf, "%*[^/]/%[^ ]",http_request);
                        			printf("the request---%s %d\n",http_request,strlen(http_request));
                        			deal_request(http_request,connected_client[i]);
					}
				}
			}
			if(FD_ISSET(server_sockfd,&read_set))
			{
				if( (connect_fd = accept(server_sockfd, (struct sockaddr*)&client_addr,&addr_len)) == -1)	
				{
					printf("connect error\n");
					continue;
				}
				if(connected_count<MAXCLIENT)
				{
					for(i=0;i<MAXCLIENT;i++)
					{
						if(connected_client[i]==0)
						{
							connected_client[i]=connect_fd;
							break;
						}
					}
					connected_count++;
					printf("new connection client[%d] %d:%d\n",
							connected_count,
							inet_ntoa(client_addr.sin_addr), 
							ntohs(client_addr.sin_port));  
				}
				else
				{
					printf("max connections arrive, exit\n");  
					send(connect_fd, "bye", 4, 0);
					close(connect_fd);
					continue;  
				}
			}		
		}
		
        }
	for (i = 0; i < BACKLOG; i++)   
        {  
		if(connected_client[i]!= 0)   
                {  
			close(connected_client[i]);  
                }  
        }   
	close(server_sockfd);
	return 0;
}	
