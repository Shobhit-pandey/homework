#include	<sys/types.h>	/* basic system data types */
#include	<sys/socket.h>	/* basic socket definitions */
#include	<sys/time.h>	/* timeval{} for select() */
#include	<time.h>		/* timespec{} for pselect() */
#include	<netinet/in.h>	/* sockaddr_in{} and other Internet defns */
#include	<arpa/inet.h>	/* inet(3) functions */
#include	<errno.h>
#include	<fcntl.h>		/* for nonblocking */
#include	<netdb.h>
#include	<signal.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include    <strings.h>     /* for bzero */
#include	<sys/stat.h>	/* for S_xxx file mode constants */
#include	<sys/uio.h>		/* for iovec{} and readv/writev */
#include	<unistd.h>
#include	<sys/wait.h>
#include	<sys/select.h>

#define LISTENQ 1024
#define MAXLINE 4096
#define MAXSOCKADDR 128
#define BUFFSIZE 4096

#define SERV_PORT 9879
#define SERV_PORT_STR "9879"


/* setting up a bitmap */

/* #define BYTES 536870912 */
/* uint8_t bitmap[BYTES]; */

/* uint8_t MASK[] = {1 << 0, 1 << 1, 1 << 2, 1 << 3, */
/*                   1 << 4, 1 << 5, 1 << 6, 1 << 7}; */

/* //check bit in bitmap */
/* bitmap[BYTE] & MASK[3]; */

/* //set bit in bitmap */
/* bitmap[354] | MASK[2]; */

/* //is the bit corresponding to 432935 set? */
/* bitmap[432935 / 8] & MASK[432935 % 8]; */

int main(int argc, char **argv)
{
	int i;
	int listenfd;
	int sockfd;

	int n;

	char buf[MAXLINE];
	socklen_t clilen;
	struct sockaddr_in cliaddr;
	struct sockaddr_in servaddr;

	
	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //192.168.1.101 ---- 101.1.168.192
	servaddr.sin_port = htons(SERV_PORT);

	bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	listen(listenfd, LISTENQ);

	clilen = sizeof(cliaddr);

	
	//for(;;){
		sockfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);

		for(;;){
			
			bzero(buf, MAXLINE);

			if( (n = read(sockfd, buf, MAXLINE)) == 0){
				//nothing was read, end of "file"
				close(sockfd);
				break;
			}else{
				fputs(buf, stdout);
				write(sockfd, buf, n);
			}
		}
		//}

	return 0;
}
